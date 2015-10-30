
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
#ifndef STRUCTURE_HASHABLE_H
#define STRUCTURE_HASHABLE_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/type.h"
#include "reason/system/object.h"

#include "reason/system/primitive.h"
#include "reason/system/sequence.h"

using namespace Reason::System;

namespace Reason { namespace Structure {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Hashable
{
public:

	typedef int (*HashFunction)(const _Kind_ & kind);
	static HashFunction Function;

	static int Hash(const _Kind_ & kind)
	{
		return Identify<_Kind_>(*(_Kind_*)(&kind),0);
	}

private:

	template<typename Argument>
	static int Identify(Argument & kind, void (Argument::*)())
	{
		return Process(kind,&kind);
	}

	template<typename Argument>
	static int Identify(Argument & kind, ...)
	{
		return Process(kind,kind);
	}

	static int Process(_Kind_ & kind, Reason::System::Object *)
	{
		return kind.Hash();
	}

	static int Process(_Kind_ & kind, void *)
	{
		return (int)&kind;
	}

	static int Process(_Kind_ & kind, ...)
	{
        return (int)kind;
	}

};

template <typename _Kind_>
typename Hashable<_Kind_>::HashFunction Hashable<_Kind_>::Function=0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Hashable<_Kind_*>
{
public:

	typedef int (*HashFunction)(const _Kind_ * kind);
	static HashFunction Function;

	static int Hash(const _Kind_ * kind)
	{
		return Identify<_Kind_>((_Kind_*)kind,0);
	}

private:

	template<typename Argument>
	static int Identify(Argument * kind, void (Argument::*)())
	{
		return Process(kind,kind);
	}

	template<typename Argument>
	static int Identify(Argument * kind, ...)
	{
		return Process(kind,kind);
	}

	static int Process(_Kind_ * kind, Reason::System::Object *)
	{
		return kind->Hash();
	}

	static int Process(_Kind_ * kind, void *)
	{

		return (int)(long)kind;
	}

	static int Correlate(_Kind_ * kind, char[])
	{
		return Sequences::Hash(kind);
	}

	static int Process(_Kind_ * kind, ...)
	{
        return (int)kind;
	}

};

template <typename _Kind_>
typename Hashable<_Kind_*>::HashFunction Hashable<_Kind_*>::Function=0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Hashable<_Kind_&>
{
public:

	typedef int (*HashFunction)(const _Kind_ & kind);
	static HashFunction Function;

	static int Hash(const _Kind_ & kind)
	{
		return Identify<_Kind_>(*(_Kind_*)(&kind),0);
	}

private:

	template<typename Argument>
	static int Identify(Argument & kind, void (Argument::*)())
	{
		return Process(kind,&kind);
	}

	template<typename Argument>
	static int Identify(Argument & kind, ...)
	{
		return Process(kind,kind);
	}

	static int Process(_Kind_ & kind, Reason::System::Object *)
	{
		return kind.Hash();
	}

	static int Process(_Kind_ & kind, void *)
	{
		return (int)&kind;
	}

	static int Process(_Kind_ & kind, ...)
	{
        return (int)kind;
	}

};

template <typename _Kind_>
typename Hashable<_Kind_&>::HashFunction Hashable<_Kind_&>::Function=0;

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

#endif

