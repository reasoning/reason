
/**
 * Reason
 * Copyright (C) 2009  Emerson Clarke
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>

#ifdef REASON_PLATFORM_POSIX
#include <unistd.h>
#define EBUSY 0
#endif

#include "reason/system/string.h"
#include "reason/network/http/http.h"
#include "reason/network/socket.h"
#include "reason/network/socketstream.h"
#include "reason/network/uri.h"
#include "reason/network/url.h"
#include "reason/system/storage/drive.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;
using namespace Reason::System::Storage;
using namespace Reason::Network::Http;
using namespace Reason::Network;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const int HttpConstants::StatusCount = 15;
const struct HttpConstants::StatusStruct HttpConstants::Status[] =
{
	{"200", "OK"},
	{"201", "Created"},
	{"202", "Accepted"},
	{"204", "No Content"},
	{"301", "Moved Permanently"},
	{"302", "Moved Temporarily"},
	{"304", "Not Modified"},
	{"400", "Bad Request"},
	{"401", "Unauthorized"},
	{"403", "Forbidden"},
	{"404", "Not Found"},
	{"500", "Internal Server Error"},
	{"501", "Not Implemented"},
	{"502", "Bad Gateway"},
	{"503", "Service Unavailable"}
};	

const int HttpConstants::HeaderCount = 49;
const char * HttpConstants::Header[] = 
{
	"Accept",
	"Accept-Charset",
	"Accept-Encoding",
	"Accept-Language",
	"Accept-Ranges",
	"Age",
	"Allow",
	"Authorization",
	"Cache-Control",
	"Connection",
	"Content-Encoding",
	"Content-Language",
	"Content-Length",
	"Content-Location",
	"Content-MD5",
	"Content-Range",
	"Content-Type",
	"Date",
	"ETag",
	"Expect",
	"Expires",
	"From",
	"Host",
	"If-Match",
	"If-Modified-Since",
	"If-None-Match",
	"If-Range",
	"If-Unmodified-Since",
	"Last-Modified",
	"Location",
	"Max-Forwards",
	"Pragma",
	"Proxy-Authenticate",
	"Proxy-Authorization",
	"Range",
	"Referer",
	"Retry-After",
	"Server",
	"TE",
	"Trailer",
	"Transfer-Encoding",
	"Upgrade",
	"User-Agent",
	"Vary",
	"Via",
	"Warning",
	"Www-Authenticate",
	"Set-Cookie",
	"Cookie",
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char HttpHeader::Separator[] = "\r\n";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HttpMessage::HttpMessage():
	Header(0),Body(0)
{
	Append((Line = new Path(true)));
	Append((Header = new HttpHeader()));
	Append((Body = new Path(true)));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool HttpMessageParser::Parse(int message)
{

	Message = message;

	if (!Handler || Eof()) return false;

	return ParseMessage();
}

bool HttpMessageParser::ParseMessage()
{
	if (Message.Is(HttpConstants::MESSAGE_LINE))
	{
		if (! (ParseRequest() || ParseResponse()))
			return false;
	}

	if (Message.Is(HttpConstants::MESSAGE_HEADER))
	{
		if (! ParseMessageHeader())
			return false;
	}

	if (Message.Is(HttpConstants::MESSAGE_BODY))
	{
		if (!ParseMessageBody())
			return false;
	}

	return true;
}

bool HttpMessageParser::ParseMessageHeader()
{
	while (!Eof() && !Is(HttpHeader::Separator))
	{
		if (!ParseField())
			return false;
	}

	return true;
}

bool HttpMessageParser::ParseMessageBody()
{
	if (Is(HttpHeader::Separator))
	{
		Skip(2);

		Mark();
		while (!Eof()) Next();
		Trap();

		Handler->ProcessBody(Token);

		return true;
	}

	return false;
}

bool HttpMessageParser::ParseChunk(String & body)
{

	while (!Eof())
	{

		int chunk = 0;
		ParseChunkSize(chunk);

		if (chunk)
		{

			if (!Is(HttpHeader::Separator))
			{
				OutputError("HttpMessageParser::ParseChunk - Missing separator after chunk data, line %d column %d",Line(),Column());
				return false;
			}

			Skip(2);
		}
		else
		{

			while (!Eof())
				ParseField();
		}

		if (!Is(HttpHeader::Separator))
		{
			OutputError("HttpMessageParser::ParseChunk - Missing separator after chunk data, line %d column %d",Line(),Column());
			return false;
		}

		Skip(2);

	}

	return true;
}

bool HttpMessageParser::ParseChunkSize(int & size)
{
	Mark();
	while (!Eof() && IsHex() && !Is(';') && !Is(HttpHeader::Separator))
		Next();
	Trap();

	int chunk = Token.Hex();
	if (chunk < 0 || (Token.StartsWith("0") && (!Token.Is("0") && Is(HttpHeader::Separator))))
	{

		OutputError("HttpMessageParser::ParseChunkSize - Invalid chunk size, line %d column %d",Line(),Column());
		return false;
	}

	while (Is(';'))
	{

		while (!Eof() && !(IsControl() && IsAny("()<>@,;:\"/[]?={}")) && !Is(HttpHeader::Separator))
			Next();

		if (Is("="))
		{
			Next();

			if (Is("\""))
			{
				ParseString();
			}
			else
			{

				while (!Eof() && !(IsControl() && IsAny("()<>@,;:\"/[]?={}")) && !Is(HttpHeader::Separator))
					Next();

			}

		}
	}

	if (!Is(HttpHeader::Separator))
	{
		OutputError("HttpMessageParser::ParseChunkSize - Missing separator after chunk size, line %d column %d",Line(),Column());
		return false;
	}

	Skip(2);

	return true;
}

bool HttpMessageParser::ParseRequest()
{

	if (Is("GET") || Is("HEAD") || Is("POST") || Is("PUT") || Is("DELETE") || Is("TRACE") || Is("CONNECT") || Is("OPTIONS") )
	{

		ParseWord();
		Substring method(Token);

		if (ParseWhitespace())
		{
			Mark();
			while(!Eof() && !IsWhitespace()) Next();
			Trap();

			Substring uri(Token);

			if (ParseWhitespace())
			{
				Mark();
				while(!Eof() && !IsWhitespace()) Next();
				Trap();

				Substring version(Token);

				if (Is(HttpHeader::Separator)) 
				{
					Skip(2);
					Handler->ProcessRequestLine(method,uri,version);
					return true;
				}
				else
				{
					Handler->Error("HttpMessageParser::ParseRequest","Missing separator, line %d column %d",Line(),Column());
				}
			}
			else
			{
				Handler->Error("HttpMessageParser::ParseRequest","Missing whitespace after uri, line %d column %d",Line(),Column());
			}

		}
		else
		{
			Handler->Error("HttpMessageParser::ParseRequest","Missing whitespace after method, line %d column %d",Line(),Column());
		}
	}

	return false;
}

bool HttpMessageParser::ParseResponse()
{

	if (Is("HTTP"))
	{
		Mark();
		while(!Eof() && !IsWhitespace()) Next();
		Trap();

		Substring version(Token);

		if (ParseWhitespace() && IsNumeric())
		{
			Mark();
			while(!Eof() && !IsWhitespace()) Next();
			Trap();

			Substring code(Token);

			if (ParseWhitespace())
			{
				Mark();
				while(!Eof() && !Is(HttpHeader::Separator)) Next();
				Trap();

				Substring phrase(Token);

				if (Is(HttpHeader::Separator)) 
				{
					Skip(2);
					Handler->ProcessResponseLine(version,code,phrase);
					return true;
				}
				else
				{
					Handler->Error("HttpMessageParser::ParseResponse","Missing separator, line %d column %d",Line(),Column());
				}
			}
			else
			{
				Handler->Error("HttpMessageParser::ParseResponse","Missing whitespace after status code, line %d column %d",Line(),Column());
			}

		}
		else
		{
			Handler->Error("HttpMessageParser::ParseResponse","Missing whitespace after version, line %d column %d",Line(),Column());
		}

	}

	return false;
}

bool HttpMessageParser::ParseField()
{

	{

		if (ParseFieldName())
		{
			Substring name(Token);
			ParseWhitespace();
			if (ParseFieldValue())
			{
				Substring value(Token);

				if (Is(HttpHeader::Separator)) 
				{
					Skip(2);
					Handler->ProcessField(name,value);
					return true;
				}
				else
				{
					Handler->Error("HttpMessageParser::ParseField","Missing separator, line %d column %d",Line(),Column());
				}

			}
			else
			{
				Handler->Warning("HttpMessageParser::ParseField","Missing field value, line %d column %d",Line(),Column());

				while(!Eof() && !Is(HttpHeader::Separator))
					Accept();

				if (Is(HttpHeader::Separator))
				{
					Skip(2);
					return true;
				}
			}
		}
		else
		{
			Handler->Warning("HttpMessageParser::ParseField","Missing field name, line %d column %d",Line(),Column());

			while(!Eof() && !Is(HttpHeader::Separator))
				Accept();

			if (Is(HttpHeader::Separator))
			{
				Skip(2);
				return true;
			}
		}
	}

	return false;
}

bool HttpMessageParser::IsToken()
{

	return !IsControl() && !IsSeparator();
}

bool HttpMessageParser::IsSeparator()
{

	return IsAny("()\"<>@,;:\\/[]?={}") || IsWhitespace();
}

bool HttpMessageParser::ParseFieldName()
{

	if (IsToken())
	{
		Mark();
		while (!Eof() && IsToken()) Next();
		Trap();

		ParseWhitespace();

		if (Is(":"))
		{	
			Next();
			return true;
		}
	}
	return false;
}

bool HttpMessageParser::ParseFieldValue()
{
	if (!IsWhitespace())
	{
		Mark();
		while (!Eof() && !Is(HttpHeader::Separator)) Next();
		Trap();
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HttpRequest::Initialise()
{

	Line->Append((Method = new Path(true)));
	Line->Append((Uri = new Path(true)));
	Line->Append((Version = new Path(true)));

	Host = new HttpField(Header,"Host: ");
	From = new HttpField(Header,"From: ");
	UserAgent = new HttpField(Header,"User-Agent: ");
	Accept = new HttpField(Header,"Accept: ");
	AcceptEncoding = new HttpField(Header,"Accept-Encoding: ");
	ContentType = new HttpField(Header,"Content-Type: ");
	ContentLength = new HttpField(Header,"Content-Length: ");
	Connection = new HttpField(Header,"Connection: ");
	Referer = new HttpField(Header,"Referer: ");

	Timeout = Sockets::TimeoutDefault;
}

HttpRequest::HttpRequest()
{
	Initialise();
	Allocate(HttpConstants::MessageSize);
}

HttpRequest::HttpRequest(const Url &url, int options)
{
	Initialise();
	Allocate(HttpConstants::MessageSize);
	Construct(url,options);
}

HttpRequest::~HttpRequest()
{
}

bool HttpRequest::Construct(Socket & socket, int message)
{

	SocketStream::StreamAdapter adapter(socket);

	StreamScanner scanner(adapter);

	HttpRequestReader reader(scanner,this);
	return reader.Parse(message);
}

bool HttpRequest::Construct(const Reason::Network::Url &url, int options)
{

	if (options & HttpConstants::METHOD)
	{		
		if( options & HttpConstants::METHOD_GET)
		{
			Method->Replace("GET");
		}
		else
		if (options & HttpConstants::METHOD_POST)
		{
			Method->Replace("POST");
		}
		else
		if (options & HttpConstants::METHOD_HEAD)
		{
			Method->Replace("HEAD");
		}
		else
		{
			OutputError("HttpRequest::Construct - Invalid or unspecified method in http options.\n");
			return false;
		}
	}
	else
	{

		Method->Replace("GET");
	}

	Uri->AttachBefore(" ");

	if( options & HttpConstants::PROXY )
	{

		Uri->Replace(url);	
	}
	else
	{

		if (url.Path->StartsWith("/"))
			Uri->Replace(((Url&)url).At(url.Path));
		else
			Uri->Replace("/" + ((Url&)url).At(url.Path));
	}

	Uri->Replace(" ","%20");

	Version->AttachBefore(" ");

	if(options & HttpConstants::VERSION_HTTP_1_1)
	{
		Version->Replace("HTTP/1.1");
	}
	else
	{

		Version->Replace("HTTP/1.0");
	}

	Host->Value(*url.Host);

	if (! url.Port->IsEmpty())
	{
		Host->Append(":");
		Host->Append(*url.Port);
	}		

	UserAgent->Value("Mozilla/5.0 (Windows; U; Windows NT 5.1; en-GB; rv:1.7.12) Gecko/20050919 Firefox/1.0.7");

	Accept->Value("*/*");

	if (options & HttpConstants::CONNECTION_KEEP_ALIVE)
	{

		Connection->Value("Keep-Alive");
	}
	else
	if (options & HttpConstants::CONNECTION_CLOSE)
	{

		Connection->Value("close");
	}

	Line->Append(HttpHeader::Separator);
	Header->Append(HttpHeader::Separator);

	return true;
}

bool HttpRequest::Deconstruct(char * data, int size)
{
	Assign((char*)data,size);
	return Deconstruct();
}

bool HttpRequest::Deconstruct()
{

	Path::Release(false);

	if (IsEmpty()) return false;

	StringScanner scanner(*this);
	HttpRequestReader reader(scanner,this);
	return reader.Parse();
}

void HttpRequest::Redirect(const Reason::Network::Url & url)
{
	Uri->Replace(((Url&)url).At(url.Path));						
	Host->Replace(*url.Host);
	if (!url.Port->IsEmpty())
	{
		Host->Append(":");
		Host->Append(*url.Port);
	}		
}

void HttpRequest::Cookies(Reason::Structure::Set<HttpCookie*> & set)
{

	Path::Enumerator enumerator(*Header);
	enumerator.Forward();
	while (enumerator.Has())
	{
		if (enumerator()->StartsWith("Cookie",true))
		{	

			Path * path = 0;
			while ((path = enumerator()) != 0 && path->Parent == Header)
			{	
				if (path->Is(HttpHeader::Separator))
					break;

				enumerator.Move();			

				path->Clear();
				Header->Delete(path);
			}

			enumerator.Move();
			if (path && path->Parent == Header)
			{
				path->Clear();
				Header->Delete(path);
			}

		}
		else
		{
			enumerator.Move();
		}	
	}

	String cookie;

	Iterator<HttpCookie*> iterator = set.Iterate();
	for (iterator.Forward();iterator.Has();iterator.Move())
	{
		cookie.Append(*iterator()->Name);
		cookie.Append("=");
		cookie.Append(*iterator()->Value);
		cookie.Append(";");
	}

	if (!cookie.IsEmpty())
		Header->Field(HttpConstants::HEADER_COOKIE,cookie);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HttpRequestReader::ProcessResponseLine(const Sequence & version, const Sequence & code, const Sequence & phrase)
{
	Abort();
}

void HttpRequestReader::ProcessRequestLine(const Sequence & method, const Sequence & uri, const Sequence & version)
{
	if (!((Sequence&)method).IsSequenceOf(Request->Data,Request->Size))
		*Request << method << " " << uri << " " << version << HttpHeader::Separator;

	Request->Method->Assign(method);
	Request->Uri->Assign(uri);
	Request->Version->Assign(version);
}

void HttpRequestReader::ProcessField(const Sequence & name, const Sequence & value)
{
	if (!((Sequence&)name).IsSequenceOf(Request->Data,Request->Size))
		*Request << name << ": " << value << HttpHeader::Separator;

	if (((Sequence &)name).Is("Accept",true))
	{
		Request->Accept->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Accept-Encoding",true))
	{
		Request->AcceptEncoding->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Connection",true))
	{
		Request->Connection->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Content-Type",true))
	{
		Request->ContentType->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Content-Length",true))
	{
		Request->ContentLength->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Date",true))
	{

	}
	else
	if (((Sequence &)name).Is("Server",true))
	{

	}
	else
	if (((Sequence &)name).Is("User-Agent",true))
	{
		Request->UserAgent->Assign(value);
	}
	else
	{
		Request->Header->Field(name,value);
	}
}

void HttpRequestReader::ProcessBody(const Sequence & body)
{
	if (!((Sequence&)body).IsSequenceOf(Request->Data,Request->Size))
		*Request << HttpHeader::Separator << HttpHeader::Separator << body;

	Request->Body->Assign(body);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HttpResponse::Initialise()
{
	State = RESPONSE_STATE_NONE;

	Line->Append((Version=new Path(true)));
	Line->Append((StatusCode=new Path(true)));
	Line->Append((ReasonPhrase=new Path(true)));

	Date = new HttpField(Header,"Date: ");
	Pragma = new HttpField(Header,"Pragma: ");

	Location = new HttpField(Header,"Location: ");	
	Server = new HttpField(Header,"Server: ");
	Authenticate = new HttpField(Header,"Authenticate: ");

	Allow = new HttpField(Header,"Allow: ");

	Connection = new HttpField(Header,"Connection: ");
	TransferEncoding = new HttpField(Header,"Transfer-Encoding: ");
	ContentEncoding = new HttpField(Header,"Content-Encoding: ");
	ContentLength = new HttpField(Header,"Content-Length: ");
	ContentType = new HttpField(Header,"Content-Type: ");
	Expires = new HttpField(Header,"Expires: ");
	LastModified = new HttpField(Header,"Last-Modified: ");

	Timeout = Sockets::TimeoutDefault;
}

HttpResponse::HttpResponse():
	Minimum(0),Maximum(0),Timeout(0)
{
	Initialise();
	Allocate(HttpConstants::MessageSize);
}

HttpResponse::HttpResponse(int code, int version):
	Minimum(0),Maximum(0),Timeout(0)
{
	Initialise();
	Allocate(HttpConstants::MessageSize);
	Construct(code,version);
}

HttpResponse::~HttpResponse()
{

}

void HttpResponse::Construct(int code, int version)
{
	if (version == HttpConstants::VERSION_HTTP_1_0)
	{
		Version->Replace("HTTP/1.0");
	}
	else
	{
		Version->Replace("HTTP/1.1");
	}

	if (StatusCode->IsEmpty())
	{
		StatusCode->AttachBefore(new Path(" "));
		StatusCode->Append(HttpConstants::Status[code].Number);
	}
	else
	{
		StatusCode->Replace(HttpConstants::Status[code].Number);
	}

	if (ReasonPhrase->IsEmpty())
	{
		ReasonPhrase->AttachBefore(new Path(" "));
		ReasonPhrase->Append(HttpConstants::Status[code].Phrase);
		ReasonPhrase->AttachAfter(new Path(HttpHeader::Separator));
	}
	else
	{
		ReasonPhrase->Replace(HttpConstants::Status[code].Phrase);	
	}	

	if (State == RESPONSE_STATE_NONE)
	{
		assert(strlen(Data) == Size);
		Server->Name("Server");
		Server->Value("Reason 3.14159");

		assert(strlen(Data) == Size);

		if (!Header->Field("Connection"))
			Header->Field("Connection","close");
	}

	if (!Body->Prev || !Body->Prev->Is(HttpHeader::Separator))
		Body->AttachBefore(new Path(HttpHeader::Separator));
}

void HttpResponse::Construct()
{
	OutputMessage("HttpResponse::Construct - Adding generic headers.\n");
	Construct(HttpConstants::STATUS_OK);
}

void HttpResponse::GuessContentType(char *data)
{
	StringParser parser(data);
	parser.SkipWhitespace();

	if (parser.Is("<!") || parser.Is("<html",true) || parser.Is("<head",true) || parser.Is("<body",true))
	{
		ContentType->Value("text/html");
	}
	else
	if (parser.Is("<?xml ",true))
	{
		ContentType->Value("application/xml");
	}
	else
	if (parser.Is("GIF8"))
	{
	    ContentType->Value("image/gif");
	}
	else
	if (parser.Is("#def"))
	{
	    ContentType->Value("image/x-bitmap");
	}
	else
	if (parser.Is("! XPM2"))
	{
	    ContentType->Value("image/x-pixmap");
	}
	else
	{

		String hex("1234");
		hex.Format("%c%c%c%c",0xFF,0xD8,0xFF,0xE0);
		if (parser.Is(hex))
		{
			ContentType->Value("image/jpeg");
			return;
		}

		hex.Format("%c%c%c%c",0xFF,0xD8,0xFF,0xEE);
		if (parser.Is(hex))
		{
			ContentType->Value("image/jpg");
			return;
		}

		hex.Format("%c%c",0xFE,0xFF);
		if (parser.Is(hex)) 
		{	

			if (parser.Is(2," < ? x"))
			{
				ContentType->Value("application/xml");
				return;
			}
		}

		hex.Format("%c%c",0xFF,0xFE);
		if (parser.Is(hex))
		{

			if (parser.Is(2,"< ? x "))
			{
				ContentType->Value("application/xml");
				return;
			}
		}
	}
}

void HttpResponse::GuessContentType(File &file)
{
	if (file.Extension().Contains("htm",true))
		 ContentType->Value("text/html");
	else
	if (file.Extension().Contains("xml",true))
		ContentType->Value("application/xml");
	else
	if (file.Extension().Contains("jpg",true))
		ContentType->Value("image/jpeg");
	else
	if (file.Extension().Contains("jpeg",true))
		ContentType->Value("image/jpeg");
	else
	if (file.Extension().Contains("gif",true))
		ContentType->Value("image/gif");

}

void HttpResponse::Deconstruct()
{

	Path::Release(false);

	if (IsEmpty()) return;

	StringScanner scanner(*this);
	HttpResponseReader reader(scanner,this);
	reader.Parse();

	State = RESPONSE_STATE_PARSED;
}

void HttpResponse::Deconstruct(char * data, int size)
{
	Assign((char*)data,size);
	Deconstruct();
}

void HttpResponse::Cookies(Reason::Structure::Set<HttpCookie*> & set)
{
	Path::Enumerator iterator(*Header);
	for (iterator.Forward();iterator.Has();iterator.Move())
	{	
		if (iterator()->StartsWith("Set-Cookie",true) && iterator.Move())
		{

			StringParser parser;
			parser.Assign(iterator.Reference());

			if (!parser.Eof())
			{	
				parser.Mark();
				while (!parser.Eof())
				{
					if (parser.Is("="))
						break;
					parser.Next();
				}
				parser.Trap();

				Reason::System::Substring name;
				name.Assign(parser.Token);
				name.Trim();

				if (name.IsEmpty())
					continue;

				parser.ParseWhitespace();
				if (parser.Is("="))
				{
					parser.Next();
					parser.ParseWhitespace();

					parser.Mark();
					while (!parser.Eof())
					{
						if (parser.Is(";"))
							break;
						parser.Next();
					}
					parser.Trap();

					HttpCookie * cookie = new HttpCookie();
					cookie->Name->Replace(name);
					cookie->Value->Replace(parser.Token);

					set.Update(cookie);

					while(! parser.Eof())
					{
						if (parser.ParseWord())
						{
							name.Assign(parser.Token);

							parser.ParseWhitespace();
							if (parser.Is("="))
							{
								parser.Next();
								parser.ParseWhitespace();

								parser.Mark();
								while (!parser.Eof())
								{
									if (parser.Is(";"))
										break;
									parser.Next();
								}
								parser.Trap();
							}
							else
							if (!name.Is("Secure",true))
							{
								while (!parser.Eof())
								{
									if (parser.Is(";"))
										break;
									parser.Next();
								}

								continue;
							}

							if (name.Is("Secure",true))
							{
								cookie->Secure->Replace("Secure");
							}
							else
							if (name.Is("Comment",true))
							{
								cookie->Comment->Replace(parser.Token);
							}
							else
							if (name.Is("Domain",true))
							{
								cookie->Domain->Replace(parser.Token);
							}
							else
							if (name.Is("Max-Age",true))
							{
								cookie->MaxAge->Replace(parser.Token);
							}
							else
							if (name.Is("Path",true))
							{
								cookie->Path->Replace(parser.Token);
							}
							else
							if (name.Is("Version",true))
							{
								cookie->Version->Replace(parser.Token);
							}
							else
							if (name.Is("Expires",true))
							{

								cookie->MaxAge->Replace(parser.Token);
							}

						}
						else
						{
							parser.Next();
						}
					}

				}

			}

		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HttpResponseReader::ProcessResponseLine(const Sequence & version, const Sequence & code, const Sequence & phrase)
{
	Response->Version->Assign(version);
	Response->StatusCode->Assign(code);
	Response->ReasonPhrase->Assign(phrase);
}

void HttpResponseReader::ProcessRequestLine(const Sequence & method, const Sequence & uri, const Sequence & version)
{
	Abort();
}

void HttpResponseReader::ProcessField(const Sequence & name, const Sequence & value)
{
	if (((Sequence &)name).Is("Date",true))
	{
		Response->Date->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Pragma",true))
	{
		Response->Pragma->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Location",true))
	{
		Response->Location->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Server",true))
	{
		Response->Server->Assign(value);
	}
	else
	if (((Sequence &)name).Is("WWW-Authenticate",true))
	{
		Response->Authenticate->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Allow",true))
	{
		Response->Allow->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Connection",true))
	{
		Response->Connection->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Transfer-Encoding",true))
	{
		Response->TransferEncoding->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Content-Encoding",true))
	{
		Response->ContentEncoding->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Content-Length",true))
	{
		Response->ContentLength->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Content-Type",true))
	{
		Response->ContentType->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Expires",true))
	{
		Response->Expires->Assign(value);
	}
	else
	if (((Sequence &)name).Is("Last-Modified",true))
	{
		Response->LastModified->Assign(value);
	}
	else
	{
		Response->Header->Field(name,value);
	}
}

void HttpResponseReader::ProcessBody(const Sequence & body)
{
	Response->Body->Assign(body);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HttpClient::HttpClient()
{

}

HttpClient::~HttpClient()
{

	Connection.Socket.Disconnect();
}

bool HttpClient::Exists(const Url &url)
{
	if (((Url&)url).IsEmpty()) return 0;

	Address addr(url);

	HttpRequest request;
	request.Construct(url);

	bool found = true;
	Socket socket;

	if (socket.Connect(addr))
	{
		if ( ! socket.Write(request.Data,request.Size) ) return 0;	

		HttpResponse response;
		response.Size = socket.Read(response.Data,64);

		if (response.Size == 0)
		{
			OutputError("HttpClient::Exists - No data was received.\n");
			found = false;
		}
		else
		{	
			response.Deconstruct();

			if ( ! response.StatusCode->Is(HttpConstants::Status[HttpConstants::STATUS_OK].Number))
			{
				OutputWarning("HttpClient::Exists - HTTP request failed. Status code [%s] Reason phrase [%s]\n",response.StatusCode->Print(),response.ReasonPhrase->Print());
				found = false;
			}		
		}
	}
	else
	{
		found = false;
	}

	socket.Disconnect();
	return found;
}

bool HttpClient::Process(const Url &url, HttpResponse &response)  
{
	HttpRequest request(url);
	return Process(url,request,response);
}

bool HttpClient::Process(const Url &url, HttpRequest& request, HttpResponse &response)
{
	if (!((Url&)url).IsValid())	 
	{
		OutputError("HttpClient::Process - Url is invalid.\n");	 
		return false;
	}

	Address address(url);	
	return Process(address,request,response);
}

bool HttpClient::Process(Address & address, HttpRequest& request, HttpResponse &response)
{
	if (Connection.Address.Host() != address.Host())
	{
		Connection.Construct(address);
	}

	bool processed = Process(Connection.Address, Connection.Socket,request,response);

	if ( !(request.Connection->Is("keep-alive",true) && response.Connection->Is("keep-alive",true)) && (request.Version->Is("HTTP/1.0") || request.Connection->Is("close",true) || response.Connection->Is("close",true)) )
		Connection.Socket.Disconnect();

	return processed;
}

bool HttpClient::Process(HttpConnection & connection, HttpRequest &request, HttpResponse & response)
{
	HttpContext context;
	return Process(connection.Address,connection.Socket,request,response,context);
}

bool HttpClient::Process(HttpConnection & connection, HttpRequest &request, HttpResponse & response, HttpContext & context)
{
	return Process(connection.Address,connection.Socket,request,response,context);
}

bool HttpClient::Process(Address & address, Socket & socket, HttpRequest& request, HttpResponse &response)
{
	HttpContext context;
	return Process(address,socket,request,response,context);
}

bool HttpClient::Process(Address & address, Socket & socket, HttpRequest& request, HttpResponse &response, HttpContext & context)
{

	HttpAuthentication & authentication = context.Authentication;

	int redirects = 0;
	bool processed = false;
	while (! processed)
	{
		if (socket.IsConnected() || socket.Connect(address))
		{	
			if (socket.Write(request.Data,request.Size, request.Timeout) ) 
			{

				if (!ProcessHeader(socket,response)) break;

				response.Cookies(context.Cookies);

				if (response.StatusCode->IsEmpty())
				{

					OutputError("HttpClient::Process - Missing status code. \nRequest:\n%s\n\nResponse:\n%s\n\n",request.Print(),response.Print());
				}

				if (response.StatusCode->Is(HttpConstants::Status[HttpConstants::STATUS_MOVED_TEMPORARILY].Number) ||
					response.StatusCode->Is(HttpConstants::Status[HttpConstants::STATUS_MOVED_PERMANENTLY].Number))
				{

					if ( ++redirects > HttpConstants::RedirectLimit )		
					{
						OutputError("HttpClient::Process - Maximum number of redirects was exceeded.\n");
						break;
					}
					else
					{
						OutputMessage("HttpClient::Process - Redirecting, Error %s %s.\n",response.StatusCode->Print(),response.ReasonPhrase->Print());

						Url absolute = "http://" + *request.Host;

						Url redirect = *response.Location;						

						if (redirect.IsRelative())
							redirect.Absolute(absolute);

						request.Redirect(redirect);
						request.Cookies(context.Cookies);

						address.Construct(redirect);

						socket.Release();
						response.Release();

						continue;
					}

				}
				else
				if (response.StatusCode->Is(HttpConstants::Status[HttpConstants::STATUS_UNAUTHORIZED].Number))
				{

					Path * scheme = response["WWW-Authenticate"];
					if (scheme)
					{
						if (scheme->Contains("Basic") && authentication.IsBasic())
						{
							String basic = authentication.Username + ":" + authentication.Password;
							Transcoder(basic).EncodeBase64();
							basic.Prepend("Basic ");
							request.Header->Field("Authorization",basic);
							response.Release();
							continue;
						}
					}
				}
				else
				if ( ! response.StatusCode->Is(HttpConstants::Status[HttpConstants::STATUS_OK].Number))
				{
					OutputWarning("HttpClient::Process - HTTP request failed. Status code [%s] Reason phrase [%s]\n",response.StatusCode->Print(),response.ReasonPhrase->Print());
					break;
				}
				else
				{

					if (!ProcessBody(socket,response)) break;
				}

				processed = true;
			}
			else
			{
				OutputWarning("HttpClient::Process - Could not send request.\n");
				break;
			}
		}
		else
		{
			OutputWarning("HttpClient::Process - Could not connect to address \"%s\".\n",address.ResolveHostname());
			break;
		}
	}

	if (request.Connection->Is("close",true) || response.Connection->Is("close"))
		socket.Disconnect();

	return processed;
}

bool HttpClient::ProcessHeader(Socket & socket, HttpResponse & response)
{

	if (response.Remaining() < 1024) response.Reserve(1024);
	response.Size = socket.Receive(response.Data,response.Remaining(),response.Timeout);

	if (response.Size > 0)
	{
		char * separator = "\r\n\r\n";
		while ( response.IndexOf(separator,4) == -1 )
		{
			if (response.Remaining() < 1024) response.Reserve(1024);
			int bytes = socket.Receive(response.PointerAt(response.Size),response.Remaining(),response.Timeout);					
			if (! (bytes > 0)) break;						
			response.Size += bytes;
		}
	}

	if (response.Size <= 0)
	{
		OutputError("HttpClient::ProcessHeader - No data was received.\n");
		return false;
	}
	else
	if (socket.IsTimeout())
	{
		OutputError("HttpClient::ProcessHeader - Socket timed out during read.\n");
		return false;
	}

	response.Deconstruct();
	return true;
}

bool HttpClient::ProcessBody(Socket & socket, HttpResponse & response)
{

	if (response.TransferEncoding->Is("chunked",true))
	{

		if (response.Size <= 0)
		{
			OutputError("HttpClient::ProcessHeader - No data was received.\n");
			return false;
		}
		else
		if (socket.IsTimeout())
		{
			OutputError("HttpClient::ProcessHeader - Socket timed out during read.\n");
			return false;
		}

	}
	else
	{

		if (!response.ContentLength->IsEmpty())
		{

			int remaining = response.Allocated - response.Size;
			int required = response.ContentLength->Integer();

			if ( (response.Maximum && required > response.Maximum) || (response.Minimum && required < response.Minimum) ) 
			{
				OutputError("HttpClient::ProcessBody - Response size was outside the allowed tolerance. Minimum [%d] Maximum [%d] Actual [%d]\n",response.Minimum,response.Maximum,required);
				return false;
			}

			required = (required > 0)?required-response.Body->Size:0;

			if (required > remaining)
			{

				response.Reserve(required);
			}

			OutputMessage("HttpClient::ProcessBody - Reserved %d required bytes, response size is currently %d with allocated %d\n",required,response.Size,response.Allocated);		
		}

		int bytes=0;
		do 
		{

			if (response.Remaining() < HttpConstants::MessageSize*2) response.Reserve(HttpConstants::MessageSize*2);
			bytes = socket.Receive(response.PointerAt(response.Size),response.Remaining(),response.Timeout);	
			if ( !(bytes > 0) ) break;	
			if ( (response.Maximum && response.Size > response.Maximum) ) break;
			response.Size += bytes;
		} 
		while(bytes > 0);

		if ( (response.Maximum && response.Size > response.Maximum) || (response.Minimum && response.Size < response.Minimum) ) 
		{
			OutputError("HttpClient::ProcessBody - Response size was outside the allowed tolerance. Minimum [%d] Maximum [%d] Actual [%d]\n",response.Minimum,response.Maximum,response.Size);
			return false;
		}

		if (socket.IsTimeout())
		{
			OutputError("HttpClient::ProcessBody - Socket timed out during read.\n");
			return false;
		}

		response.Release(false);
		response.Deconstruct();

		int length = response.ContentLength->Integer();
		if (length > 0 && response.Body->Size != length)
		{
			OutputError("HttpClient::ProcessBody - Response size did not match the content length, expected %d, actual %d.\n",length,response.Body->Size);
			return false;
		}
	}

	return true;
}

bool HttpClient::ProcessHead(const Url &url, HttpResponse &response)  
{
	HttpRequest request(url,HttpConstants::VERSION_HTTP_1_0|HttpConstants::METHOD_HEAD);
	return ProcessHead(url,request,response);
}

bool HttpClient::ProcessHead(const Url &url,HttpRequest & request, HttpResponse & response)
{
	OutputMessage("HttpClient::ProcessHead - Requesting head from [%s]\n",((Url&)url).Print());

	if (((Url&)url).IsEmpty() || !((Url&)url).IsValid())	
	{
		OutputError("HttpClient::ProcessHead - Invaid url.\n");	
		return false;
	}

	Address address(url);
	return ProcessHead(address,request,response);
}

bool HttpClient::ProcessHead(Address & address,HttpRequest & request, HttpResponse & response)
{
	if ( ! request.Method->StartsWith("HEAD"))
	{
		OutputError("HttpClient::ProcessHead - The request method must be HEAD.\n");
		return false;
	}

	bool found = false;
	int redirects = 0;
	Socket socket;

	while (! found)
	{
		if (socket.Connect(address))
		{
			if (socket.Write(request.Data,request.Size,request.Timeout) ) 
			{
				if (!ProcessHeader(socket,response)) break;

				if (response.StatusCode->Is(HttpConstants::Status[HttpConstants::STATUS_MOVED_TEMPORARILY].Number) ||
					response.StatusCode->Is(HttpConstants::Status[HttpConstants::STATUS_MOVED_PERMANENTLY].Number))
				{					
					if ( ++redirects > HttpConstants::RedirectLimit )		
					{
						OutputError("HttpClient::ProcessHead - Maximum number of redirects was exceeded.\n");
						break;
					}
					else
					{
						OutputMessage("HttpClient::ProcessHead - Redirecting, Error %s %s.\n",response.StatusCode->Print(),response.ReasonPhrase->Print());									

						Url absolute = "http://" + *request.Host;

						Url redirect;
						redirect.Construct(*response.Location);
						if (redirect.IsRelative())
							redirect.Absolute(absolute);

						request.Uri->Replace(*response.Location);
						request.Host->Replace(*redirect.Host);
						if (!redirect.Port->IsEmpty())
						{
							request.Host->Append(":");
							request.Host->Append(*redirect.Port);
						}	

						address.Construct(redirect);

						socket.Release();
						response.Release();

						continue;
					}

				}
				else
				if ( ! response.StatusCode->Is(HttpConstants::Status[HttpConstants::STATUS_OK].Number))
				{
					OutputWarning("HttpClient::ProcessHead - HTTP request failed. Status code [%s] Reason phrase [%s]\n",response.StatusCode->Print(),response.ReasonPhrase->Print());
					break;
				}

				found = true;
			}
		}
		else
		{
			OutputWarning("HttpClient::ProcessHead - Could not connect to address \"%s\".\n",address.ResolveHostname());
			break;
		}
	}

	socket.Disconnect();
	return found;

}

bool HttpClient::ProcessText(const Url &url, HttpResponse &response)
{
	HttpRequest request(url);
	return ProcessText(url,request,response);
}

bool HttpClient::ProcessText(const Url &url,HttpRequest & request, HttpResponse &response)
{
	OutputMessage("HttpClient::ProcessText - Requesting text from [%s]\n",((Url&)url).Print());

	if (((Url&)url).IsEmpty() || !((Url&)url).IsValid())	
	{
		OutputError("HttpClient::Process - Invalid url.\n");	
		return false;
	}

	Address address(url);
	return ProcessText(address,request,response);
}

bool HttpClient::ProcessText(Address & address, HttpRequest & request, HttpResponse &response)
{

	bool found = false;
	int redirects = 0;
	Socket socket;

	while (! found)
	{
		if (socket.Connect(address))
		{
			if (socket.Write(request.Data,request.Size,request.Timeout) ) 
			{
				if (!ProcessHeader(socket,response)) break;

				if (response.StatusCode->Is(HttpConstants::Status[HttpConstants::STATUS_MOVED_TEMPORARILY].Number) ||
					response.StatusCode->Is(HttpConstants::Status[HttpConstants::STATUS_MOVED_PERMANENTLY].Number))
				{					
					if ( ++redirects > HttpConstants::RedirectLimit )		
					{
						OutputError("HttpClient::ProcessText - Maximum number of redirects was exceeded.\n");
						break;
					}
					else
					{
						OutputMessage("HttpClient::ProcessText - Redirecting, Error %s %s.\n",response.StatusCode->Print(),response.ReasonPhrase->Print());

						Url absolute = "http://" + *request.Host;

						Url redirect;
						redirect.Construct(*response.Location);
						if (redirect.IsRelative())
							redirect.Absolute(absolute);
						address.Construct(redirect);

						request.Uri->Replace(*response.Location);
						request.Host->Replace(*redirect.Host);
						if (!redirect.Port->IsEmpty())
						{
							request.Host->Append(":");
							request.Host->Append(*redirect.Port);
						}	

						socket.Release();
						response.Release();

						continue;
					}

				}
				else
				if ( ! response.StatusCode->Is(HttpConstants::Status[HttpConstants::STATUS_OK].Number))
				{
					OutputWarning("HttpClient::ProcessText - HTTP request failed. Status code [%s] Reason phrase [%s]\n",response.StatusCode->Print(),response.ReasonPhrase->Print());
					break;
				}
				else
				if ( response.ContentType->Contains("text/") )
				{

					if (!ProcessBody(socket,response)) break;
				}
				else
				{
					OutputWarning("HttpClient::ProcessText - Closing request, content type was not text or header was too large.\n");
					break;
				}

				found = true;

			}
			else
			{
				OutputWarning("HttpClient::ProcessText - Could not send request.\n");
				break;
			}
		}
		else
		{
			OutputWarning("HttpClient::ProcessText - Could not connect to address \"%s\".\n",address.ResolveHostname());
			break;
		}
	}

	socket.Disconnect();
	return found;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HttpManager::HttpManager():ConnectionCount(0),ConnectionLimit(15),ListenSocket(Sockets::SOCKET_MODE_ASYNCHRONOUS)
{
}

HttpManager::~HttpManager() 
{
	if (ListenSocket.IsConnected())
		ListenSocket.Disconnect();
}

bool HttpManager::Startup(const char *address, int port)
{

	OutputMessage("HttpManager::Startup\n");
	ListenAddress.Construct(address,port);

	if (ListenSocket.Create() && ListenSocket.Bind(ListenAddress) && ListenSocket.Listen())
	{
		Sentinel.Enter();
		return true;
	}

	return false;	
}

bool HttpManager::Shutdown()
{
	OutputMessage("HttpManager::Shutdown.\n");

	Sentinel.Leave();

    while ( Threads.Count > 0 )
    {

		Thread::Sleep(1000);
    }

	return true;
}

void HttpManager::Run(void * thread)
{
	OutputMessage("HttpManager::Run - Waiting for incoming connection on thread [%08lX]\n",((Thread*)thread)->Id);

	Accept();	

	--ConnectionCount;

}

void HttpManager::Process(HttpConnection & connection)
{

	HttpRequest request;

	OutputMessage("HttpManager::Process - Receiving first chunk of data.\n");

	request.Size = connection.Socket.Receive(request.Data,request.Allocated);

	request.Deconstruct();
	int length=0;
	if (request.ContentLength && !request.ContentLength->IsEmpty())
		length = request.ContentLength->Integer();

	const char * separator = "\r\n\r\n";
	if ((length == 0 && request.IndexOf(separator) != (request.Size-4)) || (length > request.Body->Size))
	{
		int bytes=0;
		do 
		{
			if (request.Remaining() < (HttpConstants::MessageSize*2)) request.Reserve(HttpConstants::MessageSize*2);
			bytes = connection.Socket.Receive(request.PointerAt(request.Size),(length)?length-request.Body->Size:request.Remaining());	
			if ( !(bytes > 0) ) break;	
			request.Size += bytes;
		} 
		while(bytes > 0);
	}

	OutputMessage("HttpManager::Process - Request size = %d.\n",request.Size);

	if ( ! request.IsEmpty() )
	{

		HttpResponse response;
		response.Construct();

		if (request.Deconstruct())
		{

			Url url(*request.Uri);
			OutputMessage("HttpManager::Process - Processing requested uri [%s]\n",url.Print());

			int status=0;
			HttpContext context;
			for (Services.Forward();Services.Has();Services.Move())
			{
				if ((status=Services()->Process(connection,request,response,context)) != -1)
				{
					if (status != STATUS_OK)
						Error(response, status);

					break;
				}
			}		
		}
		else
		{

			Error(response, STATUS_BAD_REQUEST);
		}

		OutputMessage("HttpServer::Process - Response: \n%s\n",response.Print());
		OutputMessage("HttpServer::Process - Transmitting response.\n");

		connection.Socket.Write(response.Data,response.Size);
	}
	else
	{
		OutputError("HttpServer::Process - Remote connection was closed before any data was received.\n");
	}

	connection.Socket.Disconnect();
}

void HttpManager::Error(HttpResponse & response, int status)
{
	String page;

	if ( status == STATUS_NOT_FOUND)
	{		
		response.Construct(STATUS_NOT_FOUND);
		page = "web/404error.html";
	}
	else
	if (status == STATUS_BAD_REQUEST)
	{
		response.Construct(STATUS_BAD_REQUEST);
		page = ("web/400error.html");
	}

	String string;	
	string.Allocate(HttpConstants::MessageSize);

	File file(page);
	file.Read(string);
	file.Close();

	response.ContentType->Value("text/html");
	response.ContentLength->Value(string.Size);
	response.Body->Replace(string);
}

void HttpManager::Accept()
{

	HttpConnection remoteConnection;

	OutputMessage("HttpManager::Accept - Listening for socket connection.\n");

	do 
	{

		if (ListenSocket.Accept(remoteConnection.Socket,remoteConnection.Address))
		{
			++ConnectionCount;

			OutputMessage("HttpManager::Accept - Connection received from [%s].\n",remoteConnection.Address.Print());

			if (ConnectionCount < ConnectionLimit)
			{
				OutputMessage("HttpManager::Accept - Starting new thread for next connection.\n");

				Critical.Enter();
				Start();
				Critical.Leave();
			}

			OutputMessage("HttpManager::Accept - Processing request.\n");

			Process(remoteConnection);

			break;
		}

		Thread::Sleep(100);

	}
	while (Sentinel.Wait());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HttpPublisher::Process(HttpContext & context, HttpRequest & request, HttpResponse & response)
{

	Url url(*request.Uri);
	OutputMessage("HttpPublisher::Process - Processing requested file [%s]\n",url.Print());

	if (url.IsRelative())
	{

		url.Normalise();	

		bool secure = true;

		String buffer;
		buffer.Allocate(HttpConstants::MessageSize);

		Drive filesystem;
		String relative="";

		if ( secure && ! url.Path->IsEmpty() )
		{

			if ( url.Path->ReferenceAt(0) == '/' || url.Path->ReferenceAt(0) == '\\')
				url.Path->Prepend("web");
			else
				url.Path->Prepend("web/");

			filesystem.Construct(url.Path->Data,url.Size-(url.Path->Data-url.Data));
			relative = filesystem.Slice(filesystem.IndexOf("web")+3);
		}
		else
		{
			relative = url;

			if ( !url.Path->IsEmpty()  && url.Path->ReferenceAt(0) == '/' || url.Path->ReferenceAt(0) == '\\')
				url.Path->Prepend(".");

			filesystem.Construct(url.Path->Data,url.Size-(url.Path->Data-url.Data));
		}

		if (filesystem.Exists())
		{

			if (filesystem.IsFile())
			{
				File file(filesystem);

				response.Construct(STATUS_OK);
				response.GuessContentType(file);

				if (response.ContentType->Contains("text/"))
					file.Open(File::OPTIONS_OPEN_TEXT_READ);
				else
					file.Open(File::OPTIONS_OPEN_BINARY_READ);

				file.Read(buffer);
				file.Close();

				response.ContentLength->Value((int)file.Status().Size);

				if (file.Status().Size > buffer.Size)
				{
					OutputError("HttpPublisher::Process - Requested file was not read correctly.\n");
				}

				response.Body->Replace(buffer);
			}
			else
			{
				Folder folder(filesystem);						

				if (!filesystem.EndsWith("/")) filesystem.Append("/");
				filesystem.Append("index.html");
				filesystem.Construct();

				if ( filesystem.Exists() )
				{
					File file(filesystem);
					file.Read(buffer);
					file.Close();

					response.Construct(STATUS_OK);
					response.ContentType->Value("text/html");
					response.ContentLength->Value(buffer.Size);
					response.Body->Replace(buffer);
				}
				else
				{
					File file("web/browse.html");
					file.Read(buffer);
					file.Close();

					buffer.Replace("<hv:path/>",relative);

					String listing;
					listing.Allocate(buffer.Remaining()-512);

					folder.List();

					for(folder.Folders.Forward();folder.Folders.Has();folder.Folders.Move())
					{

						if (!relative.IsEmpty())
						{

							listing << "<tr class=row>";
							listing << "<td class=date>" << Time(folder.Folders().Status().Accessed).Print() << "</td>";
							listing << "<td class=info>Directory</td>";
							listing << "<td class=name><a href=\"";
							listing << relative << "/" << *folder.Folders().Name;
							listing << "\">" << *folder.Folders().Name << "</a></td>";
							listing << "</tr>";
						}
					}

					for(folder.Files.Forward();folder.Files.Has();folder.Files.Move())
					{
						if (!relative.IsEmpty())
						{
							listing << "<tr class=row>";
							listing << "<td class=date>" << Time(folder.Files().Status().Accessed).Print() << "</td>";
							listing << "<td class=info>" << folder.Files().Status().Size << "</td>";
							listing << "<td class=name><a href=\"";
							listing << relative << "/" << folder.Files().Basename();
							listing << "\">" << folder.Files().Basename() << "</a></td>";									
							listing << "</tr>";
						}
					}

					buffer.Replace("<hv:listing/>",listing);

					response.Construct(STATUS_OK);
					response.ContentType->Value("text/html");
					response.ContentLength->Value(buffer.Size);
					response.Body->Replace(buffer);		
				}

			}
		}
		else
		{
			return STATUS_NOT_FOUND;
		}

		return STATUS_OK;
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HttpServer::HttpServer():ListenSocket(Sockets::SOCKET_MODE_ASYNCHRONOUS)
{
	ConnectionLimit = 15;
	ConnectionCount = 0;
}

HttpServer::~HttpServer() 
{
	if (ListenSocket.IsConnected())
		ListenSocket.Disconnect();
}

bool HttpServer::Startup(const Sequence & address, int port)
{
	OutputMessage("HttpServer::Startup\n");
	ListenAddress.Construct(address,port);
	if (ListenSocket.Create() && ListenSocket.Bind(ListenAddress) && ListenSocket.Listen())
	{
		Sentinel.Lock();
		return true;
	}

	return false;	
}

bool HttpServer::Startup(const char *address, int port)
{

	OutputMessage("HttpServer::Startup\n");
	ListenAddress.Construct(address,port);
	if (ListenSocket.Create() && ListenSocket.Bind(ListenAddress) && ListenSocket.Listen())
	{
		Sentinel.Lock();
		return true;
	}

	return false;	
}

bool HttpServer::Shutdown()
{
	OutputMessage("HttpServer::Shutdown - Shutting down HTTP server.\n");

	Sentinel.Unlock();

    while ( Threads.Count > 0 )
    {

		Thread::Sleep(100);
    }

	return true;
}

void HttpServer::Run(void * thread)
{
	int id = ((Thread*)thread)->Id;
	OutputMessage("HttpServer::Run - Waiting for incoming connection on thread [%08lX]\n",id);

	Accept();	

	--ConnectionCount;
}

void HttpServer::ProcessRequest(HttpConnection & connection)
{

	HttpRequest request;

	OutputMessage("HttpServer::ProcessRequest - Receiving first chunk of data.\n");

	request.Size = connection.Socket.Receive(request.Data,request.Allocated);

	char * separator = "\r\n\r\n";
	while ( ( request.IndexOf(separator,4) == -1 ) )
	{

		int bytesReceived = connection.Socket.Receive(request.PointerAt(request.Size),request.Remaining());
		if (bytesReceived == 0) break;
		request.Size += bytesReceived;
	}

	OutputMessage("HttpServer::ProcessRequest - Request size = %d.\n",request.Size);

	if ( ! request.IsEmpty() )
	{

		String buffer;
		buffer.Allocate(HttpConstants::MessageSize);
		HttpResponse response;

		if (request.Deconstruct())
		{
			if (!request.ContentLength->IsEmpty())
			{
				int remaining = request.Allocated - request.Size;
				int required = request.ContentLength->Integer();
				if (!request.Body->IsEmpty())
					required -= request.Body->Size;

				if (required > remaining)
					request.Reserve(required);

				request.Size += connection.Socket.Read(request.Data+request.Size,required);
				request.Body->Assign(request.After(separator,4));
			}

			Url url(*request.Uri);
			OutputMessage("HttpServer::ProcessRequest - Processing requested file [%s]\n",url.Print());

			if (url.IsRelative())
			{

				url.Normalise();	

				bool secure = false;

				Drive filesystem;
				String relative="";

				if ( secure && ! url.Path->IsEmpty() )
				{

					if ( url.Path->ReferenceAt(0) == '/' || url.Path->ReferenceAt(0) == '\\')
						url.Path->Prepend("web");
					else
						url.Path->Prepend("web/");

					filesystem.Construct(url.Path->Data,url.Size-(url.Path->Data-url.Data));
					relative = filesystem.Slice(filesystem.IndexOf("web")+3);
				}
				else
				{
					relative = url;

					if ( url.Path->ReferenceAt(0) == '/' || url.Path->ReferenceAt(0) == '\\')
						url.Path->Prepend(".");

					filesystem.Construct(url.Path->Data,url.Size-(url.Path->Data-url.Data));
				}

				if (filesystem.Exists())
				{

					if (filesystem.IsFile())
					{
						File file(filesystem);

						response.Construct(HttpConstants::STATUS_OK);
						response.GuessContentType(file);

						if (response.ContentType->Contains("text/"))
							file.Open(File::OPTIONS_OPEN_TEXT_READ);
						else
							file.Open(File::OPTIONS_OPEN_BINARY_READ);

						file.Read(buffer);
						file.Close();

						response.ContentLength->Value((int)file.Status().Size);

						if (file.Status().Size > buffer.Size)
						{
							OutputError("HttpServer::ProcessRequest - Requested file was not read correctly.\n");
						}

					}
					else
					{
						Folder folder(filesystem);						

						if (!filesystem.EndsWith("/")) filesystem.Append("/");
						filesystem.Append("index.html");
						filesystem.Construct();

						if ( filesystem.Exists() )
						{
							File file(filesystem);
							file.Open(File::MODE_OPEN);
							file.Read(buffer);
							file.Close();

							response.Construct(HttpConstants::STATUS_OK);
							response.ContentType->Value("text/html");
							response.ContentLength->Value(buffer.Size);

						}
						else
						{
							File file;
							file.Construct("web/browse.html");
							file.Open();
							file.Read(buffer);
							file.Close();

							buffer.Replace("<hv:path/>",relative);

							String listing;
							listing.Allocate(buffer.Remaining()-512);

							folder.List();

							for(folder.Folders.Forward();folder.Folders.Has();folder.Folders.Move())
							{

								if (!relative.IsEmpty())
								{

									listing << "<tr class=row>";
									listing << "<td class=date>" << Time(folder.Folders().Status().Accessed).Print() << "</td>";
									listing << "<td class=info>Directory</td>";
									listing << "<td class=name><a href=\"";
									listing << relative << "/" << *folder.Folders().Name;
									listing << "\">" << *folder.Folders().Name << "</a></td>";
									listing << "</tr>";
								}
							}

							for(folder.Files.Forward();folder.Files.Has();folder.Files.Move())
							{
								if (!relative.IsEmpty())
								{								
									listing << "<tr class=row>";
									listing << "<td class=date>" << Time(folder.Files().Status().Accessed).Print() << "</td>";
									listing << "<td class=info>" << folder.Files().Status().Size << "</td>";
									listing << "<td class=name><a href=\"";
									listing << relative << "/" << folder.Files().Basename();
									listing << "\">" << folder.Files().Basename() << "</a></td>";									
									listing << "</tr>";
								}
							}

							buffer.Replace("<hv:listing/>",listing);

							response.Construct(HttpConstants::STATUS_OK);
							response.ContentType->Value("text/html");
							response.ContentLength->Value(buffer.Size);
						}

					}

				}
				else
				{
					response.Construct(HttpConstants::STATUS_NOT_FOUND);

					File errorFile;
					errorFile.Construct("web/404error.html");
					errorFile.Open(File::MODE_OPEN);
					errorFile.Read(buffer);
					errorFile.Close();

					response.ContentType->Value("text/html");
					response.ContentLength->Value(buffer.Size);
				}
			}
			else
			{

			}
		}
		else
		{

			response.Construct(HttpConstants::STATUS_BAD_REQUEST);

			File errorFile;
			errorFile.Construct("web/400error.html");
			errorFile.Open(File::MODE_OPEN);
			errorFile.Read(buffer);
			errorFile.Close();

			response.ContentType->Value("text/html");
			response.ContentLength->Value(buffer.Size);

		}

		OutputMessage("HttpServer::ProcessRequest - Response: \n%s\n",response.Print());

		response.Construct();

		response.Body->Replace(buffer);

		OutputMessage("HttpServer::ProcessRequest - Transmitting response.\n");
		connection.Socket.Write(response.Data,response.Size);
	}
	else
	{
		OutputError("HttpServer::ProcessRequest - Remote connection was closed before any data was recieved.\n");
	}

	connection.Socket.Disconnect();
}

void HttpServer::Accept()
{

	HttpConnection connection;

	OutputMessage("HttpServer::Accept - Listening for socket connection.\n");

	do 
	{

		if (ListenSocket.Accept(connection.Socket, connection.Address))
		{
			++ConnectionCount;

			OutputMessage("HttpServer::Accept - Connection received from [%s].\n",connection.Address.Print());

			if (Sentinel.Wait())
			{

				if (ConnectionCount < ConnectionLimit)
				{
					OutputMessage("HttpServer::Accept - Starting new thread for next connection.\n");
					Start();
				}

				OutputMessage("HttpServer::Accept - Processing request.\n");

				ProcessRequest(connection);
			}

			break;
		}

		Thread::Sleep(100);

		OutputMessage("HttpServer::Accept - Waiting.\n");

	}
	while (Sentinel.Wait());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HttpAgent::HttpAgent():
	Sleeping(0)
{
	Sentinel.Lock();

}

HttpAgent::~HttpAgent()
{
	Sentinel.Unlock();

	bool exiting = true;
	while(exiting)
	{
		Critical.Enter();
		exiting = Threads.Count;
		Critical.Leave();
		Thread::Sleep(100);
	}
}

bool HttpAgent::Active()
{
	Critical::Auto lock(Critical);
	return Threads.Count > 0 && (Sleeping < Threads.Count || Has());
}

bool HttpAgent::Enqueue(const Link & link)
{

	Critical::Auto lock(Critical);

	while (Queue.Count < link.Depth+1)
		Queue.Append(Reason::Structure::Set<Link>());

	if (!Queue[link.Depth].Insert(link))
		return false;

	return true;
}

bool HttpAgent::Dequeue(Link & link)
{
	Critical::Auto lock(Critical);
	Iterator< Reason::Structure::Set<Link> > iterator = Queue.Iterate();

	int depth=0;
	for (iterator.Forward();iterator.Has();iterator.Move())
	{
		if (iterator().Count > 0)
		{
			link = iterator().Minimum()();
			iterator().Delete(link);

			if (iterator().Count == 0 && depth == Queue.Count-1)
			{

				Queue.DeleteAt(depth);
			}

			return true;
		}
	}

	return false;
}

bool HttpAgent::Has(const Link & link)
{
	Critical::Auto lock(Critical);
	Iterator< Reason::Structure::Set<Link> > iterator = Queue.Iterate();
	for (iterator.Forward();iterator.Has();iterator.Move())
	{
		if (iterator().Contains(link))
			return true;
	}

	return false;
}

bool HttpAgent::Has()
{
	Critical::Auto lock(Critical);
	Iterator< Reason::Structure::Set<Link> > iterator = Queue.Iterate();
	for (iterator.Forward();iterator.Has();iterator.Move())
	{
		if (iterator().Count > 0)
			return true;
	}

	return false;
}

int HttpAgent::Count()
{
	Critical::Auto lock(Critical);
	int count=0;
	Iterator< Reason::Structure::Set<Link> > iterator = Queue.Iterate();
	for (iterator.Forward();iterator.Has();iterator.Move())
		count += iterator().Count;

	return count;
}

void HttpAgent::Run(void * thread)
{
	int id = ((Thread*)thread)->Id;

	Link link;
	int inactivity=0;
	int state=AGENT_STATE_INITIAL;
	do 
	{

		Critical.Enter();
		if (!Has())
		{
			if (state != AGENT_STATE_SLEEPING)
			{
				++Sleeping;
				state = AGENT_STATE_SLEEPING;
			}
		}
		else
		{
			if (state == AGENT_STATE_SLEEPING)
				--Sleeping;

			Dequeue(link);

			state = AGENT_STATE_PROCESSING;
		}
		Critical.Leave();

		switch(state)
		{
		case AGENT_STATE_SLEEPING: 
			Thread::Sleep(100); 
			break;
		case AGENT_STATE_PROCESSING: 
			Process(link); 
			break;
		}
	}
	while (Sentinel.Wait());

	if (thread)
	{
		OutputMessage("HttpAgent::Run - Thread [%08lX] has stopped.\n",id);	
	}

}	

void HttpAgent::Process(Link & link)
{
	HttpResponse response;
	HttpRequest request;
	request.Construct(link,HttpConstants::VERSION_HTTP_1_1|HttpConstants::METHOD_GET);

	String referer("Referer: ");
	referer.Append((const Sequence&)link.Slice(0,link.LastIndexOf('/')+1));
	request.UserAgent->Replace("User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.7) Gecko/20040803 Firefox/0.9.3\n");

	OutputMessage("HttpAgent::Process - Request: \n%s\n",request.Print());

	HttpClient http;
	if ( ! http.Process(link,request,response))
	{
		OutputMessage("HttpAgent::Process - Could not load url %s.\n",link.Print());
		OutputMessage("HttpAgent::Process - Response: \n%s\n",response.Print());
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HttpCookie::HttpCookie()
{
	Append(Name = new Reason::System::Path(true));
	Append(Value = new Reason::System::Path(true));
	Append(Comment = new Reason::System::Path(true));
	Append(Domain = new Reason::System::Path(true));
	Append(MaxAge = new Reason::System::Path(true));
	Append(Path = new Reason::System::Path(true));
	Append(Secure = new Reason::System::Path(true));
	Append(Version = new Reason::System::Path(true));
}

int HttpCookie::Compare(Reason::System::Object * object, int comparitor)
{
	return Name->Compare(((HttpCookie*)object)->Name);
}

void HttpCookie::Load(Stream & reader)
{
	Persistence persistence(reader);
	if (persistence.ReadClass("HttpCookie"))
	{

		persistence.ReadPath(*this);
	}
}

void HttpCookie::Store(Stream & writer)
{
	Persistence persistence(writer);
	persistence.WriteClass("HttpCookie");
	persistence.WritePath(*this);

}

void HttpCookie::Print(String & string)
{
	string << *Name << "=\"" << *Value << "\"";

	if (Comment->IsActivated())
		string << ";Comment=\"" << *Comment << "\"";

	if (Domain->IsActivated())
		string << ";Domain=\"" << *Domain << "\"";

	if (MaxAge->IsActivated())
		string << ";Max-Age=\"" << *MaxAge << "\"";

	if (Path->IsActivated())
		string << ";Path=\"" << *Path << "\"";

	if (Secure->IsActivated())
		string << ";Secure";

	if (Version->IsActivated())
		string << ";Version=\"" << *Version << "\"";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HttpForm::Construct(HttpRequest & request)
{

	Method = METHOD_NONE;
	Fields.Release();

	Path * path = request.ContentType;
	if (path)
	{
		if (path->Is("application/x-www-form-urlencoded"))
		{
			Method.On(METHOD_POST_URLENCODE);

			String body = *request.Body;
			body.Trim();

			Url url = "http://localhost?";
			url.Append(body);
			url.Construct();
			url.Parameters(Fields);

		}
		else
		if (path->Is("multipart/form-data"))
		{
			Method.On(METHOD_POST_MULTIPART);

		}
	}

	{

		int count = Fields.Count;
		Url url = *request.Uri;
		url.Parameters(Fields);

		if (Fields.Count > count)
			Method.On(METHOD_GET);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

