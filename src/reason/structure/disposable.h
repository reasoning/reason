
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

#ifndef STRUCTURE_DISPOSABLE_H
#define STRUCTURE_DISPOSABLE_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/interface.h"
#include "reason/generic/generic.h"

using namespace Reason::Generic;

namespace Reason { namespace Structure { 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Disposer 
{
public:

	static void Destroy(_Kind_ & type)
	{
		ProcessDestroy(type,&type);
	}

	static void Release(_Kind_ & type)
	{
		ProcessRelease(type,&type);
	}

private:

	static void ProcessDestroy(_Kind_ & type, Reason::System::Disposable *)
	{

	}

	static void ProcessDestroy(_Kind_ & type, ...)
	{

	}

	static void ProcessRelease(_Kind_ & type, Reason::System::Disposable *)
	{

	}

	static void ProcessRelease(_Kind_ &type, ...)
	{

	}
};

template <typename _Kind_>
class Disposer<_Kind_&>
{
public:

	static void Destroy(_Kind_ & type)
	{
		ProcessDestroy(type,&type);
	}

	static void Release(_Kind_ & type)
	{
		ProcessRelease(type,&type);
	}

private:

	static void ProcessDestroy(_Kind_ & type, Reason::System::Disposable *)
	{

	}

	static void ProcessDestroy(_Kind_ & type, ...)
	{

	}

	static void ProcessRelease(_Kind_ & type, Reason::System::Disposable *)
	{

	}

	static void ProcessRelease(_Kind_ &type, ...)
	{

	}
};

template <typename _Kind_>
class Disposer<_Kind_ *>
{
public:

	static void Destroy(_Kind_ *& type)
	{
		if (type != 0)
			ProcessDestroy(type,type);
	}

	static void Release(_Kind_ *& type)
	{
		if (type != 0)
			ProcessRelease(type,type);
	}

private:

	static void ProcessDestroy(_Kind_ *& type, Reason::System::Disposable *)
	{

		delete (_Kind_*)type;
		type = 0;
	}

	static void ProcessDestroy(_Kind_ *& type, void *[])
	{

		delete [] type;
		type = 0;
	}

	static void ProcessDestroy(_Kind_ *& type, void *)
	{

		delete (void*)type;
		type = 0;
	}

	static void ProcessRelease(_Kind_ *& type, Reason::System::Disposable *)
	{

		type = 0;
	}

	static void ProcessRelease(_Kind_ *& type, void *[])
	{
		type = 0;
	}

	static void ProcessRelease(_Kind_ *& type, void *)
	{
		type = 0;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

