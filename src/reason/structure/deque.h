
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
#ifndef STRUCTURE_DEQUE_H
#define STRUCTURE_DEQUE_H

#include "reason/structure/array.h"

using namespace Reason::System;

namespace Reason { namespace Structure {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Abstract {

template<typename _Kind_>
class Deque : public Placeable
{
public:

	class Block
	{
	public:

		void * operator new(size_t, int size)
		{
			return new char[sizeof(_Kind_)*size];
		}
	};

	class Fragment : public Variable<_Kind_>
	{
	public:

		int Index;		 
	};

	class Item
	{
	public:

		int Fragment;	
		class Block * Block;		
	};

	Array<Fragment> Fragments;

	Array<Item> Items;

	int Count;

	virtual Iterand<_Kind_> Insert( typename Template<_Kind_>::ConstantReference type, int index)
	{

		Item & item = Items[index/4];
		if (item.Fragment)
		{
			for (int i=0;i<item.Fragment;++i)
			{
				if (Fragments.Index == index)
				{

					Fragments.Insert(Fragment(type,index));

					for (int j=i;j<Fragments.Size;++j)
						Fragments[j].Index++;

				}
				else
				{

					++index;
				}
			}
		}
	}

	void Push()
	{

	}

	void Pop()
	{

	}	

	void Inject()
	{

	}

	void Eject()
	{

	}
};

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
class Deque : public Array<_Kind_>, public Placeable
{
public:

	int Offset;
	int Placement;

	Deque():Offset(0),Placement(0)
	{

	}

	Deque(const Deque & deque):
		Offset(0),Placement(PLACE_AFTER)
	{
		*this = deque;
	}

	Deque(const Iterable<_Kind_> & iterable)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());
	}

	int Preceding()
	{

		return -1;
	}

	int Following()
	{

		return -1;
	}

	int Balance()
	{

		return Following()-Preceding();
	}

	virtual Iterand<_Kind_> Insert( typename Template<_Kind_>::ConstantReference type, int index)
	{

		OutputAssert(index >= 0 && index <= this->Size);

		if (this->Size == this->Allocated)
		{
			this->Allocate(0);
		}

		if (Balance() > 0)
		{

			Move(this->Data+index+1,this->Data+index,(this->Size-index));
			this->Data[index]=(typename Template<_Kind_>::Reference)type;
			++this->Size;

		}
		else
		{

			this->Data = (_Kind_ *) Move(this->Data-1,this->Data,index);
			this->Data[index]=(typename Template<_Kind_>::Reference)type;
			++this->Size;
		}

		return Iterand<_Kind_>();
	}

	virtual Iterand<_Kind_>  Insert(typename Template<_Kind_>::ConstantReference type)
	{

		if (this->Size == this->Allocated)
		{
			this->Allocate(0);
		}

		if (Balance() > 0)
		{

			*(this->Data+this->Size)=(typename Template<_Kind_>::Reference)type;
		}
		else
		{

			*(--this->Data)=(typename Template<_Kind_>::Reference)type;
		}

		++this->Size;
		return Iterand<_Kind_>();
	}

	virtual Iterand<_Kind_> Append(typename Template<_Kind_>::ConstantReference type)
	{

		if (Following())
		{
			this->Data[this->Size++]=(typename Template<_Kind_>::Reference)type;
		}
		else
		{
			Insert(type,this->Size);
		}

		return Iterand<_Kind_>();
	}

	virtual Iterand<_Kind_> Prepend(typename Template<_Kind_>::ConstantReference type)
	{
		if (Preceding())
		{
			*(--this->Data)=(typename Template<_Kind_>::Reference)type;
			++this->Size;
		}
		else
		{
			Insert(type,0);
		}

		return Iterand<_Kind_>();
	}

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

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

