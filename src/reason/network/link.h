
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
#ifndef NETWORK_LINK_H
#define NETWORK_LINK_H

#include "reason/network/url.h"
#include "reason/structure/list.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::Network;
using namespace Reason::System;
using namespace Reason::Structure;

namespace Reason { namespace Network {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Link: public Url
{
public:

	Url Referer;
	int Depth;

	Link(const Link &link):Url(link),Depth(link.Depth),Referer(link.Referer) {}
	Link(const Link &link, int depth):Url(link),Depth(depth),Referer(link.Referer) {}
	Link(const Url &url, int depth=0):Url(url),Depth(depth) {}
	Link(const Sequence & sequence, int depth=0):Url(sequence),Depth(depth) {}
	Link(const char *data):Url(data),Depth(0) {}
	Link(char *data, int size):Url(data,size),Depth(0) {}
	Link():Depth(0) {}

	bool Construct(const Link &link) {Depth=link.Depth;Referer=link.Referer;return Url::Construct(link);}
	bool Construct(const Link &link, int depth) {Depth=depth;Referer=link.Referer;return Url::Construct(link);}
	bool Construct(const Url &url, int depth=0) {Depth=depth;return Url::Construct(url);}
	bool Construct(const Sequence & sequence, int depth=0) {Depth=depth;return Url::Construct(sequence);}
	bool Construct(const char *url) {Depth=0;return Url::Construct(url);}
	bool Construct(char *url, int size)	{Depth=0;return Url::Construct(url);}
	bool Construct() {return Url::Construct();}

	Link & operator = (const Link & link) {Url::operator = (link);Depth=link.Depth;Referer=link.Referer;return *this;}

	int Compare(Object * object, int comparitor=COMPARE_GENERAL)
	{

		if (object->InstanceOf(Url::Instance))
		{

			Url * url = (Url*) object;
			int index = IndexAt(Fragment);
			int urlIndex = url->IndexAt(url->Fragment);
			switch(comparitor)
			{
			case Comparable::COMPARE_GENERAL:case Comparable::COMPARE_PRECISE:
				return Sequences::Compare(Data, index>=0?index:Size,url->Data, urlIndex>=0?urlIndex:url->Size,!comparitor);
			default:
				return (Url*)this - url;
			}

		}
		else
		{
			return Sequence::Compare(object,comparitor);
		}
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Navigate : public Url
{
public:

	Reason::Structure::List<Url *> History;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

