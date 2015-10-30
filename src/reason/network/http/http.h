
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
#ifndef PROTOCOL_HTTP_H
#define PROTOCOL_HTTP_H

#include "reason/language/xml/xml.h"	

#include "reason/network/socket.h"
#include "reason/network/link.h"

#include "reason/platform/thread.h"
#include "reason/system/file.h"
#include "reason/system/stream.h"
#include "reason/system/path.h"
#include "reason/system/string.h"
#include "reason/system/parser.h"
#include "reason/structure/map.h"
#include "reason/structure/enumerator.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;
using namespace Reason::Platform;

using namespace Reason::Network;
using namespace Reason::Language;
using namespace Reason::Structure;

namespace Reason { namespace Network { namespace Http {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpConstants
{
public:

	const static int MessageSize	= 4096;		
	const static int RedirectLimit	= 5;		

	enum Status
	{	
		STATUS_OK = 0,
		STATUS_CREATED,
		STATUS_ACCEPTED,
		STATUS_NO_CONTENT,
		STATUS_MOVED_PERMANENTLY,
		STATUS_MOVED_TEMPORARILY,
		STATUS_NOT_MODIFIED,
		STATUS_BAD_REQUEST,
		STATUS_UNAUTHORIZED,
		STATUS_FORBIDDEN,
		STATUS_NOT_FOUND,
		STATUS_INTERNAL_SERVER_ERROR,
		STATUS_NOT_IMPLEMENTED,
		STATUS_BAD_GATEWAY,
		STATUS_SERVICE_UNAVAILABLE	
	};

	static const int StatusCount;
	static const struct StatusStruct
	{
		char *Number;
		char *Phrase;
	} Status[];

	enum Header
	{
		HEADER_ACCEPT = 0,
		HEADER_ACCEPT_CHARSET,
		HEADER_ACCEPT_ENCODING,
		HEADER_ACCEPT_LANGUAGE,
		HEADER_ACCEPT_RANGES,
		HEADER_AGE,
		HEADER_ALLOW,
		HEADER_AUTHORIZATION,
		HEADER_CACHE_CONTROL,
		HEADER_CONNECTION,
		HEADER_CONTENT_ENCODING,
		HEADER_CONTENT_LANGUAGE,
		HEADER_CONTENT_LENGTH,
		HEADER_CONTENT_LOCATION,
		HEADER_CONTENT_MD5,
		HEADER_CONTENT_RANGE,
		HEADER_CONTENT_TYPE,
		HEADER_DATE,
		HEADER_ETAG,
		HEADER_EXPECT,
		HEADER_EXPIRES,
		HEADER_FROM,
		HEADER_HOST,
		HEADER_IF_MATCH,
		HEADER_IF_MODIFIED_SINCE,
		HEADER_IF_NONE_MATCH,
		HEADER_IF_RANGE,
		HEADER_IF_UNMODIFIED_SINCE,
		HEADER_LAST_MODIFIED,
		HEADER_LOCATION,
		HEADER_MAX_FORWARDS,
		HEADER_PRAGMA,
		HEADER_PROXY_AUTHENTICATE,
		HEADER_PROXY_AUTHORIZATION,
		HEADER_RANGE,
		HEADER_REFERER,
		HEADER_RETRY_AFTER,
		HEADER_SERVER,
		HEADER_TE,
		HEADER_TRAILER,
		HEADER_TRANSFER_ENCODING,
		HEADER_UPGRADE,
		HEADER_USER_AGENT,
		HEADER_VARY,
		HEADER_VIA,
		HEADER_WARNING,
		HEADER_WWW_AUTHENTICATE,
		HEADER_SET_COOKIE,
		HEADER_COOKIE
	};

	static const int HeaderCount;
	static const char * Header[];

	enum Message
	{
		MESSAGE_LINE	=(1),
		MESSAGE_HEADER	=(1)<<1,
		MESSAGE_BODY	=(1)<<2,
	};

	enum Options
	{
		VERSION_HTTP_1_0		=(1),
		VERSION_HTTP_1_1		=(1)<<1,
		VERSION					=VERSION_HTTP_1_0|VERSION_HTTP_1_1,
		PROXY					=(1)<<2,
		METHOD_GET				=(1)<<3,
		METHOD_POST				=(1)<<4,
		METHOD_HEAD				=(1)<<5,
		METHOD					=METHOD_GET|METHOD_POST|METHOD_HEAD,
		CONNECTION_CLOSE		=(1)<<6,
		CONNECTION_KEEP_ALIVE	=(1)<<7,
		CONNECTION				=CONNECTION_CLOSE|CONNECTION_KEEP_ALIVE,
	};

	enum ContentType
	{
		TEXT_HTML,
		IMAGE_JPG,
		IMAGE_GIF
	};

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpConnection : public Reason::Network::Connection
{
public:

	HttpConnection(const HttpConnection & connection):Connection(connection.Address,connection.Socket) {}
	HttpConnection(const Reason::Network::Address & address, const Reason::Network::Socket & socket):Connection(address,socket) {}
	HttpConnection(const Reason::Network::Address & address):Connection(address) {}
	HttpConnection() {}
	~HttpConnection() {}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpConnectionPool
{
public:

	Reason::Structure::Map<HttpConnection *, bool> Connections;
	int Limit;

	HttpConnectionPool(int limit=0):Limit(limit)
	{
	}

	~HttpConnectionPool()
	{
		Connections.DestroyKeys();
	}

	HttpConnection * Open(const Url & url) {return Open(Address(url));}

	HttpConnection * Open(const Address & address)
	{
		Iterator< Mapped<HttpConnection *,bool> > iterator = Connections.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			if (iterator().Value())
			{
				if (iterator().Key()->Socket.IsDisconnected())
				{

					iterator().Key()->Address = address;
				}
				else
				if (iterator().Key()->Socket.IsError() || iterator().Key()->Socket.IsTimeout())
				{

					iterator().Key()->Socket.Destroy();
					iterator().Key()->Address = address;
				}
				else
				if (iterator().Key()->Address != address)
				{

					continue;
				}

				iterator().Value() = false;
				return iterator().Key();
			}
		}

		if (Limit == 0 || Connections.Length() < Limit)
		{
			Iterand< Mapped<HttpConnection*,bool> > iterand = Connections.Insert(new HttpConnection(address),false);
			return iterand().Key();

		}

		return 0;
	}

	void Close(HttpConnection * connection)
	{
		Connections.Update(connection,true);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpAuthentication
{
public:

	enum AuthenticationScheme
	{
		AUTHENTICATION_SCHEME_NONE,
		AUTHENTICATION_SCHEME_BASIC,
		AUTHENTICATION_SCHEME_DIGEST,
	};

    int Scheme;	

	String Username;
	String Password;

	HttpAuthentication():Scheme(0) {}

	bool IsNone() {return Scheme == AUTHENTICATION_SCHEME_NONE;}
	bool IsBasic() {return Scheme == AUTHENTICATION_SCHEME_BASIC;}
	bool IsDigest() {return Scheme == AUTHENTICATION_SCHEME_DIGEST;}
};

class HttpCredentials
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpCookie : public Path, public Serialisable
{
public:

	HttpCookie();

	Reason::System::Path * Name;
	Reason::System::Path * Value;

	Reason::System::Path * Comment;
	Reason::System::Path * Domain;
	Reason::System::Path * MaxAge;
	Reason::System::Path * Path;
	Reason::System::Path * Secure;
	Reason::System::Path * Version;

	void Print(String & string);

	void Load(Stream & reader);
	void Store(Stream & writer);

	int Compare(Reason::System::Object * object, int comparitor=Comparable::COMPARE_GENERAL);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpContext
{
public:

	Reason::Structure::Set<HttpCookie*> Cookies;
	HttpAuthentication Authentication;

	HttpContext()
	{
	}

	~HttpContext()
	{
		Cookies.Destroy();
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpHeader: public Path
{
public:

	static const char Separator[];	

	HttpHeader():Path(true){}

	void Field(const int name, const Reason::System::String & value)
	{
		return Field(HttpConstants::Header[name], value);
	}

	void Field(const class Substring & name, const class Substring & value)
	{
		if (((Sequence&)name).IsSequenceOf(*Parent) && ((Sequence&)value).IsSequenceOf(*Parent))
		{

			Path * path = new Path();
			Append(path);
			path->Assign(name);
			path->Attach(new Path());
			path->Next->Assign(value);
			path->Next->Attach(new Path());		
		}
		else
		{

			String label(name);
			label.Proppercase();
			if (!label.Contains(":")) label.Append(": ");
			Path * path = new Path(label);
			Append(path);
			path->Attach(new Path(value),Placeable::PLACE_AFTER);
			path->Next->Attach(new Path(HttpHeader::Separator),Placeable::PLACE_AFTER);
		}
	}

	Path * Field(const char * data) {return Field((char*)data,String::Length(data));}
	Path * Field(const Sequence & sequence) {return Field(sequence.Data,sequence.Size);}
	Path * Field(char * data, int size) {return Field(0,data,size);}

	Path * Field(Path * from, const char * data) {return Field((char*)data,String::Length(data));}
	Path * Field(Path * from, const Sequence & sequence) {return Field(sequence.Data,sequence.Size);}
	Path * Field(Path * from, char * data, int size)
	{
		Path * path = Containing(data,size,true);
		if (path)
		{
			if (path->Next && !path->Next->Is(HttpHeader::Separator))
			{
				return path->Next;
			}			
		}

		return 0;
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpField : public Path
{
public:

	String Label;

	HttpField():Path(true) {}
	HttpField(HttpHeader * header):Path(true) {header->Append(this);}

	HttpField(HttpHeader * header, const String & name):Path(true)
	{
		header->Append(this);
		Name(name);
	}

	HttpField(HttpHeader * header, const String & name, const String & value):Path(true)
	{
		header->Append(this);
		Name(name);
		Value(value);
	}

	void Name(const String & name)
	{
		if (!Label.IsEmpty() && Prev && Prev->Is(Label))
		{

			assert(this->Next && this->Next->Is(HttpHeader::Separator));

			if (!((String&)name).Is(Label,true))
			{
				Label = name;
				Label.Proppercase();
				if (!Label.Contains(":")) Label.Append(": ");

				Prev->Replace(Label);		
			}
		}
		else
		{

			Label = name;
			Label.Proppercase();
			if (!Label.Contains(":")) Label.Append(": ");
		}				
	}

	void Value(const String & value)
	{
		if (!Parent) return;

		if (!Prev || !Prev->Is(Label))
		{
			this->Attach(new Path(Label), Placeable::PLACE_BEFORE);
		}

		this->Replace(value);

		if (!Next || !Next->Is(HttpHeader::Separator))
		{
			this->Attach(new Path(HttpHeader::Separator), Placeable::PLACE_AFTER);
		}
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpMessage : public Path
{
public:

	Path * Line;
	HttpHeader * Header;
	Path * Body;

	HttpMessage();

	Path * operator [] (const char * data) {return Header->Field(data);}
	Path * operator [] (const Sequence & sequence) {return Header->Field(sequence);}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpMessageHandler : public Handler
{
public:

	virtual void ProcessResponseLine(const Sequence & version, const Sequence & code, const Sequence & phrase)=0;
	virtual void ProcessRequestLine(const Sequence & method, const Sequence & uri, const Sequence & version)=0;
	virtual void ProcessField(const Sequence & name, const Sequence & value)=0;
	virtual void ProcessBody(const Sequence & body)=0;
};

class HttpMessageParser : public Parser
{
public:

	HttpMessageHandler * Handler;
	Bitmask8 Message;

	HttpMessageParser(Reason::System::Scanner & scanner):Parser(scanner),Handler(0),Message(0){}

	virtual bool Parse(int message = (HttpConstants::MESSAGE_LINE|HttpConstants::MESSAGE_HEADER|HttpConstants::MESSAGE_BODY));

	bool ParseRequest();
	bool ParseResponse();

	bool ParseMessage();
	bool ParseMessageHeader();
	bool ParseMessageBody();

	bool ParseChunk(String & body);
	bool ParseChunkSize(int & size);

	bool ParseField();
	bool ParseFieldName();
	bool ParseFieldValue();

	bool IsToken();
	bool IsSeparator();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpRequest : public HttpMessage
{
private:
	inline void Initialise();
public:

	Path * Method;
	Path * Uri;
	Path * Version;

	HttpField * Host;
	HttpField * From;
	HttpField * UserAgent;
	HttpField * Accept;	
	HttpField * AcceptEncoding;
	HttpField * ContentLength;
	HttpField * ContentType;
	HttpField * Connection;
	HttpField * Referer;

	int Timeout;

	HttpRequest();
	HttpRequest(const Url & url, int options = (HttpConstants::VERSION_HTTP_1_0|HttpConstants::METHOD_GET));
	~HttpRequest();

	bool Construct(const Reason::Network::Url &url, int options = (HttpConstants::VERSION_HTTP_1_0|HttpConstants::METHOD_GET));
	bool Construct(Socket & socket, int message = (HttpConstants::MESSAGE_LINE|HttpConstants::MESSAGE_HEADER|HttpConstants::MESSAGE_BODY));

	bool Deconstruct();
	bool Deconstruct(char * data, int size);

	void Redirect(const Reason::Network::Url & url);

	void Cookies(Reason::Structure::Set<HttpCookie*> & set);

};

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpRequestReader : public HttpMessageParser, public HttpMessageHandler
{
public:

	HttpRequest * Request;

	HttpRequestReader(Reason::System::Scanner & scanner, HttpRequest * request):HttpMessageParser(scanner),Request(request)
	{
	}

	virtual bool Parse(int message = (HttpConstants::MESSAGE_LINE|HttpConstants::MESSAGE_HEADER|HttpConstants::MESSAGE_BODY))
	{
		if (Request)
		{
			HttpMessageParser::Handler = this;
			return HttpMessageParser::Parse(message);
		}

		return false;
	}

	void ProcessResponseLine(const Sequence & version, const Sequence & code, const Sequence & phrase);
	void ProcessRequestLine(const Sequence & method, const Sequence & uri, const Sequence & version);
	void ProcessField(const Sequence & name, const Sequence & value);
	void ProcessBody(const Sequence & body);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpResponse : public HttpMessage
{
private:
	inline void Initialise();

public:

	enum ResponseState
	{
		RESPONSE_STATE_NONE,
		RESPONSE_STATE_RECEIVED,
		RESPONSE_STATE_PARSED,
		RESPONSE_STATE_CONSTRUCTED,
	};

	char State;

	Path * Version;		
	Path * StatusCode;	
	Path * ReasonPhrase;	

	HttpField * Date;
	HttpField * Pragma;

	HttpField * Location;
	HttpField * Server;
	HttpField * Authenticate;

	HttpField * Allow;

	HttpField * Connection;
	HttpField * TransferEncoding;
	HttpField * ContentEncoding;
	HttpField * ContentLength;
	HttpField * ContentType;
	HttpField * Expires;
	HttpField * LastModified;

	int Minimum;
	int Maximum;
	int Timeout;

	HttpResponse();
	HttpResponse(int code, int version = HttpConstants::VERSION_HTTP_1_0);
	~HttpResponse();

	bool IsParsed()			{return State == RESPONSE_STATE_PARSED;};
	bool IsConstructed()	{return State == RESPONSE_STATE_CONSTRUCTED;};

	void Construct();	
	void Construct(int code, int version = HttpConstants::VERSION_HTTP_1_0);

	void GuessContentType(char *data);		
	void GuessContentType(File &file);		

	void ConstructHeader(const String & name, const String & value)
	{
		HttpField * field = new HttpField(Header,name,value);
	}

	void ConstructBody(const String & body)
	{
		Body->Replace(body);
		Body->Append(new Path(HttpHeader::Separator));
	}

	void Deconstruct();
	void Deconstruct(char * data, int size);

	void Cookies(Reason::Structure::Set<HttpCookie*> & set);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpResponseReader : public HttpMessageParser, public HttpMessageHandler
{
public:

	HttpResponse * Response;

	HttpResponseReader(Reason::System::Scanner & scanner, HttpResponse * response):HttpMessageParser(scanner),Response(response)
	{
	}

	virtual bool Parse()
	{
		if (Response)
		{
			HttpMessageParser::Handler = this;
			return HttpMessageParser::Parse();
		}

		return false;
	}

	void ProcessResponseLine(const Sequence & version, const Sequence & code, const Sequence & phrase);
	void ProcessRequestLine(const Sequence & method, const Sequence & uri, const Sequence & version);
	void ProcessField(const Sequence & name, const Sequence & value);
	void ProcessBody(const Sequence & body);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpSession
{
public:

	Reason::Structure::Objects::List	Cookies;

	void Load();
	void Store();

	void Construct(HttpResponse &response);
	void To(HttpRequest &request);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpClient
{
public:

	HttpClient();
	~HttpClient();

	Reason::Network::Connection Connection;

	bool Process(const Url &url, HttpResponse &response);
	bool Process(const Url &url, HttpRequest &request, HttpResponse &response);
	bool Process(Address & address, HttpRequest &request, HttpResponse & response);

	bool ProcessHead(const Url &url, HttpResponse &response);
	bool ProcessHead(const Url &url, HttpRequest &request, HttpResponse &response);
	bool ProcessHead(Address & address, HttpRequest &request, HttpResponse & response);

	bool ProcessTrace(const Url &url, HttpResponse &response);
	bool ProcessTrace(const Url &url, HttpRequest &request, HttpResponse &response);
	bool ProcessTrace(Address & address, HttpRequest &request, HttpResponse & response);

	bool ProcessText(const Url &url, HttpResponse &response);
	bool ProcessText(const Url &url, HttpRequest &request, HttpResponse &response);
	bool ProcessText(Address & address, HttpRequest &request, HttpResponse & response);

	bool Exists(const Url &url);

public:

	static bool Process(HttpConnection & connection, HttpRequest &request, HttpResponse & response);
	static bool Process(HttpConnection & connection, HttpRequest &request, HttpResponse & response, HttpContext & context);

	static bool Process(Address & address, Socket & socket, HttpRequest &request, HttpResponse & response);
	static bool Process(Address & address, Socket & socket, HttpRequest &request, HttpResponse & response, HttpContext & context);

	static bool ProcessHeader(Socket & socket, HttpResponse & response);
	static bool ProcessBody(Socket & socket, HttpResponse & response);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpPipeline : public HttpClient
{
public:

	Reason::Structure::List< Pair<HttpRequest,HttpResponse> > Pipe;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpService : public HttpConstants
{
public:

	virtual int Process(HttpConnection & connection, HttpRequest & request, HttpResponse & response)
	{
		HttpContext context;
		return Process(connection,request,response,context);
	}

	virtual int Process(HttpConnection & connection, HttpRequest & request, HttpResponse & response, HttpContext & context) 
	{
		return Process(connection.Address,connection.Socket,request,response,context);
	}

	virtual int Process(Address & address, Socket & socket, HttpRequest & request, HttpResponse & response)
	{
		HttpContext context;
		return Process(address,socket,request,response,context);
	}

	virtual int Process(Address & address, Socket & socket, HttpRequest & request, HttpResponse & response, HttpContext & context)=0;
};

class HttpManager: public Threaded, public HttpConstants
{
protected:

	Reason::Platform::Critical Sentinel;

	void Run(void * thread);
	virtual void Accept();	
	virtual void Process(HttpConnection & connection);
	virtual void Error(HttpResponse & response, int status);

public:

	int ConnectionLimit;
	int ConnectionCount;
	Network::Socket ListenSocket;
	Network::Address ListenAddress;
	Structure::Enumerator<HttpService*> Services;

	HttpManager();
	~HttpManager();

	virtual bool Startup(const char *address, int port=80);
	virtual bool Shutdown();

	bool Active()
	{
		return Threads.Count > 0;
	}

};

class HttpPublisher : public HttpService
{
public:
		virtual int Process(HttpContext & context, HttpRequest & request, HttpResponse & response);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpServer: public Threaded
{
protected:

	Locked Sentinel;

	void Run(void * thread);
	virtual void Accept();	
	virtual void ProcessRequest(HttpConnection & connection);

public:

	int ConnectionLimit;
	int ConnectionCount;

	Socket ListenSocket;
	Address ListenAddress;

	HttpServer();
	~HttpServer();

	bool Startup(const Sequence & address, int port=80);
	bool Startup(const char *address, int port=80);
	bool Shutdown();

	virtual bool Active()
	{
		return Threads.Count > 0;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpAgent : public Threaded
{
public:

	enum AgentStates
	{
		AGENT_STATE_INITIAL		=(0),
		AGENT_STATE_SLEEPING	=(1)<<0,
		AGENT_STATE_PROCESSING	=(1)<<1,
	};

	int Sleeping;
	Locked	Sentinel;

	Reason::Structure::List< Reason::Structure::Set<Link> > Queue;

	HttpAgent();
	~HttpAgent();

	virtual bool Active();

	virtual bool Enqueue(const Link & link);
	virtual bool Dequeue(Link & link);
	virtual bool Has(const Link & link);
	virtual bool Has();

	int Count();

	virtual void Run(void * thread);
	virtual void Process(Link & link);	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class HttpForm
{
public:

	enum FormMethod
	{
		METHOD_NONE				=(0),
		METHOD_GET				=(1),
		METHOD_POST_MULTIPART	=(1)<<1,
		METHOD_POST_URLENCODE	=(1)<<2,
		METHOD_POST				=METHOD_POST_MULTIPART|METHOD_POST_URLENCODE,

	};

	Bitmask8 Method;

	Reason::System::Properties Fields;

	HttpForm():Method(0)
	{

	}

	bool IsPost() {return Method.Is(METHOD_POST);}
	bool IsGet() {return Method.Is(METHOD_GET);}

	void Construct(HttpRequest & request);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

