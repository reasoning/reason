
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

#include "reason/network/uri.h"
#include "reason/language/xml/xml.h"
#include "reason/network/http/http.h"

#include "assert.h"
#include "ctype.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "assert.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::Language;
using namespace Reason::Network::Http;

namespace Reason { namespace Network {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool UriParser::IsUnreserved()
{

	return (IsAlphanumeric() || IsMark());
}

bool UriParser::IsMark()
{
	return (Is('-') || 
			Is('_') || 
			Is('.') || 
			Is('!') || 
			Is('~') || 
			Is('*') || 
			Is('\'') || 
			Is('(') || 
			Is(')'));
}

bool UriParser::IsReserved()
{

	return (Is(';') || 
			Is('/') || 
			Is('?') || 
			Is(':') ||
			Is('@') || 
			Is('&') || 
			Is('=') || 
			Is('+') ||
			Is('$') || 
			Is(','));
}

bool UriParser::IsHexEscaped()
{

	return Is('%') && (*Scanner)(3) && Character::IsHex(*At(1,1)) && Character::IsHex(*At(2,1));
}

bool UriParser::Parse(Reason::Network::Uri &uri)
{

	Uri = &uri;
	Assign(*Uri);

	class Token state(Scanner->Token);

	if (!Eof())
	{

		ParseWhitespace();

		if (!Eof() && !ParseAbsoluteUri())
		{

			Load(state);

			if (Eof() || ! ParseRelativeUri())
				return false;
		}

		if (Is('#'))
		{
			if (!ParseFragment())
				return false;
		}

		ParseWhitespace();

		if (!Eof())
		{

			OutputError("UriParser::Parse - Column %d, \"%c\" [%s]\n",Column(),*(Uri->Data+Column()),Uri->Data);
			return false;
		}

		return true;
	}

	return false;
}

bool UriParser::ParseAbsoluteUri()
{
	if (! ParseScheme())
		return false;

	if (ParseHeirarchicalPart())
		return true;

	if (ParseOpaquePart())
	{
		Uri->Type = Uri::ABSOLUTE_URI_OPAQUE;
		return true;
	}

	return false;

}

bool UriParser::ParseRelativeUri()
{
	if (Is('/'))
	{
		Mark();	
		Next();

		if (Is('/'))
		{
			Next();

			if (!ParseNetworkPath())
				return false;

			Uri->Type = Uri::RELATIVE_URI_NETWORK_PATH;
		}
		else
		{
			if (!ParseAbsolutePath())
				return false;

			Trap();

			Uri->Path.Assign(Token);
			Uri->Type = Uri::RELATIVE_URI_ABSOLUTE_PATH;
		}

	}
	else
	{
		Mark();

		if (! ParseRelativePath())
			return false;

		Trap();

		Uri->Path.Assign(Token);
		Uri->Type = Uri::RELATIVE_URI_RELATIVE_PATH;
	}

	if (Is('?'))
	{
		if (!ParseQuery())
			return false;
	}

	return true;

}

bool UriParser::ParseHeirarchicalPart()
{
	if (Is('/'))	
	{
		Mark();	
		Next();

		if (Is('/'))	
		{

			Next();

			if (!ParseNetworkPath())
				return false;

			Uri->Type = Uri::ABSOLUTE_URI_NETWORK_PATH;
		}
		else
		{

			if (!ParseAbsolutePath())
				return false;

			Uri->Type = Uri::ABSOLUTE_URI_ABSOLUTE_PATH;

			Trap();
			Uri->Path.Assign(Token);
		}

		if (Is('?'))
			return ParseQuery();
		else
			return true;

	}

	return false;

}

bool UriParser::ParseOpaquePart()
{

	if ( IsUnreserved() || IsHexEscaped() || IsAny(";?:@&=+$,"))
	{
		Mark();
		Next();

		while (IsUnreserved() || IsHexEscaped() || IsReserved())
			Next();

		Trap();
		Uri->Path.Assign(Token);
		return true;

	}
	else
	{
		return false;
	}

}

bool UriParser::ParseScheme()
{

	if (IsAlpha())
	{
		Mark();
		Next();
		while (! Eof() && (IsAlphanumeric() || Is('+') || Is('-') || Is('.')))
			Next();

		if (Is(':'))
		{
			Trap();
			Next();

			Uri->Scheme.Assign(Token);
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool UriParser::ParseNetworkPath()
{
	if (!ParseAuthority())
		return false;

	if (Is('/'))
	{
		Mark();
		Next();

		ParseAbsolutePath();

		Trap();
		Uri->Path.Assign(Token);
	}

	return true;
}

bool UriParser::ParseAbsolutePath()
{

	while ( !Eof() && (Is('/') || IsUnreserved() || IsHexEscaped() || IsAny(":@&=+$,") || Is(';')) )
	{
		Next();	
	}

	return true;
}

bool UriParser::ParseRelativePath()
{

	if (IsUnreserved() || IsHexEscaped() || IsAny(";@&=+$,"))
	{
		do
		{
			Next();
		}
		while (IsUnreserved() || IsHexEscaped() || IsAny(";@&=+$,"));

		if (Is('/'))
		{
			return ParseAbsolutePath();
		}
		else
		{
			return true;
		}

	}

	return false;
}

bool UriParser::ParseAuthority()
{

	if (! ParseServerBasedAuthority())
	{
		return ParseRegistryBasedAuthority();

	}

	return true;
}

bool UriParser::ParseServerBasedAuthority()
{

	Uri->Authority = Uri::AUTHORITY_IPV4;

	class Token token;
	Store(token);

	Mark();

	while (IsUnreserved() || IsHexEscaped() || IsAny(";:&=+$,"))
	{
		Next();
	}

	if ( Is('@'))
	{
		Trap();
		Uri->Credentials.Assign(Token);
		Next();
		Mark(); 
	}
	else
	{

		Load(token);

	}

	int dots = 0;

	while (! (Eof() || Is('/') || Is('?') || Is(':') || Is('#')) )
	{

		if (IsAlphanumeric())
		{

			do
			{
				if (IsAlpha())
				{
					Next();
					Uri->Authority = Uri::AUTHORITY_HOST;	
				}
				else
				if (IsNumeric())
				{
					Next();
				}
				else if (Is('-') && !Is(-1,'.') )
				{

					Next();
					Uri->Authority = Uri::AUTHORITY_HOST;

					if (IsAlphanumeric())
						Next();
					else
						return false;
				}
				else
				{
					return false;
				}

			}
			while ( ! (Eof() || Is('/') || Is('?') || Is(':') || Is('.') || Is('#')) );

			if (Is('.'))
			{
				Next();		
				++dots;
			}

		}
		else
		{
			return false;
		}

		if ( Eof() || Is('/') || Is('?') || Is(':') || Is('#') )
			break;		

	}

	Trap();

	if (Uri->Authority == Uri::AUTHORITY_IPV4)
	{	

		if (dots > 3 || Is(-1,'.') || Token.IsEmpty())
			return false;

	}
	else
	if (dots > 0)
	{

		int index;
		if (Token.ReferenceAt(0) == '.')
		{

			index = Token.LastIndexOf(Token.Size-2,'.');
		}
		else
		{
			index = Token.LastIndexOf('.');
		}

		if (!Character::IsAlpha(Token.CharAt(index+1)))
			return false;
	}

	Uri->Server.Assign(Token);

	if (Is(':'))
	{
		Next();
		Mark();

		while ( !( Eof() || Is('/') || Is('?') ))
		{
			if (IsNumeric())
				Next();
			else
				return false;	

		}

		Trap();

		Uri->Port.Assign(Token);
	}

	return true;

}

bool UriParser::ParseRegistryBasedAuthority()
{

	Uri->Authority.Assign(0);

	if ( IsUnreserved() || IsHexEscaped() || IsAny("$,;:@&=+"))
	{

		do
		{
			Next();
		}
		while ( !Eof() && (IsUnreserved() || IsHexEscaped() || IsAny("$,;:@&=+")) );

		if (Eof() || Is('/') || Is('?'))
		{
			Trap();	
			Uri->Authority.Assign(Uri::AUTHORITY_REGISTRY);
			Uri->Registry.Assign(Token);
			return true;
		}
	}

	return false;
}

bool UriParser::ParseQuery()
{

	if (Is('?'))
	{
		Mark();
		Next();
		while ( !Eof() && (IsReserved() || IsUnreserved() || IsHexEscaped()))
		{
			Next();
		}
		Trap();
		Uri->Query.Assign(Token);
	}

	return true;
}

bool UriParser::ParseFragment()
{

	if (Is('#'))
	{
		Mark();

		Next();
		while ( !Eof() && (IsReserved() || IsUnreserved() || IsHexEscaped()))
		{
			Next();
		}

		Trap();
		Uri->Fragment.Assign(Token);

	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Uri::Construct(char *url, int size)
{
	String::Construct(url,size);
	String::Trim();
	Initialise();	

	UriParser parser;
	return parser.Parse(*this);
}

bool Uri::Construct()
{
	Initialise();	

	UriParser parser;
	return parser.Parse(*this);
}

void Uri::Initialise()
{
	Scheme.Release();
	Registry.Release();
	Credentials.Release();
	Server.Release();
	Port.Release();
	Path.Release();
	Query.Release();
	Fragment.Release();

	Type.Assign(0);
	Authority.Assign(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

