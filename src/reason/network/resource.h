
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
#ifndef NETWORK_RESOURCE_H
#define NETWORK_RESOURCE_H

#include "reason/network/url.h"
#include "reason/system/file.h"
#include "reason/system/folder.h"

using namespace Reason::Network;
using namespace Reason::System;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Network {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Resolver;

class Resource : public String 
{
public:

	static Identity Instance;

	virtual Identity& Identify()
	{
		return Instance;
	}

	virtual bool InstanceOf(Identity &identity)
	{
		return Instance == identity || String::InstanceOf(identity);
	}

public:

	Reason::Network::Resolver * Resolver;
	Reason::Network::Url Url;

	Resource(const Resource & resource):Resolver(resource.Resolver),Url(resource.Url),String(resource) {}
	Resource(const Reason::Network::Url & url):Resolver(0),Url(url) {}
	Resource(Reason::Network::Resolver * resolver):Resolver(resolver) {}
	Resource():Resolver(0) {}

	virtual ~Resource() {}

	int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL)
	{
		if (object->InstanceOf(Instance))
		{
			return Url.Compare(&((Resource*)object)->Url);
		}
		else
		if (object->InstanceOf(Url::Instance))
		{
			return Url.Compare(object);
		}
		else
		{
			return Identity::Error;
		}
	}

	using String::Assign;

	void Assign(const Resource & resource)
	{
		String::Assign(resource);
		Url.Construct(resource.Url);
	}

	using String::Acquire;

	void Acquire(const Resource & resource)
	{
		String::Acquire(resource);
		Url.Construct(resource.Url);
	}

	bool Resolve();

	void Destroy()
	{
		String::Destroy();
		Url.Destroy();
	}

	void Release()
	{
		String::Release();
		Url.Release();
	}

	virtual bool Download(const Reason::Network::Url & url);
	virtual bool Load(const File & file);

	virtual bool Create(const char * data) {return Create((char*)data,String::Length(data));}
	virtual bool Create(const Sequence & sequence) {return Create(sequence.Data,sequence.Size);}
	virtual bool Create(char * data, int size);

};

class Resolver
{
public:

	Resolver()
	{
	}

	virtual ~Resolver()
	{
	}

	virtual bool Resolve(Resource & resource)
	{

		return resource.Url.Download(resource);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

#endif