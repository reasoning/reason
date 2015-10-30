
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
#ifndef REASON_STRUCTURE_INTRINSIC_H
#define REASON_STRUCTURE_INTRINSIC_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Structure { namespace Intrusive {

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/indices.h"
#include "reason/generic/generic.h"
#include "reason/generic/traits.h"
#include "reason/messaging/callback.h"

using namespace Reason::System;
using namespace Reason::Generic;
using namespace Reason::Messaging;

namespace Reason { namespace Structure { namespace Intrinsic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Range
{
	int First;
	int Last;

	Range(int first=0,int last=0):First(first),Last(last){}
};

template <typename _Return_, typename _Arg1_>
class Functor
{
public:

	Callback<_Return_,_Arg1_> Call;

	Functor() 
	{
	}

	Functor(Callback<_Return_,_Arg1_> & callback):Call(callback)
	{
	}

	template<typename _Functor_>
	Functor(_Functor_ & functor):Call(&functor,&_Functor_::operator ())
	{
	}

	_Return_ operator() (_Arg1_ arg) const
	{
		return Call(arg);
	}	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_, int _Factor_=175>
class Array
{
public:

	static const float Factor;

	struct PointerOrPrimitive
	{
		enum {Value = Logical::Or<Traits::Pointer<_Kind_>::Value,Traits::Primitive<_Kind_>::Value>::Value};		
	};

	static void Allocate(_Kind_ *& data, int allocated)
	{

		if (PointerOrPrimitive::Value)
		{
			data = (_Kind_*)malloc(allocated*sizeof(_Kind_));	
		}
		else
		{
			data = new _Kind_[allocated];									
		}
	}

	static void Reallocate(_Kind_ *& data, int size, int allocated)
	{

		if (PointerOrPrimitive::Value)
		{

			_Kind_ * mem = (_Kind_*)realloc(data,allocated*sizeof(_Kind_));

			data = mem;
		}
		else
		{
			_Kind_ * mem = new _Kind_[allocated];
			Copy(mem,data,size);
			delete [] data;

			data = mem;			
		}	
	}

	static void Delete(_Kind_ * data)
	{

		if (PointerOrPrimitive::Value)
		{
			free(data);
		}
		else
		{
			delete [] data;		
		}	
	}

	static _Kind_ * Copy(_Kind_ * to, _Kind_ * from, int size)
	{

		if (PointerOrPrimitive::Value)
		{
			memcpy(to,from,sizeof(_Kind_)*size);
		}
		else
		{
			if (to+size >= from && to <= from+size) return 0;

			int loop = (size+7)/8;
			if (loop > 0)
			{
				switch (size%8)
				{
					case 0: do {	*to++ = *from++;
					case 7:			*to++ = *from++;
					case 6:			*to++ = *from++;
					case 5:			*to++ = *from++;
					case 4:			*to++ = *from++;
					case 3:			*to++ = *from++;
					case 2:			*to++ = *from++;
					case 1:			*to++ = *from++;
							} while (--loop > 0);
				}
			}
		}

		return to;
	}

	static _Kind_ * Move(_Kind_ * to, _Kind_ *from, int size)
	{	

		if (PointerOrPrimitive::Value)
		{
			memmove(to,from,sizeof(_Kind_)*size);
		}
		else
		{ 
			if (to > from+size || to < from )
			{
				int loop = (size+7)/8;
				if (loop > 0)
				{
					switch (size%8)
					{
						case 0: do {	*to++ = *from++;
						case 7:			*to++ = *from++;
						case 6:			*to++ = *from++;
						case 5:			*to++ = *from++;
						case 4:			*to++ = *from++;
						case 3:			*to++ = *from++;
						case 2:			*to++ = *from++;
						case 1:			*to++ = *from++;
								} while (--loop > 0);
					}
				}

			}
			else
			if (to > from)
			{
				to += size-1;
				from += size-1;				

				int loop = (size+7)/8;
				if (loop > 0)
				{
					switch (size%8)
					{
						case 0: do {	*to-- = *from--;
						case 7:			*to-- = *from--;
						case 6:			*to-- = *from--;
						case 5:			*to-- = *from--;
						case 4:			*to-- = *from--;
						case 3:			*to-- = *from--;
						case 2:			*to-- = *from--;
						case 1:			*to-- = *from--;
								} while (--loop > 0);
					}
				}
			}
		}

		return to;
	}

	template<typename _Compare_>
	static inline int Index(_Kind_ * data, int size, _Compare_ comp)
	{

		int first	= 0;
		int last	= size-1;
		int middle	= first + (int)((last-first+1)>>1);
		int compare	= 0;

		while (first <= last)
		{
			compare = comp(data+middle);
			if (compare == 0) break;

			if (compare < 0)
				last = middle-1;
			else
				first = middle+1;

			middle = first + (int)((last-first+1)>>1);				
		}	

		return middle;		
	}

public:

	_Kind_ * Data;
	int Size;
	int Allocated;

	Array(int allocated)
	{
		Data = new _Kind_[allocated];
		Size = 0;
		Allocated = allocated;
	}

	Array():Data(0),Size(0),Allocated(0)
	{
	}

	Array(const Array & array):Data(0),Size(0),Allocated(0)
	{

		if (array.Size > 0)
		{
			Data = new _Kind_[array.Size];								
			Allocated = array.Size;					
			Array<_Kind_>::Copy(Data,array.Data,array.Size);	
			Size = array.Size;
		}				
	}

	~Array()
	{
		if (Data != 0)
		{
			Array<_Kind_>::Delete(Data);
		}

		Data = 0;
	}

	void Release()
	{
		if (Data)
		{

			Size=0;
		}
	}

	void Destroy()
	{
		if (Data)
		{
			for (int i=0;i<Length();++i)
				Disposer<_Kind_>::Destroy(Data[i]);

			Array<_Kind_>::Delete(Data);
			Data=0;
			Size=0;
			Allocated=0;
		}
	}	

	inline _Kind_ & At(int index)
	{

		if (Indices::Normalise(index,Size))
			return Data[index];

		return Null<_Kind_>::Value();
	}

	int Length()
	{
		return Size;
	}

	_Kind_ & operator [] (int index)
	{
		return At(index);
	}

	inline void Allocate(int amount=0)
	{

		if (Allocated > 0)
		{
			if (amount == 0)
				amount = (Allocated*Factor)+1;
			else
			if (amount < 0 || amount < Size)
				amount = Size;

			Array<_Kind_>::Reallocate(Data,Size,amount);
		}
		else
		{
			if (amount == 0)
				amount = (((Size>0)?Size:1)*Factor)+1;			
			else
			if (amount < 0 || amount < Size)
				amount = Size;

			_Kind_ * data;
			Array<_Kind_>::Allocate(data,amount);

			if (Size > 0)
				Array<_Kind_>::Copy(data,Data,Size);		

			Data = data;
		}

		Allocated = amount;									

	}

	Array & operator = (const Array & array)
	{
		if (this != &array)
		{
			if (array.Size > 0)
			{
				if (array.Size > Allocated)
				{
					if (Allocated)
						Array<_Kind_>::Delete(Data);

					Allocated = array.Size;					
					Array<_Kind_>::Allocate(Data,Allocated);					
				}	

				Array<_Kind_>::Copy(Data,array.Data,array.Size);
			}

			Size = array.Size;
		}

		return *this;
	}		

	inline bool Insert(typename Template<_Kind_>::ConstantReference kind)
	{
		if (Size >= Allocated)
			Allocate();				

		Data[Size++] = Reference<_Kind_>::Template(kind);
		return true;
	}

	inline bool Insert(typename Template<_Kind_>::ConstantReference kind, int index)
	{

		if (index != Length() && !Indices::Normalise(index,Size))
			return false;

		if (Size >= Allocated)		
			Allocate();

		Array<_Kind_>::Move(Data+index+1,Data+index,Size-index);		

		Data[index] = Reference<_Kind_>::Template(kind);
		++Size;

		return true;		
	}

	inline bool Append(typename Template<_Kind_>::ConstantReference kind)
	{
		Insert(kind,Size);
	}

	inline bool Prepend(typename Template<_Kind_>::ConstantReference kind)
	{
		Insert(kind,0);
	}

	inline int Index(const Functor<int,_Kind_> & compare)
	{

		int first	= 0;
		int last	= this->Size-1;
		int middle	= first + (int)((last-first+1)>>1);
		int result	= 0;

		while (first <= last)
		{
			result = compare(Data[middle]);
			if (result == 0) break;

			if (result < 0)
				last = middle-1;
			else
				first = middle+1;

			middle = first + (int)((last-first+1)>>1);				
		}	

		return middle;		
	}

	inline int Index(typename Template<_Kind_>::ConstantReference kind)
	{
		int index=Size;
		while(index-- > 0)
		{
			if (Data[index] == kind)
			{
				return index;					
			}
		}

		return -1;
	}

	inline bool Remove(typename Template<_Kind_>::ConstantReference kind)
	{
		int index=Size;
		while(index-- > 0)
		{
			if (Data[index] == kind)
			{
				Array<_Kind_>::Move(Data+index,Data+index+1,--Size-index);
				return true;					
			}
		}

		return false;
	}

	inline bool Remove(typename Template<_Kind_>::ConstantReference kind, int index)
	{
		if (!Indices::Normalise(index,Size))
			return false;

		if (Data[index] == kind)
		{
			Array<_Kind_>::Move(Data+index,Data+index+1,--Size-index);
			return true;					
		}
		return false;		
	}

	inline bool Select(typename Template<_Kind_>::ConstantReference kind)
	{
		int index=Size;
		while(index-- > 0)
			if (Data[index] == kind)
				return true;					

		return false;				
	}

	inline bool Select(typename Template<_Kind_>::Reference kind, int index)
	{
		if (!Indices::Normalise(index,Size))
			return false;

		if (&kind == &Data[index])
			return true;

		return false;
	}

	void Unique(const Callback<int,_Kind_,_Kind_> & compare)
	{
		Sort();
		for (int i=0;i<Size-1;++i)
		{	
			if (compare(Data[i],Data[i+1]) == 0)
				Array<_Kind_>::Move(Data[i],Data[i+1],--Size-i);
		}	
	}

	void Swap(int i,int j)
	{

		if (i != j)
		{
			_Kind_ data = Data[i];
			Data[i] = Data[j];
			Data[j] = data;
		}
	}

	void Shuffle(int seed=1)
	{
		if (Size > 2)
		{
			srand(seed);
			for (int i=Size-1;i>0;--i)
				Swap(i,rand()%i);
		}
	}

	template<typename _Compare_>
	void Sort(_Compare_ compare)
	{
		Sort(0,Size-1,compare);
	}

	void Sort(int first, int last, const Callback<int,_Kind_,_Kind_> & compare)
	{
		if (!Indices::Normalise(first,Size) || !Indices::Normalise(last,Size))
			return false;

		if (last < first) return;

		Array<Range> stack;
		stack.Allocate(last-first);
		stack.Insert(Range(first,last));

		while (stack.Size > 0)
		{

			Range range = stack.At(stack.Size-1);
			stack.Remove(range,stack.Size-1);

			first = range.First;
			last = range.Last;

			if (first < last)
			{

				int left = first+1;
				int right = last;

				while(right >= left)
				{
					while (left < last && compare(Data[left],Data[first]) < 0)
						++left;

					while (right > first && compare(Data[right],Data[first]) > 0)
						--right;

					if (left >= right) break;

					Swap(left,right);
				}

				if (right == first && left == first+1)
					continue;

				Swap(first,right);

				stack.Append(Range(first,right-1));
				stack.Append(Range(left+1,last));

			}

		}
	}					

};

template<typename _Kind_, int _Factor_>
const float Array<_Kind_,_Factor_>::Factor = (_Factor_>100)?(float)_Factor_/100:1.5;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
class Stack : public Array<_Kind_>
{
public:

	inline bool Push(typename Template<_Kind_>::ConstantReference kind)
	{
		return Append(kind)	
	}

	inline bool Pop(typename Template<_Kind_>::Reference kind)
	{
		return Remove(kind,Length()-1);		
	}

	inline bool Pop()
	{
		_Kind_ kind;
		Select(kind,Length()-1);Remove(kind,Length()-1);
		return kind;		
	}	

	inline bool Peek(typename Template<_Kind_>::Reference kind)
	{
		return Select(kind,Length()-1);
	}
};

template<typename _Kind_>
class Stack<_Kind_*> : public Array<_Kind_*>
{
public:

	inline bool Push(typename Template<_Kind_>::ConstantReference kind)
	{
		return Append(kind)	
	}

	inline bool Pop(typename Template<_Kind_>::Reference kind)
	{
		return Select(kind,Length()-1) && Remove(kind,Length()-1);		
	}

	inline _Kind_ * Pop()
	{
		_Kind_ * kind;
		Select(kind,Length()-1);Remove(kind,Length()-1);
		return kind;		
	}

	inline bool Peek(typename Template<_Kind_>::Reference kind)
	{
		return Select(kind,Length()-1);
	}

	inline _Kind_ * Peek()
	{
		_Kind_ * kind;
		Select(kind,Length()-1);
		return kind;
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_, int _Allocated_=4, int _Factor_=175>
class Vector
{
public:

	static const float Factor; 

	_Kind_ * Data;
	int Size;
	int Allocated;

	_Kind_ Buffer[(_Allocated_*sizeof(_Kind_)>4*sizeof(int))?0:_Allocated_];
	Vector(int allocated)
	{
		Allocated = sizeof(Buffer)/sizeof(_Kind_);

		if (allocated > sizeof(Buffer))
		{
			Data = new _Kind_[allocated];		
			Allocated = allocated;
		}

		Size = 0;		
	}

	Vector():Size(0)
	{
		Data = Buffer;
		#ifdef REASON_PLATFORM_X86_64
		Allocated = (_Allocated_*sizeof(_Kind_)>4*sizeof(int))?0:_Allocated_;
		#else
		Allocated = sizeof(Buffer)/sizeof(_Kind_);
		#endif
	}

	Vector(const Vector & vector):Size(0)
	{
		Data = Buffer;
		#ifdef REASON_PLATFORM_X86_64
		Allocated = (_Allocated_*sizeof(_Kind_)>4*sizeof(int))?0:_Allocated_;
		#else
		Allocated = sizeof(Buffer)/sizeof(_Kind_);
		#endif

		if (vector.Size > Allocated)
		{
			Data = new _Kind_[vector.Size];								
			Allocated = vector.Size;					
		}				

		Array<_Kind_>::Copy(Data,vector.Data,vector.Size);	
		Size = vector.Size;
	}

	~Vector()
	{
		if (Data != Buffer && Data != 0)
		{

			Array<_Kind_>::Delete(Data);
		}

		Data = 0;
	}

	void Release()
	{
		if (Data != Buffer && Data != 0)
		{

			Size=0;
		}
	}

	void Destroy()
	{
		if (Data != Buffer && Data != 0)
		{
			for (int i=0;i<Length();++i)
				Disposer<_Kind_>::Destroy(Data[i]);

			Array<_Kind_>::Delete(Data);
			Data=0;
			Size=0;
			Allocated=0;
		}
	}	

	inline _Kind_ & At(int index)
	{
		if (Indices::Normalise(index,Size))
			return Data[index];

		return Null<_Kind_>::Value();
	}

	int Length()
	{
		return Size;
	}

	_Kind_ & operator [] (int index)
	{
		return At(index);
	}

	inline void Allocate(int amount=0)
	{

		if (amount == 0)
		{	
			amount = ((Allocated <= 0)?((Size>0)?Size:1):(Allocated*Factor))+1;
		}
		else
		if (amount < 0 || amount < Size)
		{
			amount = Size;
		}

		if (Data == Buffer)
		{
			_Kind_ * data;
			Array<_Kind_>::Allocate(data,amount);
			Array<_Kind_>::Copy(data,Data,Size);	

			Data = data;
		}
		else			
		if (Allocated > 0)
		{
			Array<_Kind_>::Reallocate(Data,Size,amount);
		}
		else
		{
			_Kind_ * data;
			Array<_Kind_>::Allocate(data,amount);

			if (Size > 0)
				Array<_Kind_>::Copy(data,Data,Size);

			Data = data;
		}

		Allocated = amount;									

	}

	Vector & operator = (const Vector & vector)
	{
		if (this != &vector)
		{
			if (vector.Size > 0)
			{
				if (vector.Size > Allocated)
				{
					if (Allocated && Data != Buffer)
						Array<_Kind_>::Delete(Data);

					Allocated = vector.Size;																		
					Array<_Kind_>::Allocate(Data,Allocated);					
				}	

				Array<_Kind_>::Copy(Data,vector.Data,vector.Size);
			}

			Size = vector.Size;
		}

		return *this;
	}		

	inline bool Insert(typename Template<_Kind_>::ConstantReference kind)
	{
		if (Size >= Allocated)
			Allocate();

		Data[Size++] = kind;
		return true;
	}

	inline bool Insert(typename Template<_Kind_>::ConstantReference kind, int index)
	{
		if (index != Length() && !Indices::Normalise(index,Size))
			return false;

		if (Size >= Allocated)		
			Allocate();

		Array<_Kind_>::Move(Data+index+1,Data+index,Size-index);		

		Data[index] = Reference<_Kind_>::Template(kind);
		++Size;

		return true;		
	}

	inline bool Append(typename Template<_Kind_>::ConstantReference kind)
	{
		return Insert(kind,Size);
	}

	inline bool Prepend(typename Template<_Kind_>::ConstantReference kind)
	{
		return Insert(kind,0);
	}

	inline int Index(const Callback<int,_Kind_> & compare)
	{

		int first	= 0;
		int last	= this->Size-1;
		int middle	= first + (int)((last-first+1)>>1);
		int result	= 0;

		while (first <= last)
		{
			result = compare(Data[middle]);
			if (result == 0) break;

			if (result < 0)
				last = middle-1;
			else
				first = middle+1;

			middle = first + (int)((last-first+1)>>1);				
		}	

		return middle;		
	}

	inline int Index(typename Template<_Kind_>::ConstantReference kind)
	{
		int index=Size;
		while(index-- > 0)
		{
			if (Data[index] == kind)
				return index;
		}

		return -1;
	}

	inline bool Remove(typename Template<_Kind_>::ConstantReference kind)
	{
		int index=Size;
		while(index-- > 0)
		{
			if (Data[index] == kind)
			{
				Array<_Kind_>::Move(Data+index,Data+index+1,--Size-index);
				return true;
			}
		}

		return false;
	}

	inline bool Remove(typename Template<_Kind_>::ConstantReference kind, int index)
	{
		if (!Indices::Normalise(index,Size))
			return false;

		if (Data[index] == kind)
		{
			Array<_Kind_>::Move(Data+index,Data+index+1,--Size-index);
			return true;					
		}
		return false;		
	}

	inline bool Select(typename Template<_Kind_>::ConstantReference kind)
	{
		int index=Size;
		while(index-- > 0)
			if (Data[index] == kind)
				return true;

		return false;				
	}

	inline bool Select(typename Template<_Kind_>::Reference kind, int index)
	{
		if (!Indices::Normalise(index,Size))
			return false;

		kind = Data[index];
	}

	void Unique(const Callback<int,_Kind_,_Kind_> & compare)
	{
		Sort();
		for (int i=0;i<Size-1;++i)
		{	
			if (compare(Data[i],Data[i+1]))
				Array<_Kind_>::Move(Data[i],Data[i+1],--Size-i);
		}	
	}

	void Swap(int i,int j)
	{

		if (i != j)
		{
			_Kind_ data = Data[i];
			Data[i] = Data[j];
			Data[j] = data;
		}
	}

	void Shuffle(int seed=1)
	{
		if (Size > 2)
		{
			srand(seed);
			for (int i=Size-1;i>0;--i)
				Swap(i,rand()%i);
		}
	}

	void Sort(const Callback<int,_Kind_,_Kind_> & compare)
	{
		Sort(0,Size-1,compare);
	}

	void Sort(int first, int last, const Callback<int,_Kind_,_Kind_> & compare)
	{
		if (!Indices::Normalise(first,Size) || !Indices::Normalise(last,Size))
			return false;

		if (last < first) return;

		Vector<Range> stack;
		stack.Allocate(last-first);
		stack.Insert(Range(first,last));

		while (stack.Size > 0)
		{

			Range range = stack.At(stack.Size-1);
			stack.Remove(range,stack.Size-1);

			first = range.First;
			last = range.Last;

			if (first < last)
			{

				int left = first+1;
				int right = last;

				while(right >= left)
				{
					while (left < last && compare(Data[left],Data[first]) < 0)
						++left;

					while (right > first && compare(Data[right],Data[first]) > 0)
						--right;

					if (left >= right) break;

					Swap(left,right);
				}

				if (right == first && left == first+1)
					continue;

				Swap(first,right);

				stack.Append(Range(first,right-1));
				stack.Append(Range(left+1,last));
			}

		}
	}					

};	

template<typename _Kind_, int _Allocated_, int _Factor_>
const float Vector<_Kind_,_Allocated_,_Factor_>::Factor = (_Factor_>100)?(float)_Factor_/100:1.5;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
class Dict
{
public:

};

template<typename _Kind_>
class Hashtable
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_, int _Capacity_=075>
class Table
{
public:

	static const float Capacity; 

	Vector< Vector<_Kind_> > Entries;

	Table()
	{
	}

	inline int Hash(int key)
	{

		key = ~key + (key << 15);
		key = key ^ (key >> 12);
		key = key + (key << 2);
		key = key ^ (key >> 4);
		key = key * 2057;
		key = key ^ (key >> 16);
		return key;
	}

	inline bool Insert(_Kind_ kind)
	{

		if (Entries.Size > Entries.Allocated*_Capacity_)
		{
			Vector< Vector<_Kind_> > entries;
			entries.Allocated = (Entries.Allocated*1.5)+1;
			entries.Data = new Vector<_Kind_>[entries.Allocated];

			int size = Entries.Size;
			entries.Size = entries.Allocated;

			for (int index=0;index<Entries.Allocated;++index)
			{						
				for (int chain=0;chain<Entries[index].Size;++chain)
				{
					_Kind_ & k = Entries[index][chain];

					int key = (Hash(k)&(entries.Allocated-1));

					entries[key].Insert(k);					
				}
			}

			if (entries.Data != entries.Buffer)
			{
				if (Entries.Data != Entries.Buffer)
					delete [] Entries.Data;

				Entries.Data = entries.Data;
				Entries.Allocated = entries.Allocated;
				entries.Data = 0;
			}
			else				
			{
				Entries = entries;
			}

			Entries.Size = size;				
		}

		int key = (Hash(kind)&(Entries.Allocated-1));
		Entries[key].Insert(kind);						

		++Entries.Size;

		return true;
	}

	inline bool Remove(_Kind_ kind)
	{
		int key = (Hash(kind)&(Entries.Allocated-1));
		return Entries[key].Remove(kind);	
	}

	inline bool Select(_Kind_ kind)
	{
		int key = (Hash(kind)&(Entries.Allocated-1));
		return Entries[key].Select(kind);	
	}

};

template<typename _Kind_, int _Capacity_>
const float Table<_Kind_,_Capacity_>::Capacity = (float)_Capacity_/100;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_, int _Factor_=150>
class Ring
{
public:

	static int Move(Ring & to, int toIndex, Ring & from, int fromIndex, int size)
	{
		Indices::Normalise(fromIndex,from.Length());

		int amount = Copy(to,toIndex,from,fromIndex,size);
		if (size > amount);
			size = amount;

		int index = (from.First+fromIndex)%from.Allocated;

		if (from.First > from.Last)
		{
			if (index+size > from.Allocated)
			{
				amount = from.Allocated-index;
				Array<_Kind_>::Move(from.Data+from.First+amount,from.Data+from.First,(from.Allocated-from.First)-amount);
				Array<_Kind_>::Move(from.Data,from.Data+(size-amount),from.Last-(size-amount));
			}
			else
			{
				Array<_Kind_>::Move(from.Data+from.First+size,from.Data+from.First,(from.Allocated-from.First)-size);
			}
		}
		else
		{
			Array<_Kind_>::Move(from.Data+index,from.Data+index+size,(from.Last-index)-size);
		}

		return size;
	}

	static int Copy(Ring & to, int toIndex, Ring & from, int fromIndex, int size)
	{

		Indices::Normalise(fromIndex,from.Length());

		int amount = from.Length()-fromIndex;
		if (size > amount)
			size = amount;		

		int index = (from.First+fromIndex)%from.Allocated;

		int insert = 0;
		if (from.First > from.Last)
		{
			if (index+size > from.Allocated)
			{
				amount = from.Allocated-index;
				insert = Insert(to,toIndex,from.Data+index,amount);
				if (insert < amount) return insert;
				insert += Insert(to,toIndex+amount,from.Data,size-amount);
				if (insert < size) return insert;
			}
			else
			{
				insert = Insert(to,toIndex,from.Data+index,size);
				if (insert < size) return insert;
			}
		}
		else
		{
			insert = Insert(to,toIndex,from.Data+index,size);
			if (insert < size) return insert;
		}

		return size;
	}

	static int Insert(Ring & to, int toIndex, _Kind_ * data, int size)
	{
		Indices::Normalise(toIndex,to.Length());

		int amount = to.Allocated-to.Length();

		if (amount < size)
			size = amount;

		if (size <= 0)
			return 0;

		int index = (to.First+toIndex)%to.Allocated;

		_Kind_ * offset;
		int left=0,right=0;
		int first=0,last=0;

		enum RingOps
		{
			DATA_LINEAR			=(1),
			DATA_NONLINEAR		=(1)<<1,
			MOVE_WRAP_LEFT		=(1)<<2,
			MOVE_WRAP_RIGHT		=(1)<<3,
			MOVE_NOWRAP_LEFT	=(1)<<4,
			MOVE_NOWRAP_RIGHT	=(1)<<5,
			MOVE_NOWRAP = MOVE_NOWRAP_LEFT|MOVE_NOWRAP_RIGHT,
			MOVE_WRAP = MOVE_WRAP_LEFT|MOVE_WRAP_RIGHT,
		};

		int op;
		if (size > 0)
		{
			op=0;
			if (to.First > to.Last)
			{			
				first = to.Allocated-to.First;
				last = to.Last;

				op |= DATA_NONLINEAR; 	
				if (index >= to.First)
				{					
					offset = to.Data+to.First;
					left = index-to.First;
					right = first-left;					
					if (left > right+to.Last)
						op |= MOVE_WRAP_RIGHT;
					else
						op |= MOVE_NOWRAP_LEFT;
				}				
				else
				if (index <= to.Last)
				{
					offset = to.Data;
					left = index;
					right = to.Last-index;
					if (left+first < right)
						op |= MOVE_WRAP_LEFT;
					else
						op |= MOVE_NOWRAP_RIGHT;
				}	
			}
			else
			{
				op |= DATA_LINEAR;

				offset = to.Data;
				left = index;
				right = to.Last-index;

				if (left < right)
					op |= MOVE_WRAP_LEFT;
				else
					op |= MOVE_NOWRAP_RIGHT;
			}

			if (op&MOVE_NOWRAP)
			{
				if (op&MOVE_NOWRAP_LEFT)
				{

					Array<_Kind_>::Move(offset-size,offset,left);
					Array<_Kind_>::Copy(offset+left-size,data,size);
					to.First -= size;
				}
				else

				{

					Array<_Kind_>::Move(offset+left+size,offset+left,right);
					Array<_Kind_>::Copy(offset+left,data,size);	
					to.Last += size;
				}
			}
			else

			{
				if (op&MOVE_WRAP_LEFT)
				{

					if (op&DATA_NONLINEAR)
					{
						Array<_Kind_>::Move(to.Data+to.First-size,to.Data+to.First,first);
					}

					index = to.First-size;
					Indices::Normalise(index,to.Length());
					to.First = index;

					offset = to.Data+to.Allocated-size;
					if (size > left)
					{
						Array<_Kind_>::Copy(offset,to.Data,left);
						Array<_Kind_>::Copy(to.Data,data+(size-left),left);
						Array<_Kind_>::Copy(offset+left,data,(size-left));
					}
					else
					{
						Array<_Kind_>::Copy(offset,to.Data,size);
						Array<_Kind_>::Move(to.Data,to.Data+size,(left-size));
						Array<_Kind_>::Copy(to.Data+(left-size),data,size);
					}

				}
				else

				{

					Array<_Kind_>::Move(to.Data+size,to.Data,last);
					to.Last += size%to.Allocated;

					offset = to.Data+to.Allocated-size;
					if (size > right)
					{
						Array<_Kind_>::Copy(to.Data+(size-right),to.Data+to.First+left,right);
						Array<_Kind_>::Copy(to.Data+to.First+left,data,right);
						Array<_Kind_>::Copy(to.Data,data+(size-right),(size-right));
					}
					else
					{
						Array<_Kind_>::Copy(to.Data,offset,size);
						Array<_Kind_>::Move(to.Data+to.First+left+size,to.Data+to.First+left,(right-size));
						Array<_Kind_>::Copy(to.Data+to.First+left,data,size);	
					}	

				}
			}

		}

		return size;
	}

	static int Move(Ring<_Kind_> & ring, int to, int from, int size)
	{
		if (!Indices::Normalise(to,ring.Length())) return 0;
		if (!Indices::Normalise(from,ring.Length())) return 0;

		int diff = to - from;
		if (diff == 0) return 0;

		OutputAssert((diff < 0 && from+size < ring.Length()) || (diff > 0 && to+size+diff < ring.Length()));

		int amount = 0;
		if (ring.First > ring.Last)
		{
			to = (ring.First+to)%ring.Allocated;
			from = (ring.First+from)%ring.Allocated;

			if (diff > 0)
			{

				if (from+size > ring.Allocated)
				{
					amount = (from+size)-ring.Allocated;
					Array<_Kind_>::Move(ring.Data+diff,ring.Data,amount);	
					size -= amount;					
				}

				if (to+size > ring.Allocated)
				{
					amount = (to+size)-ring.Allocated;
					Array<_Kind_>::Copy(ring.Data,ring.Data+from+size-amount,amount);
					size -= amount;
				}

				Array<_Kind_>::Move(ring.Data+to,ring.Data+from,size);
			}
			else
			{
				diff = abs(diff);

				if (from+size < ring.Allocated)
					amount = size;
				else
					amount = ring.Allocated-from;

				Array<_Kind_>::Move(ring.Data+to,ring.Data+from,amount);

				if (from+size > ring.Allocated)
				{
					int offset = to+amount;
					amount = min(diff,(from+size)-ring.Allocated);
					Array<_Kind_>::Copy(ring.Data+offset,ring.Data,amount);
				}

				if (to+size > ring.Allocated)
				{
					amount = (to+size)-ring.Allocated;
					Array<_Kind_>::Move(ring.Data,ring.Data+diff,amount);
				}

			}

		}	
		else
		{
			Array<_Kind_>::Move(ring.Data+to,ring.Data+from,size);
		}

	}

public:

	static const float Factor; 

	_Kind_ * Data;
	int Allocated;

	int First;
	int Last;

	Ring(int allocated):First(0),Last(0)
	{
		Data = new _Kind_[allocated];
		Allocated = allocated;
	}

	Ring():Data(0),Allocated(0),First(0),Last(0)
	{
	}

	Ring(const Ring & ring)
	{			
		Allocated = ((Ring&)ring).Length();
		Data = new _Kind_[Allocated];

		Array<_Kind_>::Copy(Data,ring.Data,Allocated);
		First = ring.First;
		Last = ring.Last;
	}

	~Ring()
	{
		if (Data != 0)
		{
			Array<_Kind_>::Delete(Data);
		}

		Data = 0;
	}

	void Release()
	{
		if (Data)
		{

			Size=0;
		}
	}

	void Destroy()
	{
		if (Data)
		{
			for (int i=0;i<Length();++i)
				Disposer<_Kind_>::Destroy(Data[i]);

			Array<_Kind_>::Delete(Data);
			Data=0;
			Size=0;
			Allocated=0;
		}
	}

	inline _Kind_ & At(int index)
	{
		if (Indices::Normalise(index,Length()))

			return Data[(First+index)%Allocated];

		return Null<_Kind_>::Value();
	}

	int Length()
	{
		return Last + ((First>Last)?Allocated-First:0);
	}

	_Kind_ & operator [] (int index)
	{
		return At(index);
	}

	inline void Allocate(int amount)
	{

		if (amount == 0)
		{	
			amount = ((Allocated == 0)?2:(Allocated*Factor))+1;			
		}
		else
		if (amount < 0 || amount < Length())
		{
			amount = Length();
		}

		if (Allocated > 0)
		{

			Array<_Kind_>::Reallocate(Data,Length(),amount);

			int offset = 0;
			if (First > Last)
			{
				offset = min(amount-Allocated,Last);
				Array<_Kind_>::Copy(Data+Allocated,Data,offset);
				if (offset < Last)
					Array<_Kind_>::Move(Data,Data+offset,Last-offset);
				Last -= offset;
			}

		}
		else
		{				
			_Kind_ * data;		
			Array<_Kind_>::Allocate(data,amount);

			int offset = 0;
			if (First > Last)
				Array<_Kind_>::Copy(data,Data+First,(offset=amount-First));

			Array<_Kind_>::Copy(data+offset,Data,Last);

			Last += offset;
			First = 0;						

			if (Allocated)
				Array<_Kind_>::Delete(Data);

			Data = data;
		}

		Allocated = amount;									

	}

	Ring & operator = (const Ring & ring)
	{
		if (this != &ring)
		{
			if (ring.Length() > 0)
			{
				if (ring.Length() > Allocated)
				{
					if (Allocated)
						Array<_Kind_>::Delete(Data);									

					Allocated = ring.Length();						
					Array<_Kind_>::Allocate(Data,Allocated);					
				}	

				int offset = 0;
				if (ring.First > ring.Last)
					Array<_Kind_>::Copy(Data,ring.Data+ring.First,(offset=ring.Allocated-ring.First));

				Array<_Kind_>::Copy(Data+offset,ring.Data,ring.Last);
				Last += offset;
				First = 0;							
			}
		}

		return *this;
	}		

	inline bool Insert(typename Template<_Kind_>::ConstantReference kind)
	{
		if (Length() >= Allocated)
		{
			int amount = ((Allocated <= 0)?2:(Allocated*Factor))+1;		

			_Kind_ * data = 0;		

			Array<_Kind_>::Allocate(data,amount);

			if (Data != 0)
			{
				int offset = 0;
				if (First > Last)
					Array<_Kind_>::Copy(data,Data+First,(offset=Allocated-First));

				Array<_Kind_>::Copy(data+offset,Data,Last);

				Last += offset;
				First = 0;		

				Array<_Kind_>::Delete(Data);
			}				

			Data = data;
			Allocated = amount;
		}

		Data[Last++] = kind;
		return true;
	}

	inline bool Insert(typename Template<_Kind_>::ConstantReference kind, int index)
	{
		if (index != Length() && !Indices::Normalise(index,Length()))	
			return false;

		index = (First+index) % Allocated;

		if (Length() >= Allocated)
		{
			int amount = ((Allocated == 0)?2:(Allocated*Factor))+1;			

			_Kind_ * data = 0;		
			Array<_Kind_>::Allocate(data,amount);

			int offset = 0;

			if (First > Last)
			{
				if (index >= First)
				{
					offset = index-First;
					Array<_Kind_>::Copy(data,Data+First,offset);
					data[offset] = kind;
					Array<_Kind_>::Copy(data+offset+1,Data+First+offset,(Allocated-First)-offset);
					offset = (Allocated-First)+1;
				}
				else
				{
					Array<_Kind_>::Copy(data,Data+First,(offset=Allocated-First));
				}				
			}

			if (index <= Last)
			{
				Array<_Kind_>::Copy(data+offset,Data,index);
				data[offset+index] = kind;
				Array<_Kind_>::Copy(data+offset+index+1,Data+index,Last-index);
				offset += 1;
			}
			else
			{
				Array<_Kind_>::Copy(data+offset,Data,Last);
			}			

			Last += offset;
			First = 0;		
			Data = data;	
			Allocated = amount;		
		}
		else
		{

			if (First > Last && index >= First)
			{
				if (index == First)
				{
					Data[--First] = kind;
				}
				else
				{
					Array<_Kind_>::Move(Data+First-1,Data+First,index-First);
					Data[index-1] = kind;
					--First;			
				}
			}
			else
			if (index <= Last)
			{
				if (index == 0)
				{

					if ((Allocated-First) < Last)
					{
						Array<_Kind_>::Move(Data+First-1,Data+First,(Allocated-First));
						Data[Allocated-1] = kind;
						--First;
					}
					else
					{
						Array<_Kind_>::Move(Data+1,Data,Last);
						Data[0] = kind;
						++Last;
					}
				}
				else
				{	
					Array<_Kind_>::Move(Data+index+1,Data+index,Last-index);
					Data[index] = kind;
					++Last;			
				}					
			}
			else
			{
				Data[Last++] = kind;
			}			
		}

		return true;		
	}

	inline bool Append(typename Template<_Kind_>::ConstantReference kind)
	{
		Insert(kind,Length());
	}

	inline bool Prepend(typename Template<_Kind_>::ConstantReference kind)
	{
		Insert(kind,0);
	}

	inline int Index(const Callback<int,_Kind_> & compare)
	{

		int first	= 0;
		int last	= Length()-1;
		int middle	= first + (int)((last-first+1)>>1);
		int result	= 0;

		while (first <= last)
		{

			result = compare(Data[(First+middle)%Allocated]);
			if (result == 0) break;

			if (result < 0)
				last = middle-1;
			else
				first = middle+1;

			middle = first + (int)((last-first+1)>>1);				
		}	

		return middle;		
	}

	inline int Index(typename Template<_Kind_>::ConstantReference kind)
	{
		int index=Length();
		while(index-- > 0)
		{
			if (Data[(First+index)%Allocated] == kind)
			{
				return index;					
			}
		}

		return -1;
	}

	inline bool Remove(typename Template<_Kind_>::ConstantReference kind)
	{
		int index=Length();
		while(index-- > 0)
		{
			if (Data[(First+index)%Allocated] == kind)
			{
				index = (First+index)%Allocated;
				if (First > Last && index >= First)
				{
					Array<_Kind_>::Move(Data+Last+1,Data+Last,index-First);
					++First;
				}
				else
				{
					Array<_Kind_>::Move(Data+index,Data+index+1,Last-index);
					--Last;
				}				
				return true;					
			}
		}

		return false;
	}

	inline bool Remove(typename Template<_Kind_>::ConstantReference kind, int index)
	{
		if (!Indices::Normalise(index,Length()))	
			return false;

		index = (First+index)%Allocated;
		if (Data[index] == kind)
		{
			if (First > Last && index >= First)
			{
				Array<_Kind_>::Move(Data+Last+1,Data+Last,index-First);
				++First;
			}
			else
			{
				Array<_Kind_>::Move(Data+index,Data+index+1,Last-index);
				--Last;
			}	

			return true;					
		}
		return false;		
	}

	inline bool Select(typename Template<_Kind_>::ConstantReference kind)
	{
		int index=Length();
		while(index-- > 0)
			if (Data[(First+index)%Allocated] == kind)
				return true;					

		return false;				
	}

	inline bool Select(typename Template<_Kind_>::Reference kind, int index)
	{
		if (!Indices::Normalise(index,Length()))	
			return false;

		kind = Data[(First+index)%Allocated];
	}

	void Unique(const Callback<int,_Kind_,_Kind_> & compare)
	{
		Sort();
		for (int i=0;i<Length()-1;++i)
		{				
			if (compare(Data[(First+i)%Allocated],Data[(First+i+1)%Allocated])==0)
				Remove(Data[i],i);
		}	
	}

	void Swap(int i,int j)
	{
		int length = Length();
		if (!Indices::Normalise(i,length) || !Indices::Normalise(j,length))	
			return;

		if (i != j)
		{
			_Kind_ data = Data[(First+i)%Allocated];
			Data[(First+i)%Allocated] = Data[(First+j)%Allocated];
			Data[(First+j)%Allocated] = data;
		}
	}

	void Shuffle(int seed=1)
	{
		if (Length() > 2)
		{
			srand(seed);
			for (int i=Length()-1;i>0;--i)
				Swap(i,rand()%i);
		}
	}

	void Sort(const Callback<int,_Kind_,_Kind_> & compare)
	{
		Sort(0,Length()-1,compare);
	}

	void Sort(int first, int last, const Callback<int,_Kind_,_Kind_> & compare)
	{
		int length = Length();
		if (!Indices::Normalise(first,length) || !Indices::Normalise(last,length))	
			return false;

		if (last < first) return;

		Ring<Range> stack;
		stack.Allocate(last-first);
		stack.Insert(Range(first,last));

		while (stack.Length() > 0)
		{

			Range range = stack.At(stack.Length()-1);
			stack.Remove(range,stack.Length()-1);

			first = range.First;
			last = range.Last;

			if (first < last)
			{

				int left = first+1;
				int right = last;

				while(right >= left)
				{
					while (left < last && compare(Data[(First+left)%Allocated],Data[(First+first)%Allocated]) < 0)
						++left;

					while (right > first && compare(Data[(First+right)%Allocated],Data[(First+first)%Allocated]) > 0)
						--right;

					if (left >= right) break;

					Swap(left,right);
				}

				if (right == first && left == first+1)
					continue;

				Swap(first,right);

				stack.Append(Range(first,right-1));
				stack.Append(Range(left+1,last));
			}
		}
	}	
};

template<typename _Kind_, int _Factor_>
const float Ring<_Kind_,_Factor_>::Factor = (_Factor_>100)?(float)_Factor_/100:1.5;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
class Deque
{
public:

	static const int Blocks[15];

	class Node : public Ring<_Kind_>
	{
	public:

		int Indexes;

		Node(int index=0):Indexes(index)
		{
		}

		~Node() 
		{		
		}

		int Compare(Node * entry)
		{

			return Indexes - entry->Indexes;
		}

		int operator() (Node * entry)
		{
			return Compare(entry);
		}

	};

	Array<Node*> Nodes;

	int Count;

	Deque():Block(0),Count(0)
	{
	}

	Deque(const Deque & deque):Block(0),Count(0)
	{

		for (int e=0;e<deque.Nodes.Length();++e)
		{

		}
	}

	~Deque()
	{
		for (int e=0;e<Nodes.Length();++e)
		{
			delete Nodes[e];
		}
	}

	void Underflow(int block)
	{
		block = Blocks[block];

		for (int n=0;n<Nodes.Size;++n)
		{

			Node * flow = Nodes[n];

			if (flow->Allocated > block)
			{

				int amount;
				Node * node;

				if (n > 0 && (node=Nodes[n-1])->Allocated <= block)
				{
					amount = ceil(((float)node->Allocated/100)*85);	

					Ring<_Kind_>::Move(*node,-1,*flow,0,amount-node->Length());					

				}

				if (n < Nodes.Size-1 && (node=Nodes[n+1])->Allocated <= block)
				{
					amount = ceil(((float)node->Allocated/100)*85);										

					Ring<_Kind_>::Move(*node,0,*flow,-1,amount-node->Length());

				}

				node = new Node();
				node->Allocate(block);

				amount = ceil(((float)node->Allocated/100)*85);	

				Ring<_Kind_>::Move(*node,0,*flow,-1,amount-node->Length());

				flow->Allocate(block);

				Nodes.Insert(node,n);	
				++n;											

			}
		}
	}

	int Block;
	void Overflow(int block)
	{	
		block = Blocks[block];

		for (int n=0;n<Nodes.Size;++n)
		{

			Node * flow = Nodes[n];

			if (flow->Allocated < block)
			{
				flow->Allocate(block);

				Node * node;				
				while (n+1 < Nodes.Size && (node=Nodes[n+1])->Allocated < block)
				{

					if (flow->Length()+node->Length() > flow->Allocated)
						break;

					int offset = 0;
					if (node->First > node->Last)
						Array<_Kind_>::Copy(flow->Data+flow->Last,node->Data+node->First,(offset=node->Allocated-node->First));

					Array<_Kind_>::Copy(flow->Data+flow->Last+offset,node->Data,node->Last);
					flow->Last += offset+node->Last;

					Nodes.Remove(node,n+1);
					delete node;
				}

			}
		}

	}

	void Reflow()
	{

		static int blocks = (sizeof(Blocks)/sizeof(int))-1;

		struct Logarithm
		{
			static int Log10(unsigned int n)
			{

				return	((n >= 10000u)? 
							((n >= 100000u)? 
								((n >= 10000000u)? 
									((n >= 100000000u)? 
										((n >= 1000000000u)?9:8) 
										:7)
									:((n >= 1000000u)? 
										6:5))
								:4)
							:((n >= 1000u)? 
								3:((n >= 100u)? 
									2:((n >= 10u)? 
										1:0)))); 

			}
		};

		int log = Logarithm::Log10(Count+1);		

		int block = (log>blocks)?blocks:log;

		if (block > Block)
			Overflow(block);
		else
		if (block < Block)
			Underflow(block);

		Block = block;
	}

	inline _Kind_ & At(int index)
	{
		OutputAssert(index < Count && index >= 0);

		Node comp(index);	

		Node * node = 0;

		Nodes.Select(node,Nodes.Index(comp));

		OutputAssert(node != 0);		
		index -= node->Indexes;
		return node->At(index);
	}

	int Length()
	{
		return Count;
	}

	_Kind_ & operator [] (int index)
	{

		return At(index);

	}

	inline void Allocate(int amount=0)
	{

		Reflow() 				
	}		

	Deque & operator = (const Deque & deque)
	{

		return *this;
	}		

	inline bool Insert(typename Template<_Kind_>::ConstantReference kind)
	{
		Node * node = Nodes[-1];		
		if (!node || node->Length() >= node->Allocated)
		{
			int block = Blocks[Block];
			node = new Node();
			node->Allocate(block);							
			node->Insert(kind);

			Nodes.Insert(node);				
			Reflow();				
		}
		else
		{	
			node->Insert(kind);
		}

		++Count;

		return true;
	}

	inline bool Insert(typename Template<_Kind_>::ConstantReference kind, int index)
	{
		if (index != Length() && !Indices::Normalise(index,Length()))
			return false;

		OutputAssert(index <= Count && index >= 0);

		Node comp(index);	
		Callback<int,Node*> indexof(&comp,&Node::Compare);			
		Node * node = 0;
		Nodes.Select(node,Nodes.Index(indexof));

		if (node)
		{
			while (node && node->Length() >= node->Allocated)
			{

				int block = Blocks[Block];
				node = new Node();
				node->Allocate(block);							
				Nodes.Insert(node);				
				Reflow();	

				Nodes.Select(node,Nodes.Index(comp));		
				OutputAssert(node != 0);
				if (!node)
					return false;
			}

			index -= node->Indexes;
			return node.Insert(kind,index);
		}
		else
		{
			OutputAssert(Length() == 0);
			return Insert(kind);
		}

		return false;		
	}

	inline bool Append(typename Template<_Kind_>::ConstantReference kind)
	{
		Insert(kind,Length());
	}

	inline bool Prepend(typename Template<_Kind_>::ConstantReference kind)
	{
		Insert(kind,0);
	}

	inline int Index(const Callback<int,_Kind_> & compare)
	{
		int first	= 0;
		int last	= this->Count-1;
		int middle	= first + (int)((last-first+1)>>1);
		int result	= 0;

		Node * node = 0;
		while (first <= last)
		{
			result = compare(At(middle));
			if (result == 0) break;

			if (result < 0)
				last = middle-1;
			else
				first = middle+1;

			middle = first + (int)((last-first+1)>>1);				
		}	

		return middle;		
	}

	inline int Index(typename Template<_Kind_>::ConstantReference kind)
	{
		int result = 0;
		int index = Nodes.Length();
		while (index-- > 0)
		{
			Node * node = Nodes[index];
			result = node->Index(kind);
			if (result != -1)
				return result+node->Indexes;
		}

		return -1;
	}

	inline bool Remove(typename Template<_Kind_>::ConstantReference kind)
	{
		bool result = false;
		int index = Nodes.Length();
		while (index-- > 0)
		{
			Node * node = Nodes[index];
			result = node->Remove(kind);
			if (result)
				return true;
		}

		return false;
	}

	inline bool Remove(typename Template<_Kind_>::ConstantReference kind, int index)
	{
		if (!Indices::Normalise(index,Length()))
			return false;

		OutputAssert(index <= Count && index >= 0);

		Node comp(index);	
		Callback<int,Node*> indexof(&comp,&Node::Compare);	
		Node * node = 0;

		Nodes.Select(node,Nodes.Index(indexof));

		if (node)
		{
			index -= node->Indexes;
			if (node->Remove(kind,index))
			{
				if (Block > 0)				
				{	
					int block = Blocks[Block-1];
					if (block >= node->Length())
						Reflow();
				}
				else
				if (node->Length() == 0)
				{
					Nodes.Remove(node);
					delete node;
				}

				return true;
			}
		}

		return false;		
	}

	inline bool Select(typename Template<_Kind_>::ConstantReference kind)
	{
		bool result = false;
		int index = Nodes.Length();
		while (index-- > 0)
		{
			Node * node = Nodes[index];
			result = node->Select(kind);
			if (result)
				return true;
		}

		return false;		
	}

	inline bool Select(typename Template<_Kind_>::Reference kind, int index)
	{
		if (!Indices::Normalise(index,Length()))
			return false;

		OutputAssert(index <= Count && index >= 0);

		Node comp(index);				
		Node * node = 0;
		Nodes.Select(node,Nodes.Index(comp));

		if (node)
		{
			index -= node->Indexes;
			if (node->Select(kind,index))
				return true;
		}

		return false;
	}

	void Unique(const Callback<int,_Kind_,_Kind_> & compare)
	{
		Sort();

		for (int i=0;i<Length()-1;++i)
		{
			_Kind_ & kind = At(i);
			if (compare(kind,At(i+1)) == 0)
			{
				Remove(kind,i+1);
				Insert(kind,i);
			}
		}	
	}

	void Swap(int i,int j)
	{
		OutputAssert(i >= 0 && i < Count && j >= 0 && j < Size);
		if (i != j)
		{
			_Kind_ tmp = At(i);
			At(i) = At(j)
			At(j) = tmp;
		}
	}

	void Shuffle(int seed=1)
	{
		if (Size > 2)
		{
			srand(seed);
			for (int i=Count-1;i>0;--i)
				Swap(i,rand()%i);
		}
	}

	void Sort(const Callback<int,_Kind_,_Kind_> & compare)
	{
		Sort(0,Count-1,compare);
	}

	void Sort(int first, int last, const Callback<int,_Kind_,_Kind_> & compare)
	{
		if (last < first) return;

		Vector<Range> stack;
		stack.Allocate(last-first);
		stack.Insert(Range(first,last));

		while (stack.Size > 0)
		{

			Range range = stack.At(stack.Size-1);
			stack.Remove(range,stack.Size-1);

			first = range.First;
			last = range.Last;

			if (first < last)
			{

				int left = first+1;
				int right = last;

				while(right >= left)
				{
					while (left < last && compare(At(left),At(first)) < 0)
						++left;

					while (right > first && compare(At(right),At(first)) > 0)
						--right;

					if (left >= right) break;

					Swap(left,right);
				}

				if (right == first && left == first+1)
					continue;

				Swap(first,right);

				stack.Append(Range(first,right-1));
				stack.Append(Range(left+1,last));
			}

		}
	}					

};	

template <typename _Kind_>

const int Deque<_Kind_>::Blocks[] = {4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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