
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

#include "reason/network/url.h"
#include "reason/network/uri.h"
#include "reason/network/http/http.h"
#include "reason/system/path.h"
#include "reason/system/parser.h"
#include "reason/system/properties.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::Network::Http;
using namespace Reason::Structure;
using namespace Reason::System;
using namespace Reason::System;

namespace Reason { namespace Network {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity Url::Instance;

void Url::Initialise()
{
	Type = URL_TYPE_NONE;

	Append((Scheme = new Reason::System::Path(true)));
	Append((Username = new Reason::System::Path(true)));
	Append((Password = new Reason::System::Path(true)));
	Append((Host = new Reason::System::Path(true)));
	Append((Port = new Reason::System::Path(true)));
	Append((Path = new Reason::System::Path(true)));
	Append((Query = new Reason::System::Path(true)));
	Append((Fragment = new Reason::System::Path(true)));
}

void Url::Release()
{
	Type = URL_TYPE_NONE;
	Path::Release();
}

void Url::Destroy()
{
	Type = URL_TYPE_NONE;
	Path::Destroy();
}

bool Url::Construct(const Url &url)
{
	Path::Release(false);
	String::Construct(url);
	Type.Assign(url.Type.Bits);

	if (!url.Scheme->IsEmpty()) Scheme->Assign(Data+(url.Scheme->Data-url.Data),url.Scheme->Size);
	if (!url.Username->IsEmpty()) Username->Assign(Data+(url.Username->Data-url.Data),url.Username->Size);
	if (!url.Password->IsEmpty()) Password->Assign(Data+(url.Password->Data-url.Data),url.Password->Size);
	if (!url.Host->IsEmpty()) Host->Assign(Data+(url.Host->Data-url.Data),url.Host->Size);
	if (!url.Port->IsEmpty()) Port->Assign(Data+(url.Port->Data-url.Data),url.Port->Size);
	if (!url.Path->IsEmpty()) Path->Assign(Data+(url.Path->Data-url.Data),url.Path->Size);
	if (!url.Query->IsEmpty()) Query->Assign(Data+(url.Query->Data-url.Data),url.Query->Size);
	if (!url.Fragment->IsEmpty()) Fragment->Assign(Data+(url.Fragment->Data-url.Data),url.Fragment->Size);
	return true;
}

void Url::WhitespaceEncode()
{
	char *p = Data;
	while (p != Data+Size)
	{
		if (*p == ' ')
			*p = '+';

		++p;
	}
}

void Url::WhitespaceDecode()
{
	char *p = Data;
	while (p != Data+Size)
	{
		if (*p == '+')
			*p = ' ';

		++p;
	}
}

bool Url::Download(String & string, bool textOnly)
{
	OutputMessage("Url::Download - Downloading \"%s\"\n",Print());

	if (Type.Equals(URL_TYPE_UNKNOWN)) 
	{
		return false;
	}

	if (Type.Equals(URL_TYPE_HTTP))
	{
		HttpResponse response;

		HttpClient http;

		if (textOnly)
		{
			if (!http.ProcessText(*this,response))
			{
				OutputError("Url::Download - Could not download document, or content type was not text.\n");
				return false;
			}

			string.Construct(*response.Body);
		}
		else
		{
			if ( ! http.Process(*this,response))
			{
				OutputError("Url::Download - Could not download document.\n");
				return false;
			}

			string.Construct(*response.Body);
		}
	}
	else
	if (Type.Equals(URL_TYPE_FILE))
	{
		File file;
		file.Construct(*Path);

		if (file.Exists())
		{
			string.Allocate(file.Status().Size);
			file.Open(File::OPTIONS_OPEN_TEXT_READ);
			file.Read(string);
			file.Close();
		}
		else
		{
			OutputError("Url::Download - File not found [%s]\n",Print());
			return false;
		}
	}

	return true;

}

bool Url::Construct(char * data, int size)
{

	String::Construct(data,size);
	String::Trim();
	return Construct();
}

bool Url::Construct()
{

	Path::Release(false);

	WhitespaceEncode();

	Uri uri;
	uri.Assign(*this);
	if (! uri.Construct())
	{
		OutputError("Url::Construct - Uri could not be constructed, %s\n",Print());
		return false;
	}
	else
	if (uri.IsAbsolute() && uri.Server.IsEmpty() && uri.Registry.IsEmpty())
	{

		OutputError("Url::Construct - Uri was missing an authority, %s\n",Print());
		return false;
	}

	if (uri.Type.Is(Uri::ABSOLUTE_URI_OPAQUE))
	{
		OutputMessage("Url::Construct - Opaque uri not supported, %s.\n", Print());
		return false;
	}
	else
	if (uri.Type.Is(Uri::ABSOLUTE_URI_ABSOLUTE_PATH))
	{
		OutputMessage("Url::Construct - Absolute uri with absolute path not supported, %s.\n",Print());
		return false;
	}

	if (uri.Authority.Is(Uri::AUTHORITY_SERVER))
	{
		Host->Assign(uri.Server);
	}
	else
	if (uri.Authority.Is(Uri::AUTHORITY_REGISTRY))
	{

		if (uri.Scheme.IsEmpty() || uri.Scheme.Is("file",true) || (uri.Scheme.IsAlpha() &&  uri.Scheme.Size == 1))
		{

			Path->Assign(uri.Registry.Data,uri.Registry.Size + uri.Path.Size);
		}
		else
		{

			Host->Assign(uri.Registry);
		}
	}

	Port->Assign(uri.Port);

	if (! uri.Credentials.IsEmpty() )
	{
		int index = uri.Credentials.IndexOf(':');
		if (index != -1)
		{
			Username->Assign(uri.Credentials.Data,index);
			Password->Assign(uri.Credentials.PointerAt(index+1),uri.Credentials.Size-(index+1));
		}
		else
		{
			Username->Assign(uri.Credentials);
		}
	}

	Query->Assign(uri.Query);
	Fragment->Assign(uri.Fragment);

	if (Path->IsEmpty())
		Path->Assign(uri.Path);

	if ( ! uri.Scheme.IsEmpty())
	{
		Scheme->Assign(uri.Scheme);

		if (Scheme->Is("http",true))
		{

			Type = URL_TYPE_HTTP;
		}
		else
		if (Scheme->Is("https",true))
		{

			Type = URL_TYPE_HTTPS;			
		}
		else
		if (Scheme->Is("ftp",true))
		{

			Type = URL_TYPE_FTP;
		}
		else
		if (Scheme->Is("file",true))
		{
			Type = URL_TYPE_FILE;
		}
		else
		if (Scheme->IsAlpha() &&  Scheme->Size == 1)
		{

			Type = URL_TYPE_FILE;
			Path->Size += Path->Data - Scheme->Data;
			Path->Data = Scheme->Data;
		}
		else
		{
            Type = URL_TYPE_UNKNOWN;
			OutputWarning("Url::Construct - Unknown scheme, %s\n",Scheme->Print());
		}
	}
	else
	{

		Scheme->Assign(uri.Scheme);
		Type = URL_TYPE_FILE;
	}

	if (Path->IsEmpty())
	{
		Path->Append("/");
	}

	WhitespaceDecode();

	return true;		
}

void Url::Filename(Reason::System::Substring &dest)
{

	int idx = Path->LastIndexOf("/");
	if (idx == -1)
		idx = Path->Size-1;

	if (idx < Path->Size-1)
		dest.Assign(Path->PointerAt(idx+1), Path->PointerAt(Path->Size-1));
}

Substring Url::Filename()
{
	Reason::System::Substring substring;
	Filename(substring);
	return substring;
}

void Url::Parameterise()
{

	Query->Release(false);

	StringParser parser;
	parser.Assign(*Query);

	if (!parser.Eof())
	{	
		if (parser.Is('?')) parser.Next();

		parser.Mark();

		Reason::System::Substring name;
		Reason::System::Substring value;

		if (!parser.Eof())
		{			
			while(true)
			{
				while (!parser.Eof() && !parser.Is('='))
					parser.Next();

				if (parser.Is('='))
				{
					parser.Trap();
					name.Assign(parser.Token);
					parser.Next();
					parser.Mark();	

					while (!parser.Eof() && !parser.Is('&'))
						parser.Next();

					parser.Trap();
					value.Assign(parser.Token);

					if (!name.IsEmpty())
					{
						Reason::System::Path * parameter = new Reason::System::Path();
						Query->Append(parameter);

						parameter->Append(new Reason::System::Path());
						(*parameter)[0].Assign(name);

						if (!value.IsEmpty())
						{
							parameter->Append(new Reason::System::Path());
							(*parameter)[1].Assign(value);
						}
					}

					name.Release();
					value.Release();

					if (!parser.Eof())
					{
						parser.Next();
						parser.Mark();					
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}

	}
}

void Url::Parameters(Reason::System::Properties & properties)
{
	Parameterise();

	Path::Enumerator iterator(*Query);
	for (iterator.Forward();iterator.Has();iterator.Move())
	{
		String name = (*iterator())[0];
		String value = (iterator()->Count == 2)?(*iterator())[1]:"";

		Decode(value);

		properties[name] = value;
	}
}

bool Url::IsAlias(const Url &url) 
{
	if (Host->IsEmpty() || url.Host->IsEmpty()) return 0;

	return  (Host->IndexOf(*url.Host,true) != -1) || (url.Host->IndexOf(*Host,true) != -1); 
}

bool Url::IsValid()
{

	return !Type.Is(URL_TYPE_NONE) && !IsEmpty();
}

bool Url::IsUnknown()
{
	return Type.Is(URL_TYPE_UNKNOWN);
}

bool Url::IsAbsolute()
{
	return ! Scheme->IsEmpty();
}

bool Url::IsRelative()
{
	return Scheme->IsEmpty();
}

bool Url::IsFile()
{
	return Type.Equals(URL_TYPE_FILE);
}

bool Url::IsFtp()
{
	return Type.Equals(URL_TYPE_FTP);
}

bool Url::IsHttp()
{
	return Type.Equals(URL_TYPE_HTTP);
}

bool Url::IsMailto()
{
	return Type.Equals(URL_TYPE_MAILTO);
}

void Url::Normalise()
{

	if (Port->Is("80"))
	{

		Port->Data--;
		Port->Size++;
		Port->Clear();
	}

	String path(*Path);
	Normalise(path);

	Path->Replace(path);
}

void Url::Normalise(String & path)
{

	if (path.IsEmpty()) 
		return;

	Reason::System::Path segments;
	segments.Construct(path);	

	segments.Split("/",true);

	Reason::System::Path *seg = segments.First;
	while (seg != 0)
	{

		if ( seg->Is("/..") && *seg->PointerAt(seg->Size) == '/'  )
		{

			Reason::System::Path *rem = seg;
			seg = seg->Next;

			if (rem->Prev != 0)
				rem->Prev->Delete();

			rem->Delete();
		}
		else
		if ( seg->Is("/.") && *seg->PointerAt(seg->Size) == '/'  )
		{

			Reason::System::Path *rem = seg;
			seg = seg->Next;
			rem->Delete();
		}
		else
		{
			seg = seg->Next;
		}
	}

	((Url&)path).Replace(segments);
}

void Url::Relative(const Url &parent)
{

	if (&parent == this)
		return;

	if (!Scheme->Is(*parent.Scheme)) 
		return;

	if (!Host->Is(*parent.Host))
		return;

	Reason::System::Path parentPath;
	parentPath.Append(parent.Path->Data,parent.Path->LastIndexOf("/")+1);
	Normalise(parentPath);
	if (((Url&)parent).IsAbsolute() && parentPath.Data[0] != '/')
		parentPath.Prepend('/');

	Reason::System::Path path;
	path.Append(Path->Data,Path->LastIndexOf("/")+1);
	Normalise(path);
	if (IsAbsolute() && path.Data[0] != '/')
		path.Prepend('/');

	if (! (IsValid() && ((Url&)parent).IsValid()) )
		return;

	Scheme->Clear();

	Substring sub(Data,Size);
	if (sub.StartsWith("://"))
		sub.Size = 3;
	else
	if (sub.StartsWith(":"))
		sub.Size = 1;

	Scheme->Assign(sub);
	Scheme->Clear();

	Host->Clear();
	Username->Clear();
	Password->Clear();

	Port->Clear();
	if(Port->Prev && Port->Prev->Is(":"))
		Port->Prev->Clear();

	Reason::System::Path segments;

	path.Split("/",false);
	parentPath.Split("/",false);

	Reason::System::Path * parentSegment = parentPath.First;
	Reason::System::Path * pathSegment = path.First;
	while (parentSegment != 0 || pathSegment != 0)
	{

		if (parentSegment && (!pathSegment || !parentSegment->Is(*pathSegment)))
		{
			while (parentSegment != 0)
			{		
				segments.Append("../");
				parentSegment = parentSegment->Next;
			}
		}

		if (pathSegment)
		{
			segments.Append(*pathSegment);
			segments.Append("/");
		}

		if (parentSegment)
			parentSegment = parentSegment->Next;

		if (pathSegment)
			pathSegment = pathSegment->Next;				
	}

	int index = Path->LastIndexOf("/")+1;
	segments.Append(Path->Data+index,Path->Size-index);
	Path->Replace(segments);

}

void Url::Absolute(const Url &parent)
{

	if (&parent == this)
		return;

	if (!Scheme->IsEmpty()) 
		return;

	if (! (IsValid() && ((Url&)parent).IsValid()) )
		return;

	if ( Scheme->IsEmpty() )
	{
		if ( ! parent.Scheme->IsEmpty() )
		{	
			Scheme->Replace(*parent.Scheme);

			if (!After((int)(Scheme->Data+Scheme->Size-Data-1)).StartsWith(":"))
				Scheme->AttachAfter(":");

			Type = parent.Type;
		}
		else
		{
			OutputWarning("Url::Absolute - The parent does not have a scheme. (parent)[%s]\n",((Url&)parent).Print());
		}	
	}
	else
	if (! Scheme->Is(*parent.Scheme))
	{
		OutputError("Url::Absolute - The parent URL is a different scheme. (parent)[%s]\n",((Url&)parent).Print());
		return;
	}

	if ( Username->IsEmpty() )
	{
		if ( ! parent.Username->IsEmpty()  )
		{
			Username->Replace(*parent.Username);
		}			
	}
	else
	if (parent.Username->IsEmpty() )
	{
		OutputError("Url::Absolute - Unlikely scenario, current URL has username, parent does not. (current)[%s]\n",Print());
		return;
	}

	if ( Password->IsEmpty() )
	{
		if (! parent.Password->IsEmpty())
		{
			Password->Replace(*parent.Password);
		}
	}
	else
	if (parent.Password->IsEmpty())
	{
		OutputError("Url::Absolute - Unlikely scenario, current URL has password, parent does not. (current)[%s]\n",((Url&)parent).Print());
		return;
	}

	if ( Host->IsEmpty() )
	{

		if (!parent.Host->IsEmpty())
		{

			Substring sub = After((int)(Scheme->Data+Scheme->Size-Data-1));
			if (sub.StartsWith(":") && !sub.StartsWith("://"))
			{

				sub.Size=1;				
				Host->Assign(sub);
				Host->AttachAfter("//");
				Host->Data += 3;
				Host->Size = 0;
			}	

			Host->Replace(*parent.Host);

			if (!parent.Port->IsEmpty())
			{
				Port->AttachBefore(":");
				Port->Replace(*parent.Port);
			}
		}
	}
	else
	{

		if ( this->IsAlias(parent) )
		{
			Host->Replace(*parent.Host);	
		}
		else

		{
			OutputWarning("Url::Absolute - Parent and current url's have different hosts, no changes will be made.\n");
			OutputMessage("Url::Absolute - (parent)[%s] (current)[%s]\n",((Url&)parent).Print(),Print());
			return;
		}
	}

	if ( Path->IsEmpty() )
	{
		if (! parent.Path->IsEmpty())
		{
			if (parent.Path->Data[0] != '/')
			{	

				Path->Append("/");
			}

			if (parent.Path->ReferenceAt(0) != '/')
			{

				Path->Append(parent.Path->Data,parent.Path->LastIndexOf("/")+1);
			}
			else
			{
				Path->Replace(*parent.Path);
			}
		}
	}
	else
	{
			if (Path->Data[0] != '/')
			{

				Reason::System::Path segments;
				segments.Append(parent.Path->Data,parent.Path->LastIndexOf("/")+1);

				segments << *Path;
				segments.Split("/",true);

				Reason::System::Path * path = segments.First;
				while (path != 0)
				{

					if ( path->Is("/..") && *path->PointerAt(path->Size) == '/'  )
					{

						Reason::System::Path * removed = path;
						path = path->Next;

						if (removed->Prev != 0)
							removed->Prev->Clear();

						removed->Clear();
					}
					else
					if ( path->Is("/.") && *path->PointerAt(path->Size) == '/'  )
					{

						Reason::System::Path * removed = path;
						path = path->Next;
						removed->Clear();
					}
					else
					{
						path = path->Next;
					}
				}

				if (((Url&)parent).IsAbsolute() && segments.Data[0] != '/')
					segments.Prepend('/');

				Path->Replace(segments);
			}
	}

}

 int Url::Hash()
{
	int key = Port->Integer();

	key += Scheme->Hash();
	key += Host->Hash();
	key += Password->Hash();
	key += Username->Hash();
	key += Path->Hash();

	return key;
}

 int Url::Compare(Reason::System::Object *object, int comparitor)
{

	return Sequence::Compare(object,comparitor);
}

 bool Url::Equals(Reason::System::Object *object, int comparitor)
{

	if (this == object) return true;

	if (object->InstanceOf(this) || object->InstanceOf(Sequence::Instance))
	{
		Sequence * sequence = ((Sequence *)object);

		switch(comparitor)
		{
		case Comparable::COMPARE_GENERAL:case Comparable::COMPARE_PRECISE:
			return Sequences::EqualsReverse(Data,Size,sequence->Data,sequence->Size,!comparitor);
		default:
			return false;
		}
	}
	else
	{	
		return Sequence::Equals(object,comparitor);
	}	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const int UrlLibrary::ProtocolCount = 8;
const UrlLibrary::UrlProtocol UrlLibrary::Protocol[] = 
	{
	{"http","http",80},
	{"https","https",443},
	{"ftp","ftp",21},
	{"gopher","gopher",70},
	{"mailto","smtp",25},
	{"news","nntp",119},
	{"telnet","telnet",23},
	{"wais","wais",0}
	};

int UrlLibrary::ResolvePort(const Url & url)
{
	switch(url.Type.Bits)
	{
	case Url::URL_TYPE_FILE:
	case Url::URL_TYPE_NONE:
	case Url::URL_TYPE_MAILTO:
	case Url::URL_TYPE_UNKNOWN:
		return 0;
	case Url::URL_TYPE_FTP:		
		return 21;
	case Url::URL_TYPE_HTTP:	
		return 80;
	case Url::URL_TYPE_HTTPS:	
		return 443;
	case Url::URL_TYPE_GOPHER:
		return 70;
	case Url::URL_TYPE_NEWS:
		return 119;
	case Url::URL_TYPE_TELNET:
		return 23;
	default:
		{
			if (!url.Scheme->IsEmpty())
			{
				for (int i=0;i<ProtocolCount;++i)
					if (url.Scheme->Is(Protocol[i].Scheme))
						return Protocol[i].Port;
			}
		}
	}

	return 0;
}

const char * UrlLibrary::ResolveProtocol(const Url & url)
{
	if (!url.Scheme->IsEmpty())
	{
		for (int i=0;i<ProtocolCount;++i)
			if (url.Scheme->Is(Protocol[i].Scheme))
				return Protocol[i].Protocol;
	}

	if (!url.Port->IsEmpty())
	{
		for (int i=0;i<ProtocolCount;++i)
			if (url.Port->Integer() == Protocol[i].Port)
				return Protocol[i].Protocol;
	}

	return 0;
}

const char * UrlLibrary::ResolveScheme(const Url & url)
{
	if (!url.Port->IsEmpty())
	{
		for (int i=0;i<ProtocolCount;++i)
			if (url.Port->Integer() == Protocol[i].Port)
				return Protocol[i].Scheme;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

