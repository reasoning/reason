
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
#ifndef STRUCTURE_ARRAY_H
#define STRUCTURE_ARRAY_H

#pragma warning(disable: 4503)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/generic/generic.h" 
#include "reason/structure/iterator.h" 
#include "reason/system/interface.h" 
#include "reason/structure/disposable.h"
#include "reason/platform/thread.h"

using namespace Reason::Generic;

namespace Reason { namespace Structure {

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

namespace Abstract {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 

class Array : public Iterable<_Kind_>, public Comparable
{
public:

	static Identity Instance;

	Identity & Identify() {return Instance;}

public:

	_Kind_ * Data;
	int	Size;
	int	Allocated;

	Array(const Array & array, bool consume=false):

		Iterable<_Kind_>(new _Policy_()),Data(0),Size(0),Allocated(0)
	{
		if (consume)
			Consume(array);
		else
			operator = (array);
	}

	Array(const Iterable<_Kind_> & iterable):

		Iterable<_Kind_>(new _Policy_()),Data(0),Size(0),Allocated(0)
	{
		Reason::Structure::Iterator<_Kind_> iterator = ((Iterable<_Kind_> &)iterable).Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());
	}

	Array(int size):

		Iterable<_Kind_>(new _Policy_()),Data(0),Size(0),Allocated(0)
	{
		Allocate(size);
		Initialise();
		Size = size;
	}

	Array(int size, typename Template<_Kind_>::ConstantReference type):

		Iterable<_Kind_>(new _Policy_()),Data(0),Size(0),Allocated(0)
	{
		Allocate(size);
		Set(Data,type,size);
		Size = size;
	}

	Array():

		Iterable<_Kind_>(new _Policy_()),Data(0),Size(0),Allocated(0)
	{
	}

	~Array()
	{
		if (Data)
			delete [] Data;
	}

	Array & operator = (const Iterable<_Kind_> & iterable)
	{
		Reason::Structure::Iterator<_Kind_> iterator = ((Iterable<_Kind_> &)iterable).Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());

		return *this;
	}

	Array & operator = (const Array & array)
	{
		if (&array == this) return *this;

		if (Data)
			delete [] Data;

		Data = 0;
		Allocated = Size = 0;

		if (array.Allocated > 0)
		{
			Allocate(array.Size);
			Copy(Data,array.Data,array.Size);
		}	

		Size = array.Size;			

		return *this;
	}

	virtual float Increment() {return 0.25;}
	virtual int Remaining() {return Allocated-Size;};

	int Length() {return Size;};

	Array & Allocate(int amount)
	{
		OutputAssert((Size > 0 && Data != 0) || (Size == 0));

		if (amount == 0)
		{

			if (!Allocated && Size > 0)
				amount = Size;
			else

				amount = ((Allocated>0)?Size:1) * (1 + Increment()) + 1;
		}
		else
		if (amount > 0)
		{
			if (amount < Size)
			{										
				OutputWarning("Array<>::Allocate - Cannot allocate less than size.\n");
				return *this;
			}
		}
		else
		if (amount < 0)
		{
			OutputError("Array<>::Allocate - Cannot allocate a negative amount.\n");			
			return *this;
		}

		_Kind_ *data = new _Kind_[amount];

		if (Size > 0)
			Copy(data,Data,Size);

		if (Allocated)
			delete [] Data;

		Data = data;
		Allocated = amount;

		Invalidate();

		return *this;
	}

	Array &  Reserve(int amount)
	{

		OutputAssert((Size > 0 && Data != 0) || (Size == 0));
		int remaining = Remaining();

		if (amount < 0)
		{
			amount = Size+(remaining-amount);
			if (amount < Size) amount = Size;
		}
		else
		if (amount > 0)
		{
			if (amount <= remaining) return *this;

			amount = Size * (1 + Increment()) + amount;
		}
		else
		{
			amount = Size;
		}

		return Allocate(amount);
	}

	Array Slice(int from)
	{		
		if (Indices::Normalise(from,Size))
		{
			Array array(Size);
			Copy(array.Data,Data+from,Size-from);

			return Array(array,true);
		}

		return Reason::System::Substring();	
	}

	Array Slice(int from, int to)
	{

		Indices::Normalise(from,Size);
		Indices::Normalise(to,Size);

		if (from >= 0 && from < Size && to > from)
		{
			Array array(Size);
			Copy(array.Data,Data+from,min(Size,to)-from);

			return Array(array,true);
		}

		return Array();	
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference type, int index)
	{

		OutputAssert(index >= 0 && index <= Size);

		if (Size == Allocated)
		{
			Allocate(0);
		}

		Move(Data+index+1,Data+index,(Size-index));
		Data[index]=(typename Template<_Kind_>::Reference)type;
		++Size;
		return Iterand<_Kind_>(this->Initial,(_Kind_*)&(char&)Data[index]);
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & iterand, int index)
	{
		return Insert(iterand(),index);
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference type)
	{

		if (Size == Allocated)
		{
			Allocate(0);
		}

		*(Data+Size)=(typename Template<_Kind_>::Reference)type;
		++Size;
		return Iterand<_Kind_>(this->Initial,(_Kind_*)&(char&)Data[Size-1]);
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & iterand)
	{

		_Kind_ * kind = 0;
		this->Put(iterand,kind);		
		if (!kind)
			return Insert(iterand());
		else
			return Insert(*kind);
	}	

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference type, int comparitor = Reason::System::Comparable::COMPARE_GENERAL)
	{
		for (int index=0;index<Size;++index)
		{
			if (Comparer<_Kind_>::Compare(Data[index],type,comparitor) == 0)
			{
				return Iterand<_Kind_>(this->Initial,(_Kind_*)&(char&)Data[index]);
			}

		}

		return Iterand<_Kind_>::Null();
	}

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> & iterand, int comparitor = Reason::System::Comparable::COMPARE_GENERAL)
	{	
		return Select(iterand(),comparitor);
	}

	virtual Iterand<_Kind_> Append(typename Template<_Kind_>::ConstantReference type)
	{
		if (Size < Allocated)
		{
			Data[Size++]=(typename Template<_Kind_>::Reference)type;
			return Iterand<_Kind_>(this->Initial,(_Kind_*)&(char&)Data[Size-1]);
		}
		else
		{
			return Insert(type,Size);
		}
	}

	virtual Iterand<_Kind_> Append(Iterand<_Kind_> & iterand)
	{	
		_Kind_ * kind = 0;
		this->Put(iterand,kind);			
		if (!kind)
			return Append(iterand());
		else
			return Append(*kind);
	}

	virtual Iterand<_Kind_> Prepend(typename Template<_Kind_>::ConstantReference type)
	{
		return Insert(type,0);
	}

	virtual Iterand<_Kind_> Prepend(Iterand<_Kind_> & iterand)
	{
		_Kind_ * kind = 0;
		this->Put(iterand,kind);			
		if (!kind)
			return Prepend(iterand());
		else
			return Prepend(*kind);
	}

	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> & iterand)
	{
		return Remove(iterand());
	}

	virtual Iterand<_Kind_> Remove()
	{
		--Size;

		Iterand<_Kind_> remove = Iterand<_Kind_>(this->Initial,Data[Size]).Dispose(Disposable::DISPOSE_POLICY);
		return remove;
	}

	virtual Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference type)
	{

		for (int i=Length()-1;i>-1;--i)
		{

			if (Comparer<_Kind_>::Compare(Data[i],type) == 0)
			{
				return RemoveAt(i);

			}
		}

		return Iterand<_Kind_>::Null();
	}

	virtual Iterand<_Kind_> RemoveAt(int index)
	{

		OutputAssert(index >= 0 && index < Size);
		if (index < 0 || index >= Size)
			Iterand<_Kind_>::Null();

		Iterand<_Kind_> remove = Iterand<_Kind_>(this->Initial,Data[index]).Dispose(Disposable::DISPOSE_POLICY);

		Move(Data+index,Data+index+1,(Size-index-1));
		--Size;

		return remove;
	}

	virtual typename Template<_Kind_>::Value RemoveValueAt(int index)
	{
		OutputAssert(index < Size);

		typename Template<_Kind_>::Value value = Data[index];
		Move(Data+index,Data+index+1,(Size-index-1));
		--Size;
		return value;
	}

	virtual Iterand<_Kind_> Delete(Iterand<_Kind_> & iterand)
	{
		return Delete(iterand());
	}

	virtual Iterand<_Kind_> Delete(typename Template<_Kind_>::ConstantReference type)
	{

		for (int i=Length()-1;i>-1;--i)
		{
			if (Comparer<_Kind_>::Compare(Data[i],type) == 0)
			{
				return DeleteAt(i);
			}
		}

		return Iterand<_Kind_>::Null();
	}

	virtual Iterand<_Kind_> DeleteAt(int index)
	{

		Iterand<_Kind_> iterand = Iterand<_Kind_>(this->Initial,Data[index]).Dispose(Disposable::DISPOSE_DESTROY);
		Move(Data+index,Data+index+1,(Size-index-1));
		--Size;
		return iterand;
	}

	virtual Iterand<_Kind_> Push(typename Template<_Kind_>::ConstantReference type)
	{
		return Insert(type);
	}

	virtual Iterand<_Kind_> Pop()
	{

		if (Size > 0)
		{
			return RemoveAt(Size-1);
		}
		else
		{
			return Iterand<_Kind_>::Null();
		}

	}

	void Consume(const Array & array)
	{

		if (Data)
		{
			for (int i=0;i<Length();++i)
			{
				Initial->Base->Dispose(Data[i]);
			}

			Size=0;
		}

		Data = array.Data;
		Size = array.Size;
		Allocated = array.Allocated;

		((Array &)array).Data = 0;
		((Array &)array).Size = 0;
		((Array &)array).Allocated = 0;
	}

	void Release()
	{
		if (Data)
		{
			for (int i=0;i<Length();++i)
			{
				Disposer<_Kind_>::Release(Data[i]);
			}

			Size=0;
		}
	}

	void Destroy()
	{
		if (Data)
		{
			for (int i=0;i<Length();++i)
			{
				Disposer<_Kind_>::Destroy(Data[i]);
			}

			delete [] Data;
			Data=0;
			Size=0;
			Allocated=0;
		}
	}

	int IndexOf(typename Template<_Kind_>::ConstantReference type, int comparitor = Reason::System::Comparable::COMPARE_GENERAL)
	{
		for (int i=0;i<Size;++i)
		{
			if (Reason::Structure::Comparer<_Kind_>::Compare(Data[i],type,comparitor)==0)
				return i;
		}
		return -1;
	}

	int Compare(Array<_Kind_> & array, int comparitor = Reason::System::Comparable::COMPARE_GENERAL)
	{
		int compare = Size-array.Size;
		if (!compare)
		{
			for (int i=0;i<Size;++i)
			{
				compare = Reason::Structure::Comparer<_Kind_>::Compare(Data[i],array[i],comparitor);
				if (compare) break;
			}

		}

		return compare;
	}

	void Assign(_Kind_ * data, int size)
	{
		Data = data;
		Size = size;
		Allocated = 0;
	}

	void Acquire(Array<_Kind_> & array)
	{
		Release();
		delete [] Data;
		Data=0;
		Size=0;
		Allocated=0;

		Data = array.Data;
		Size = array.Size;
		Allocated = array.Allocated;

		array.Data = 0;
		array.Size = 0;
		array.Allocated = 0;
	}

	void Take(Array<_Kind_> & array)
	{
		Take(array,0,array.Size);
	}

	void Take(Array<_Kind_> & array, int offset, int amount)
	{
		OutputAssert(array.Size-offset >= amount);

		if (amount == 0 || array.IsEmpty()) return;

		if (Remaining() <= amount)
			Allocate(Size + (int)(Allocated*Increment())+amount);

		Copy(Data+Size,array.Data+offset,amount);
		Move(array.Data+offset,array.Data+offset+amount,array.Size - (offset+amount));
		array.Size -= amount;
		Size += amount;
	}

	void Swap(int i,int j)
	{
		OutputAssert(i >= 0 && i < Size && j >= 0 && j < Size);
		if (i != j)
		{
			_Kind_ entry = Data[i];
			Data[i] = Data[j];
			Data[j] = entry;
		}
	}

	void Rotate(int rotation=-1)
	{

		rotation = rotation % Size;
		_Kind_ entry;

		if (rotation > 0)
		{

			while (rotation > 0)
			{

				entry = Data[Size-1];
				Remove(Size-1);
				Prepend(entry);
				-- rotation;
			}
		}
		else
		if (rotation < 0)
		{

			while (rotation < 0)
			{

				entry = Data[0];
				Remove(0);
				Append(entry);
				++rotation;
			}
		}

	}

	void Shuffle(int seed=1)
	{

		if (Size > 2)
		{

			srand(seed);

			for (int i=Size-1;i>0;--i)
			{
				Swap(i,rand()%i);
			}
		}
	}

	void Sort()
	{
		Sort(0,Length()-1);
	}

	virtual void Sort(int first, int last)
	{

		while (first < last)
		{
			int right = last;
			int left = first;

			int pivot = (int)(((long long)first+(long long)last)>>1);
			if (pivot < 0 || pivot >= last) break;

			while(right >= left)
			{

				while (left <= right && Reason::Structure::Comparer<_Kind_>::Compare(Data[left],Data[pivot]) <= 0)
					++left;

				while (left <= right && Reason::Structure::Comparer<_Kind_>::Compare(Data[right],Data[pivot]) > 0)
					--right;

				if (left > right) break;

				Swap(left,right);

				if (pivot == right) pivot = left;

				left++; 
				right--;
			}

			Swap(pivot,right);
			right--;

			if (abs(right-first) > abs(last-left))
			{
				if (left < last) Sort(left,last);	
				last = right;
			}
			else
			{
				if (first < right) Sort(first,right);
				first = left;
			}

		}
	}

	struct SortSync
	{

		int Count;
		Reason::Platform::Condition Condition;

		SortSync():Count(0) {}

		void Inc()
		{
			Atomic::Inc(&Count);
		}

		void Dec()
		{
			if (Count > 0 && Atomic::Dec(&Count) == 0)
				Condition.Signal();
		}

		bool Wait()
		{
			Condition.Wait();
			return true;
		}
	};

	struct SortData
	{
		int Left;
		int Right;
		int Parallel;
		SortSync & Sync;

		SortData(SortSync & sync, int left=0,int right=0, int parallel=0):Sync(sync),Left(left),Right(right),Parallel(parallel) {}
	};

	void Sort(int first, int last, int parallel)
	{

		if (parallel <= 0)
			parallel = 2;

		SortSync sync;

		Sort(sync,first,last,2);
		sync.Wait();

	}

	void Sort(SortSync & sync, int first, int last,int parallel)
	{
		while (first < last)
		{
			int right = last;
			int left = first;

			int pivot = (int)(((long long)first+(long long)last)>>1);
			if (pivot < 0 || pivot >= last) break;

			while(right >= left)
			{			
				while (left <= right && this->Compare(this->Data[left],this->Data[pivot],this->Comparison) <= 0)
					++left;

				while (left <= right && this->Compare(this->Data[right],this->Data[pivot],this->Comparison) > 0)
					--right;

				if (left > right) break;
				Swap(left,right);

				if (pivot == right) pivot = left;

				left++; 
				right--;
			}

			Swap(pivot,right);
			right--;

			if (parallel > 0)
			{

				if (abs(right-first) > abs(last-left))
				{

					if (first < right) Fibre::Start(Callback<void,void*>(this,&SortParallel),new SortData(sync,first,right,parallel-1));
					first = left;
				}
				else
				{
					if (left < last) Fibre::Start(Callback<void,void*>(this,&SortParallel),new SortData(sync,left,last,parallel-1));
					last = right;
				}
			}
			else
			{

				if (abs(right-first) > abs(last-left))
				{
					if (left < last) Sort(sync,left,last,-1);	
					last = right;
				}
				else
				{
					if (first < right) Sort(sync,first,right,-1);
					first = left;
				}
			}
		}
	}

	void SortParallel(void * pointer)
	{
		Fibre * fibre = (Fibre*) pointer;
		SortData * data = (SortData*)fibre->Context;
		data->Sync.Inc();

		Sort(data->Sync,data->Left,data->Right,data->Parallel);

		data->Sync.Dec();
		delete data;
	}

	typename Template<_Kind_>::Reference operator [] (int index)
	{

		OutputAssert(index >=0 && index < Size);

		return Data[index];
	}

	Array<_Kind_> & operator = (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;}
	Array<_Kind_> & operator , (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;}

	Array<_Kind_> & operator << (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;};
	Array<_Kind_> & operator >> (typename Template<_Kind_>::ConstantReference type) {Prepend(type);return *this;};

	inline typename Type<_Kind_>::Reference ValueAt(int index)			{return Reference<_Kind_>::Type(Data[index]);};
	inline typename Type<_Kind_>::Reference ReferenceAt(int index)		{return Reference<_Kind_>::Type(Data[index]);};
	inline typename Type<_Kind_>::Pointer PointerAt(int index)			{return Pointer<_Kind_>::Type(Data[index]);};
	typename Template<_Kind_>::Reference operator() (int index) {return Data[index];}

	bool IsEmpty()			{return Size==0;};

	static _Kind_ * Copy(_Kind_ * to, _Kind_ * from, int size)
	{

		OutputAssert(to+size <= from || to >= from+size);
		if (to+size >= from && to <= from+size) return 0;

		if (Generic::Is<_Kind_>::IsPointer() || Generic::Is<_Kind_>::IsPrimitive())
		{
			memcpy(to,from,sizeof(_Kind_)*size);
		}
		else
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

		return to;
	}

	static _Kind_ * Move(_Kind_ * to, _Kind_ *from, int size)
	{

		if (Generic::Is<_Kind_>::IsPointer() || Generic::Is<_Kind_>::IsPrimitive())
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

				_Kind_ * mark = from;
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

	static _Kind_ * Set(_Kind_ * to,typename Template<_Kind_>::ConstantReference type, int size)
	{

		{

			int loop = (size+7)/8;
			if (loop > 0)
			{
				switch (size%8)
				{
					case 0: do {	*to++ = (typename Template<_Kind_>::Reference)type;
					case 7:			*to++ = (typename Template<_Kind_>::Reference)type;
					case 6:			*to++ = (typename Template<_Kind_>::Reference)type;
					case 5:			*to++ = (typename Template<_Kind_>::Reference)type;
					case 4:			*to++ = (typename Template<_Kind_>::Reference)type;
					case 3:			*to++ = (typename Template<_Kind_>::Reference)type;
					case 2:			*to++ = (typename Template<_Kind_>::Reference)type;
					case 1:			*to++ = (typename Template<_Kind_>::Reference)type;
							} while (--loop > 0);
				}
			}
		}

		return to;
	}

	virtual void Initialise()
	{

		if (Generic::Is<_Kind_>::IsPointer() || Generic::Is<_Kind_>::IsPrimitive())
		{

			Variable<_Kind_> value;

			for (int i=0;i<Allocated;++i)
				Data[i] = value();
		}
	}

	class ArrayIteration : public Iteration<_Kind_>
	{
	public:

		int Index;

		ArrayIteration(Array<_Kind_> * array):Index(0),Iteration<_Kind_>(array) 
		{
		}
		ArrayIteration():Index(0),Iteration<_Kind_>(0) 
		{
		}
		~ArrayIteration()
		{
		}

		Iteration<_Kind_> * Clone()
		{
			ArrayIteration * iteration = new ArrayIteration();
			*iteration = *this;
			return iteration;
		}

		virtual void Swap(Iterand<_Kind_> & left, Iterand<_Kind_> & right)
		{

			OutputAssert(this == right.Iteration());

			left.Iteration = right.Iteration;
		}

		void Release(Iterand<_Kind_> & iterand)
		{
			if (this->Base)
			{
				this->Base->Dispose(iterand());
			}
			else
			{

				Structure::Disposer<_Kind_>::Release(iterand());
			}
		}

		void Destroy(Iterand<_Kind_> & iterand)
		{
			if (this->Base)
			{
				this->Base->Dispose(iterand());
			}
			else
			{

				Structure::Disposer<_Kind_>::Destroy(iterand());
			}
		}

		void Forward(Iterand<_Kind_> & iterand)
		{
			Array<_Kind_> * array = (Array<_Kind_> *)this->Iterable();

			int index = 0;
			iterand.Forward();

			if (array->Size > 0)
				iterand.Kind(&array->Data[index]);
		}

		void Reverse(Iterand<_Kind_> & iterand)
		{
			Array<_Kind_> * array = (Array<_Kind_> *)this->Iterable();

			int index = array->Size-1;
			iterand.Reverse();

			if (array->Size > 0)
				iterand.Kind(&array->Data[index]);
		}

		void Move(Iterand<_Kind_> & iterand)
		{
			Move(iterand,1);
		}

		void Move(Iterand<_Kind_> & iterand, int amount)
		{
			if (!Iterable)
			{

				iterand.Option |= Iterand<_Kind_>::STATE_INVALID;
				return;
			}

			amount *= iterand.Move();

			Array<_Kind_> * array = (Array<_Kind_> *)this->Iterable();

			int index = 0;
			if (iterand && array->Size > 0)
				index = (_Kind_*)&iterand - &array->Data[0];

			index += amount;
			if (index >= array->Size || index < 0)
			{
				iterand = Structure::Iterand<_Kind_>();
				return;
			}

			if (amount != 0)
				iterand.Kind(&array->Data[index]);

		}

	};

	Structure::Iterator<_Kind_> Iterate()
	{
		return Structure::Iterator<_Kind_>(new ArrayIteration(this));
	}

};

template<typename _Kind_, typename _Policy_>
Identity Array<_Kind_,_Policy_>::Instance;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 
class OrderedArray : public Abstract::Array<_Kind_,_Policy_>, public Orderable
{
public:

	OrderedArray()
	{

		if (this->Initial->Base->Order.Option == Orderable::ORDER_DEFAULT)
		{
			this->Initial->Base->Order.Option = Orderable::ORDER_ASCENDING;
		}
	}

	OrderedArray(const OrderedArray<_Kind_> & array, bool consume=false):
		Abstract::Array<_Kind_>(array)
	{
	}

	OrderedArray(const Abstract::Array<_Kind_> & array, bool consume=false):
		Abstract::Array<_Kind_>(array)
	{
	}

	OrderedArray(const Iterable<_Kind_> & iterable):
		Abstract::Array<_Kind_>(iterable)
	{
	}

	OrderedArray(int size):
		Abstract::Array<_Kind_>(size)
	{
	}

	OrderedArray(int size, typename Template<_Kind_>::ConstantReference type):
		Abstract::Array<_Kind_>(size,type)
	{
	}

	~OrderedArray()
	{
	}

	Iterand<_Kind_> Insert(Iterand<_Kind_> & kind, bool unique=false)
	{
		if (!kind) return Iterand<_Kind_>::Null();

		return Insert(kind(),unique);
	}

	Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference type, bool unique=false)
	{
		if (this->Initial->Base->Order.Option == ORDER_DEFAULT)
		{

			if (this->Size == this->Allocated)
			{
				this->Allocate(0);
			}

			Iterand<_Kind_> iterand;

			*(this->Data+this->Size)=(typename Template<_Kind_>::Reference)type;
			iterand.Kind(this->Data+this->Size);
			++this->Size;
			return iterand;
		}
		else
		{

			int first		=0;
			int last		=this->Size-1; 
			int direction	=0;

			int pivot = 0;

			while (first <= last)
			{
				pivot = (int)(((long long)first+(long long)last)>>1);
				if (pivot < 0 || pivot >= this->Size) break;

				if ((direction = this->Initial->Base->Compare(this->Data[pivot],type,this->Initial->Base->Compare.Option)) == 0 && unique == true)
				{
					return Iterand<_Kind_>();
				}

				switch (this->Initial->Base->Order.Option)
				{
				case Orderable::ORDER_ASCENDING:direction=(direction>0)?-1:1;break;
				case Orderable::ORDER_DESCENDING:direction=(direction>0)?1:-1;break;
				}

				switch(direction)
				{
				case  1:
					first = pivot+1;
					if (first > last)
						++pivot;
					break;
				case -1:
					last  = pivot-1;
					break;
				}

			}

			if (pivot < 0 || pivot >= this->Size)
			{

				return Abstract::Array<_Kind_,_Policy_>::Insert(type,this->Size);
			}
			else
			{

				return Abstract::Array<_Kind_,_Policy_>::Insert(type,pivot);
			}
		}
	}

	using Abstract::Array<_Kind_>::Select;

	Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference type)
	{
		return Select(type,this->Initial->Base->Compare.Option);
	}

	Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference type, int comparitor)
	{
		if (this->Initial->Base->Order() == ORDER_DEFAULT || comparitor != this->Initial->Base->Compare.Option)
		{
			for (int index=0;index<this->Size;++index)
			{
				if (this->Initial->Base->Compare(this->Data[index],type,this->Initial->Base->Compare.Option) == 0)
				{
					return Iterand<_Kind_>(&this->Data[index]);
				}

			}
		}
		else
		{

			int first		=0;
			int last		=this->Size-1; 
			int direction	=0;

			int pivot = 0;

			while (first <= last)
			{

				pivot = (int)(((long long)first+(long long)last)>>1);
				if (pivot < 0 || pivot >= this->Size) break;

				if ((direction = this->Initial->Base->Compare(this->Data[pivot],type,comparitor)) == 0)
				{
					return Iterand<_Kind_>(&this->Data[pivot]);
				}

				switch (this->Initial->Base->Order.Option)
				{
				case ORDER_ASCENDING:direction=(direction>0)?-1:1;break;
				case ORDER_DESCENDING:direction=(direction>0)?1:-1;break;
				}

				switch(direction)
				{
				case  1:first = pivot+1;break;
				case -1:last  = pivot-1;break;
				}

			}
		}

		return Iterand<_Kind_>();
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 
class UnorderedArray : public Abstract::Array<_Kind_,_Policy_>
{
public:

	UnorderedArray(const UnorderedArray<_Kind_,_Policy_> & array, bool consume=false):
		Abstract::Array<_Kind_,_Policy_>(array,consume)
	{
	}

	UnorderedArray(const Abstract::Array<_Kind_,_Policy_> & array, bool consume=false):
		Abstract::Array<_Kind_,_Policy_>(array,consume)
	{
	}

	UnorderedArray(const Iterable<_Kind_> & iterable):
		Abstract::Array<_Kind_,_Policy_>(iterable)
	{
	}

	UnorderedArray(int size):
		Abstract::Array<_Kind_,_Policy_>(size)
	{
	}

	UnorderedArray(int size, typename Template<_Kind_>::ConstantReference type):
		Abstract::Array<_Kind_,_Policy_>(size,type)
	{
	}

	UnorderedArray()
	{
	}

	~UnorderedArray()
	{
	}

	using Abstract::Array<_Kind_,_Policy_>::operator =;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 
class Array : public Abstract::UnorderedArray<_Kind_,_Policy_>
{
public:

	Array(const Array & array, bool consume=false):
		Abstract::UnorderedArray<_Kind_,_Policy_>(array,consume)
	{

	}

	Array(const Iterable<_Kind_> & iterable):
		Abstract::UnorderedArray<_Kind_,_Policy_>(iterable)
	{
	}

	Array(int size):
		Abstract::UnorderedArray<_Kind_,_Policy_>(size)
	{
	}

	Array(int size, typename Template<_Kind_>::ConstantReference type):
		Abstract::UnorderedArray<_Kind_,_Policy_>(size,type)
	{
	}

	Array()
	{
	}

	~Array()
	{
	}

	using Abstract::UnorderedArray<_Kind_,_Policy_>::operator =;

	Array & operator = (Iterable<_Kind_> & iterable)
	{
		return (Array&)Abstract::UnorderedArray<_Kind_>::operator = (iterable);
	}

	Array & operator = (const Array & array)
	{
		return (Array&)Abstract::UnorderedArray<_Kind_>::operator = (array);
	}

	Array & operator = (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;}
	Array & operator , (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;}

	Array & operator << (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;};
	Array & operator >> (typename Template<_Kind_>::ConstantReference type) {Prepend(type);return *this;};

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_> 
class Buffer : public Array<_Kind_>
{
public:

	int Offset;

};

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

