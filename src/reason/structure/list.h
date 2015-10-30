
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
#ifndef STRUCTURE_LIST_H
#define STRUCTURE_LIST_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/generic/generic.h"
#include "reason/structure/iterator.h"
#include "reason/system/interface.h"
#include "reason/structure/disposable.h"
#include "reason/structure/array.h"
#include "reason/structure/policy.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Structure { namespace Abstract {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_, typename _Policy_=Default<_Kind_> >

class List : public Iterable<_Kind_>, public Reason::System::Placeable, public Reason::System::Comparable
{
public:

	List():Iterable<_Kind_>(new _Policy_()) {}

	virtual Iterand<_Kind_> Append(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Prepend(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind, int index)=0;
	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Delete(typename Template<_Kind_>::ConstantReference kind)=0;

	virtual bool Contains(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual int IndexOf(typename Template<_Kind_>::ConstantReference kind)=0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_, typename _Policy_=Default<_Kind_> >

class LinkedList: public Iterable<_Kind_>, public Reason::System::Placeable, public Reason::System::Comparable
{
public:

	static Identity Instance;

	Identity & Identify() {return Instance;}

public:

	class Entry : public Variable<_Kind_>
	{
	public:

		virtual typename Type<_Kind_>::Pointer Pointer() {return Variable<_Kind_>::Pointer();}
		virtual typename Type<_Kind_>::Reference Reference() {return Variable<_Kind_>::Reference();}
		virtual typename Template<_Kind_>::Reference operator () (void) {return Variable<_Kind_>::operator ()();}

	public:

		Entry * Next;
		Entry * Prev;

		int	Key;	

		Entry(Entry * entry):Key(entry->Key),Next(0),Prev(0)
		{
			if (entry)
				this->Kind = entry->Kind;

		}

		Entry(int key=0):Key(key),Next(0),Prev(0)
		{
		}

		Entry(typename Template<_Kind_>::ConstantReference type, int key):Variable<_Kind_>(type),Key(key),Next(0),Prev(0)
		{
		}

		~Entry()
		{
		}

		void Release()
		{
			Next = Prev = 0;
			Key = 0;
			Reason::Structure::Disposer<_Kind_>::Release((*this)());
		}

		void Destroy()
		{
			Next = Prev = 0;
			Key = 0;
			Reason::Structure::Disposer<_Kind_>::Destroy((*this)());
		}

		int Compare(typename Template<_Kind_>::ConstantReference type, int comparitor = COMPARE_GENERAL)
		{
			return Reason::Structure::Comparer<_Kind_>::Compare(this->Kind,type,comparitor);
		}

		int Compare(Iterand<_Kind_> iterand, int comparitor = COMPARE_GENERAL)
		{
			return Reason::Structure::Comparer<_Kind_>::Compare(this->Kind,iterand(),comparitor);
		}

		int Compare(Entry * entry, int comparitor = COMPARE_GENERAL)
		{
			return Reason::Structure::Comparer<_Kind_>::Compare(this->Kind,(*entry)(),comparitor);
		}

	};

	class EntryIteration : public Iteration<_Kind_>
	{
	public:

		Entry * First;
		Entry * Last;

		EntryIteration(LinkedList<_Kind_,_Policy_> * list):Iteration<_Kind_>(list),First(list->First),Last(list->Last)
		{
		}

		EntryIteration():First(0),Last(0)
		{
		}

		int Index(Iterand<_Kind_> & iterand) 
		{

			return ((LinkedList<_Kind_,_Policy_> *)this->Iterable())->IndexOf(iterand);
		}

		void Release(Iterand<_Kind_> & iterand)
		{
			Entry * entry = (Entry*)&iterand;

			if (this->Iterable)
			{
				this->Iterable->Initial->Base->Dispose(iterand());
			}
			else
			{

				Structure::Disposer<_Kind_>::Release(iterand());
			}

			delete entry;
		}

		void Destroy(Iterand<_Kind_> & iterand)
		{
			Entry * entry = (Entry*)&iterand;

			if (this->Iterable)
			{
				this->Iterable->Initial->Base->Dispose(iterand());
			}
			else
			{

				Structure::Disposer<_Kind_>::Destroy(iterand());
			}

			delete entry;
		}

		Iteration<_Kind_> * Clone()
		{

			return this;
		}

		void Forward(Iterand<_Kind_> & iterand)
		{
			iterand.Variant((First)?First:((LinkedList<_Kind_,_Policy_> *)this->Iterable())->First);
			iterand.Forward();
		}

		void Reverse(Iterand<_Kind_> & iterand)
		{
			iterand.Variant((Last)?Last:((LinkedList<_Kind_,_Policy_> *)this->Iterable())->Last);
			iterand.Reverse();
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

			typename LinkedList<_Kind_,_Policy_>::Entry * entry = (typename LinkedList<_Kind_,_Policy_>::Entry *)&iterand;
			if (entry)
			{
				if (amount > 0)
				{
					while (entry != 0 && amount-- > 0)
						entry = (entry == Last)?0:entry->Next;
				}
				else
				if (amount < 0)
				{
					while (entry != 0 && amount++ < 0)
						entry = (entry == First)?0:entry->Prev;
				}			
			}

			iterand.Variant(entry);
		}

	};

	virtual Entry * New(typename Template<_Kind_>::ConstantReference kind, int key)
	{
		void * data = Iterable<_Kind_>::New(sizeof(Entry));
		return new (data) Entry(kind,key);
	}

	virtual Entry * New(typename Template<_Kind_>::ConstantReference kind)
	{
		void * data = Iterable<_Kind_>::New(sizeof(Entry));
		return new (data) Entry(kind,0);
	}

	using Iterable<_Kind_>::Del;
	virtual void Del(Entry * entry)
	{
		entry->~Entry();
		Iterable<_Kind_>::Del((void*)entry);		
	}

	using Iterable<_Kind_>::Compare;
	virtual int Compare(Iterand<_Kind_> & left, Iterand<_Kind_> & right, int compare=COMPARE_POLICY)
	{
		return Iterable<_Kind_>::Compare(left,right,compare);
	}

	using Iterable<_Kind_>::Dispose;
	virtual void Dispose(Iterand<_Kind_> & iterand)
	{
		Entry * entry = (Entry*) &iterand;
		Dispose(entry);
	}

	virtual void Dispose(Entry * entry)
	{
		 Initial->Base->Dispose((*entry)());
		 Del(entry);
	}

	virtual void Release(Iterand<_Kind_> & kind)
	{
		if (kind.Proxy == 0 && kind.Iteration && kind.Iteration->Iterable() == this)
		{
			Reason::Structure::Disposer<_Kind_>::Release(kind());
			Entry * entry = (Entry*) &kind;
			Del(entry);
		}
	}

	virtual void Destroy(Iterand<_Kind_> & kind)
	{
		if (kind.Proxy == 0 && kind.Iteration && kind.Iteration->Iterable() == this)
		{
			Reason::Structure::Disposer<_Kind_>::Destroy(kind());
			Entry * entry = (Entry*) &kind;
			Del(entry);
		}
	}	

public:

	int Count;
	Entry * First;
	Entry * Last;

	LinkedList(const Iterable<_Kind_> & kind):
		Iterable<_Kind_>(new _Policy_()),Count(0),First(0),Last(0)
	{
		operator = (kind);
	}

	LinkedList(const LinkedList & list, bool consume=false):
		Iterable<_Kind_>(new _Policy_()),Count(0),First(0),Last(0)
	{
		if (consume)
			Consume(list);
		else
			operator = (list);
	}

	LinkedList():
		Iterable<_Kind_>(new _Policy_()),Count(0),First(0),Last(0)
	{

	}

	~LinkedList()
	{
		Entry *entry = First;
		while (entry != 0)
		{
			entry = entry->Next;

			delete First;
			First = entry;
		}
	}

	LinkedList & operator = (const Iterable<_Kind_> & iterable)
	{			
		Iterand<_Kind_> iterand = iterable.Forward();
		while (iterand != 0)
		{
			Insert(iterand());
			++iterand;
		}
	}

	LinkedList & operator = (const LinkedList & list)
	{	
		if (&list == this) return *this;

		if (Count == 0)
		{

			Entry * from = list.First;		

			if (from)
			{
				Count = list.Count;
				First = New(from,0);

				Entry * to = First;
				while(from->Next)
				{
					to->Next = New(from->Next,0);
					to->Next->Prev = to;

					to = to->Next;
					from = from->Next;			
				}

				Last = to;
			}

			return *this;
		}
		else
		{
			Release();
			Entry * entry = list.First;
			while(entry)
			{
				Insert(entry());
				entry = entry->Next;
			}
		}
	}

	bool IsEmpty() {return Count == 0;};
	int Length() {return Count;};

	typename Template<_Kind_>::Reference operator [] (int index)	{return (IterandAt(index))();};

	typename Type<_Kind_>::Pointer PointerAt(int index)
	{
		Iterand<_Kind_> result = IterandAt(index);
		return (result)?result.Pointer():0;
	}

	typename Type<_Kind_>::Reference ReferenceAt(int index)
	{
		Iterand<_Kind_> result = IterandAt(index);
		return (result)?result.Reference():0;
	}

	virtual Iterand<_Kind_> IterandAt(int index)
	{
		Entry * entry = First;
		while (entry && index-- > 0)
			entry = entry->Next;

		Iterand<_Kind_> result(this->Initial,entry);
		return result;
	}

	int IndexOf(typename Template<_Kind_>::ConstantReference type, int comparitor = COMPARE_GENERAL)
	{
		Entry * entry = First;
		for (int i=0;i<Count;++i)
		{
			if (this->Initial->Base->Compare((*entry)(),type,comparitor)==0)
				return i;

			entry = entry->Next;
		}
		return -1;
	}

	int IndexOf(Iterand<_Kind_> & kind)
	{
		if (!this->Get(kind)) return -1;

		Entry * entry = First;
		for (int i=0;i<Count;++i)
		{
			if (entry == (Entry *)&kind)
				return i;

			entry = entry->Next;
		}
		return -1;
	}

	virtual Iterand<_Kind_> Append(typename Template<_Kind_>::ConstantReference kind)
	{
		Entry * entry = New(kind,0);
		return Append(Iterand<_Kind_>::Put(entry));
	}

	virtual Iterand<_Kind_> Append(Iterand<_Kind_> & kind)
	{	
		if (!kind) return Iterand<_Kind_>::Null();

		Entry * entry = (Entry*) this->Put(kind);
		if (!entry)
		{
			return Append(kind());
		}
		else
		{
			if (First == 0)
			{
				First = Last = entry;
			}
			else
			{
				Last->Next = entry;
				entry->Prev = Last;
				Last = entry;
			}
			++Count;	

			Iterand<_Kind_> result(this->Initial,entry);
			return result;			
		}
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind)
	{
		return Append(kind);
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & kind)
	{
		return Append(kind);	
	}

	virtual Iterand<_Kind_> Prepend(typename Template<_Kind_>::ConstantReference kind)
	{
		Entry * entry = New(kind,0);
		return Prepend(Iterand<_Kind_>::Put(entry));
	}

	virtual Iterand<_Kind_> Prepend(Iterand<_Kind_> & kind)
	{			
		if (!kind) return Iterand<_Kind_>::Null();

		Entry * entry = (Entry*) this->Put(kind);
		if (!entry)
		{
			return Prepend(kind());
		}
		else
		{		
			if (First == 0)
			{
				First = Last = entry;
			}
			else
			{
				First->Prev = entry;
				entry->Next = First;
				First = entry;
			}
			++Count;		

			Iterand<_Kind_> result(this->Initial,entry);
			return result;				
		}

	}

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference type, int compare = COMPARE_POLICY)
	{
		if (compare == COMPARE_POLICY)
			compare = this->Initial->Base->Compare.Option;

		Entry * entry = First;
		while (entry != 0)
		{
			if (this->Compare((*entry)(),type,compare) == 0)
				break;

			entry = entry->Next;
		}

		Iterand<_Kind_> result(this->Initial,entry);
		return result;
	}

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> & kind)
	{
		if (!kind) return Iterand<_Kind_>::Null();

		Entry * entry = (Entry*) this->Get(kind);
		if (!entry)
		{
			return Select(kind());
		}
		else
		{			
			Entry * select = First;
			while(select != 0)
			{
				if (select == entry)
					break;

				select = select->Next;
			}

			Iterand<_Kind_> result(this->Initial,entry);
			return result;
		}
	}	

	virtual Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference kind)
	{	

		Iterand<_Kind_> select = Select(kind, this->Initial->Base->Compare.Option);
		if (select)
		{
			return Remove(select);
		}

		return Iterand<_Kind_>::Null();
	}

	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> & kind)
	{
		if (!kind) return Iterand<_Kind_>::Null();

		Entry * entry = (Entry*) this->Get(kind);
		if (!entry)
		{
			return Remove(kind());
		}

		if (Count == 1)
		{
			First =0;
			Last =0;
		}
		else
		if (entry == First)
		{
			First->Next->Prev = 0;
			First = First->Next;
		}
		else
		if (entry == Last)
		{
			Last->Prev->Next = 0;
			Last = Last->Prev;
		}
		else
		{
			entry->Next->Prev = entry->Prev;
			entry->Prev->Next = entry->Next;
		}

		entry->Next=0;
		entry->Prev=0;

		--Count;

		Iterand<_Kind_> remove = Iterand<_Kind_>(this->Initial,entry);
		remove.Dispose(Disposable::DISPOSE_POLICY);			
		return remove;

	}

	virtual Iterand<_Kind_> Delete(typename Template<_Kind_>::ConstantReference kind)
	{
		Iterand<_Kind_> select = Select(kind, this->Initial->Base->Compare.Option);
		if (select)
		{

			return Delete(select);
		}

		return Iterand<_Kind_>::Null();
	}

	virtual Iterand<_Kind_> Delete(Iterand<_Kind_> & kind)
	{

		if (!kind) return Iterand<_Kind_>::Null();

		Entry * entry = (Entry*)this->Get(kind);		
		if (!entry)
		{
			return Delete(kind());
		}
		else
		{

			Iterand<_Kind_> remove = Remove(kind);			

			remove.Dispose(Disposable::DISPOSE_DESTROY);
			return remove;
		}
	}

	virtual void Consume(const LinkedList & list)
	{
		if (Count)
		{
			Entry * entry = First;
			while (entry != 0)
			{
				entry = entry->Next;
				Dispose(First);
				First = entry;
			}

			Count = 0;
			First = Last = 0;		
		}

		First = list.First;
		Last = list.Last;
		Count = list.Count;

		((LinkedList&)list).First = 0;
		((LinkedList&)list).Last = 0;
		((LinkedList&)list).Count = 0;

		((LinkedList&)list).Invalid = 0;
	}

	virtual void Release()
	{
		Entry * entry = First;
		while (entry != 0)
		{
			entry = entry->Next;
			Release(Iterand<_Kind_>(First));
			First = entry;
		}

		Count = 0;
		First = Last = 0;
	}

	virtual void Destroy()
	{
		Entry *entry = First;
		while (entry != 0)
		{
			entry = entry->Next;
			Destroy(Iterand<_Kind_>(First));
			First = entry;
		}

		Count = 0;
		First = Last = 0;
	}

	LinkedList & Union(LinkedList & list)
	{
		Entry * entry = list.First;
		while (entry != list.Last)
		{
			if (Select((*entry)())==0)
			{
				Append((*entry)());
			}
			entry = entry->Next;
		}

		return *this;
	}

	LinkedList & Intersection(LinkedList & list)
	{

		Entry *entry = First;
		while (entry != 0)
		{
			if (list.Select((*entry)()) == 0)
			{
				Entry * that = entry;
				entry = entry->Next;
				Dispose(that);
			}
			else
			{
				entry = entry->Next;
			}
		}

		return *this;
	}

	LinkedList & Difference(LinkedList & list)
	{
		Entry *entry = list.First;
		while (entry != 0)
		{
			Entry *that = Select((*entry)());
			if (that != 0)
			{
				Dispose(that);
			}

			entry = entry->Next;
		}

		return *this;
	}

	LinkedList & Rotate(int rotation=-1)
	{

		rotation = rotation % this->Count;

		Entry * entry;

		if (rotation > 0)
		{

			while (rotation > 0)
			{

				entry = this->Last;
				Iterand<_Kind_> remove = Remove(Iterand<_Kind_>::Get(entry));
				Prepend(remove);
				-- rotation;
			}
		}
		else
		if (rotation < 0)
		{

			while (rotation < 0)
			{

				entry = this->First;
				Iterand<_Kind_> remove = Remove(Iterand<_Kind_>::Get(entry));
				Append(remove);
				++rotation;
			}
		}

		return *this;
	}

	virtual LinkedList & Take(Iterand<_Kind_> & kind, LinkedList & from)
	{
		Iterand<_Kind_> remove = from.Remove(kind);
		Insert(remove);
		return *this;
	}

	virtual LinkedList & Take(typename Template<_Kind_>::ConstantReference kind, LinkedList & from)
	{
		Iterand<_Kind_> remove = from.Remove(kind);
		Insert(remove);
		return *this;
	}

	virtual LinkedList & Take(LinkedList<_Kind_,_Policy_> & from)
	{
		if (IsEmpty())
		{
			this->First	= from.First;
			this->Last	= from.Last;
			this->Count	= from.Count;

			from.First	= 0;
			from.Last	= 0;
			from.Count	= 0;

		}
		else
		{

			Entry * entry = from.First;
			while(entry)
			{
				Entry * that = entry;
				entry = entry->Next;

				that->Next = 0;
				that->Prev = 0;
				Insert(Iterand<_Kind_>::Put(that));
			}

			from.First	= 0;
			from.Last	= 0;
			from.Count	= 0;
		}

		return *this;
	}

public:

	Structure::Iterator<_Kind_> Iterate()
	{
		return Structure::Iterator<_Kind_>(new EntryIteration(this));
	}	

};

template<typename _Kind_,typename _Policy_>
Identity LinkedList<_Kind_,_Policy_>::Instance;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 
class UnorderedList : public LinkedList<_Kind_,_Policy_>
{
public:
	typedef typename Structure::Abstract::LinkedList<_Kind_,_Policy_>::Entry Entry;

protected:

	int SentinelIndex;
	Entry * Sentinel;

public:	

	UnorderedList(const Iterable<_Kind_> & iterable):Sentinel(0),SentinelIndex(0)
	{
		LinkedList<_Kind_,_Policy_>::operator = (iterable);
	}

	UnorderedList(const UnorderedList & list):Sentinel(0),SentinelIndex(0)
	{
		LinkedList<_Kind_,_Policy_>::operator = (list);
	}

	UnorderedList():Sentinel(0),SentinelIndex(0)
	{
	}

	~UnorderedList()
	{
	}

	UnorderedList & operator = (const Iterable<_Kind_> & iterable)
	{
		return LinkedList<_Kind_,_Policy_>::operator = (iterable);		
	}

	UnorderedList & operator = (const UnorderedList & list)
	{
		return LinkedList<_Kind_,_Policy_>::operator = (list);
	}

public:

	Iterand<_Kind_> IterandAt(int index)
	{

		if (index > (this->Count-1) || index < 0) return Iterand<_Kind_>::Null();

		if (index == 0)
		{
			Sentinel = this->First;
			SentinelIndex = 0;
		}
		else
		if (index == this->Count-1)
		{
			Sentinel = this->Last;
			SentinelIndex = index;
		}
		else
		{

			if (Sentinel == 0)
			{
				Sentinel = this->First;
				SentinelIndex = 0;
			}

			int dif = index - SentinelIndex;
			if (dif > 0)
			{
				while (SentinelIndex < index)
				{
					Sentinel = Sentinel->Next;
					++SentinelIndex;
				}
			}
			else
			if (dif < 0)
			{
				while (SentinelIndex > index)
				{
					Sentinel = Sentinel->Prev;
					--SentinelIndex;
				}
			}
		}

		Iterand<_Kind_> result(this->Initial,Sentinel);
		return result;
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & iterand, int index)
	{
		if (!iterand) return Iterand<_Kind_>::Null();

		Iterand<_Kind_> result;

		OutputAssert(index >=0 && index <= this->Count);

		if (index == 0)
		{

			result = Insert(iterand,Cast::Reference(Iterand<_Kind_>()),Placeable::PLACE_BEFORE);
		}
		else
		if (index == this->Count)
		{

			result = Insert(iterand,Cast::Reference(Iterand<_Kind_>()),Placeable::PLACE_AFTER);			
		}
		else
		{
			result = Insert(iterand,Cast::Reference(IterandAt(index)),Placeable::PLACE_BEFORE);
		}

		if (index <= SentinelIndex) ++SentinelIndex;

		return result;
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & kind)
	{
		return Insert(kind,Iterand<_Kind_>(),Placeable::PLACE_AFTER);
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind,int index)
	{

		Entry * entry = new Entry(kind,0);
		return Insert(Iterand<_Kind_>::Put(entry),index);
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind)
	{
		Entry * entry = new Entry(kind,0);
		return Insert(Iterand<_Kind_>::Put(entry));
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind, Iterand<_Kind_> & place, int placement = Placeable::PLACE_AFTER)
	{
		Entry * entry = new Entry(kind,0);
		return Insert(Iterand<_Kind_>::Put(entry),place,placement);
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & kind, Iterand<_Kind_> & place, int placement = Placeable::PLACE_AFTER)
	{
		if (!kind) return Iterand<_Kind_>::Null();		

		Sentinel = 0;
		SentinelIndex = 0;

		place = (Entry*) this->Get(place);

		if (place == 0)
		{
			switch (placement)
			{
			case Placeable::PLACE_BEFORE:

				place = (Variant<_Kind_>*)this->First;
			break;
			case Placeable::PLACE_AFTER:

				place = (Variant<_Kind_>*)this->Last;
			break;
			case Placeable::PLACE_RANDOM:

				place = this->IterandAt(rand()%this->Length());
			break;
			}
		}
		else
		{
			if (placement == Placeable::PLACE_RANDOM)
			{
				OutputError("List<>::Insert - Random placement cannot be used in conjunction with a sentinel.\n");
			}
		}

		Entry * entry = (Entry*) this->Put(kind);
		if (!entry) entry = new Entry(kind(),0);

		if (place == 0)
		{

			entry->Next = 0;
			entry->Prev = 0;
			this->First = this->Last = entry;
		}
		else
		{
			if (placement == Placeable::PLACE_AFTER)
			{

				if (((Entry*)&place) == this->Last)
				{
					OutputAssert(entry->Next == 0);
					this->Last = entry;
				}
				else
				{
					entry->Next	= ((Entry*)&place)->Next;
					entry->Next->Prev = entry;
				}

				entry->Prev	= (Entry*)&place;
				entry->Prev->Next = entry;

			}
			else
			{

				if (((Entry*)&place) == this->First)
				{
					OutputAssert(entry->Prev == 0);
					this->First = entry;
				}
				else
				{
					entry->Prev	= ((Entry*)&place)->Prev;
					entry->Prev->Next = entry;
				}

				entry->Next	= (Entry*)&place;
				entry->Next->Prev = entry;
			}
		}

		++this->Count;

		Iterand<_Kind_> result(this->Initial, entry);
		return result;
	}

	using LinkedList<_Kind_,_Policy_>::Remove;
	Iterand<_Kind_> Remove(Iterand<_Kind_> & iterand)
	{
		Sentinel = 0;
		SentinelIndex = 0;
		return LinkedList<_Kind_,_Policy_>::Remove(iterand);		
	}

	Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference kind)
	{
		Sentinel = 0;
		SentinelIndex = 0;
		return LinkedList<_Kind_,_Policy_>::Remove(kind);
	}

	Iterand<_Kind_> RemoveAt(int index)
	{
		Iterand<_Kind_> result = IterandAt(index);
		if (result) 
		{
			return Remove(result);
		}

		return Iterand<_Kind_>::Null();
	}

	using LinkedList<_Kind_,_Policy_>::Select;

	Iterand<_Kind_> DeleteAt(int index)
	{

		return Delete(Cast::Reference(IterandAt(index)));
	}

	virtual void Release()
	{
		SentinelIndex	=0;
		Sentinel		=0;
		LinkedList<_Kind_,_Policy_>::Release();
	}

	virtual void Destroy()
	{
		SentinelIndex	=0;
		Sentinel		=0;
		LinkedList<_Kind_,_Policy_>::Destroy();
	}

	UnorderedList & SwapAt(int i,int j)
	{
		return Swap(IterandAt(i),IterandAt(j));
	}

	UnorderedList & Swap(typename Template<_Kind_>::ConstantReference i, typename Template<_Kind_>::ConstantReference j)
	{
		return Swap(Select(i),Select(j));
	}

	UnorderedList & Swap(Iterand<_Kind_> & i, Iterand<_Kind_> & j)
	{
		if (i != j)
		{

			Entry * ii = ((Entry*)&i)->Prev;
			Entry * jj = ((Entry*)&j)->Prev;

			if (ii == (Entry*)&j)
			{
				Remove(i);
				Insert(i,j,Placeable::PLACE_BEFORE);
			}
			else
			if (jj == (Entry*)&i)
			{
				Remove(j);
				Insert(j,i,Placeable::PLACE_BEFORE);
			}
			else
			{

				Remove(j);
				Insert(j,Iterand<_Kind_>(ii),(ii)?Placeable::PLACE_AFTER:Placeable::PLACE_BEFORE);
				Remove(i);
				Insert(i,Iterand<_Kind_>(jj),(jj)?Placeable::PLACE_AFTER:Placeable::PLACE_BEFORE);
			}
		}

		return *this;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 
class OrderedList : public UnorderedList<_Kind_,_Policy_>, public Reason::System::Orderable
{
public:
	typedef typename UnorderedList<_Kind_,_Policy_>::Entry Entry;

	OrderedList(const Iterable<_Kind_> & iterable, int compare=Reason::System::Comparable::COMPARE_GENERAL)

	{
		this->Initial->Base->Compare.Option = compare;
		UnorderedList<_Kind_,_Policy_>::operator = (iterable);
	}

	OrderedList(const OrderedList & list)

	{
		this->Initial->Base->Compare.Option = list.Compare.Option;
		UnorderedList<_Kind_,_Policy_>::operator = (list);
	}

	OrderedList(int order, int compare=Reason::System::Comparable::COMPARE_GENERAL)

	{

		this->Initial->Base->Order.Option = order;
		this->Initial->Base->Compare.Option = compare;
	}

	OrderedList()

	{

	}

	~OrderedList()
	{

	}	

	OrderedList & operator = (const Iterable<_Kind_> & iterable)
	{

		return UnorderedList<_Kind_,_Policy_>::operator = (iterable);
	}

	OrderedList & operator = (const OrderedList & list)
	{
		this->Initial->Base->Order.Option = list.Order.Option;
		this->Initial->Base->Compare.Option = list.Compare.Option;

		return UnorderedList<_Kind_,_Policy_>::operator = (list);
	}

	using UnorderedList<_Kind_,_Policy_>::Insert;
	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & kind)
	{
		if (!kind) return Iterand<_Kind_>::Null();

		this->Sentinel = 0;
		this->SentinelIndex = 0;

		if (this->Initial->Base->Order() == ORDER_DEFAULT)
		{
			return UnorderedList<_Kind_,_Policy_>::Insert(kind);
		}

		int first		=0;
		int last		=this->Count-1;
		int middle		=0;		
		int offset		=0;		
		int direction	=0;		

		Entry * entry = (Entry*) this->Put(kind);

		if (!entry) entry = new Entry(kind(),0);

		Entry * pivot = this->First;

		while (first <= last)
		{

			offset = middle;
			middle = (int)(((long long)first+(long long)last)>>1);
			offset = abs(middle-offset);

			if (direction < 0)
			{
				for(int i=offset;i>0;--i)
					pivot = pivot->Prev;
			}
			else
			{
				for(int i=offset;i>0;--i)
					pivot = pivot->Next;
			}

			if ( pivot == 0 ) break;

			if (this->Initial->Base->Order() == Reason::System::Orderable::ORDER_ASCENDING)
			{
				direction = this->Initial->Base->Compare(pivot,entry)>0?-1:1;

				if ( direction > 0 )
				{
					if ( pivot->Next == 0 || this->Initial->Base->Compare(pivot->Next,entry) > 0 )

					{
						pivot = pivot->Next;
						break;
					}
					else
					{
						first = middle+1;
					}
				}
				else
				{
					if ( pivot->Prev == 0 || this->Initial->Base->Compare(pivot->Prev,entry) < 0 )

					{
						break;
					}
					else
					{
						last = middle-1;
					}
				}
			}
			else
			if (this->Initial->Base->Order() == ORDER_DESCENDING)
			{
				direction = this->Initial->Base->Compare(pivot,entry)>0?1:-1;			

				if ( direction > 0 )
				{
					if ( pivot->Next == 0 || this->Initial->Base->Compare(pivot->Next,entry) < 0 )

					{
						pivot = pivot->Next;
						break;
					}
					else
					{
						first = middle+1;
					}
				}
				else
				{
					if ( pivot->Prev == 0 || this->Initial->Base->Compare(pivot->Prev,entry) > 0 )

					{
						break;
					}
					else
					{
						last = middle-1;
					}
				}
			}
			else
			{
				OutputError("List:Insert - Insert failed, an invalid order was specified.\n");
				return Iterand<_Kind_>::Null();
			}

			if (first > last)
				pivot = 0;
		}

		if (pivot == 0)
		{

			return Insert(Cast::Reference(Iterand<_Kind_>::Put(entry)),Cast::Reference(Iterand<_Kind_>()),Placeable::PLACE_AFTER);
		}
		else
		{

			return Insert(Cast::Reference(Iterand<_Kind_>::Put(entry)),Cast::Reference(Iterand<_Kind_>::Get(pivot)),Placeable::PLACE_BEFORE);
		}
	}

	using UnorderedList<_Kind_,_Policy_>::Select;
	Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference type, int compare = Reason::System::Comparable::COMPARE_GENERAL)
	{
		if (this->Initial->Base->Order() == ORDER_DEFAULT || compare != this->Initial->Base->Compare.Option)
		{
			return UnorderedList<_Kind_,_Policy_>::Select(type,compare);
		}
		else
		{

			int first		=0;
			int last		=this->Count-1;
			int middle		=0;		
			int offset		=0;		
			int direction	=0;		

			Entry * pivot = this->First;

			while (first <= last)
			{

				offset = middle;
				middle = (int)(((long long)first+(long long)last)>>1);
				offset = abs(middle-offset);

				if (direction < 0)
				{
					for(int i=offset;i>0;--i)
						pivot = pivot->Prev;
				}
				else
				{
					for(int i=offset;i>0;--i)
						pivot = pivot->Next;
				}

				if (pivot == 0 || (direction = this->Initial->Base->Compare((*pivot)(),type)) == 0)

				{
					Iterand<_Kind_> result(this->Initial,pivot);
					return result;
				}
				else
				{

					switch (this->Initial->Base->Order())
					{
					case ORDER_ASCENDING:direction=(direction>0)?-1:1;break;
					case ORDER_DESCENDING:direction=(direction>0)?1:-1;break;
					}					

					switch(direction)
					{
					case  1:first = middle+1;break;
					case -1:last  = middle-1;break;
					}
				}
			}

			return Iterand<_Kind_>::Null();
		}
	}

	using UnorderedList<_Kind_,_Policy_>::Take;
	virtual OrderedList & Take(OrderedList & from)
	{
		bool sort = false;
		if (!IsEmpty())
			sort = true;

		UnorderedList<_Kind_,_Policy_>::Take(from);

		if (this->Initial->Base->Order() != ORDER_DEFAULT && sort)
			return Sort();
		else
			return *this;
	}

	OrderedList & Shuffle(int seed=1)
	{

		if (this->Count > 2)
		{

			Entry * entry = this->Last;
			Entry * sentinel;

			int remaining = this->Count-1;

			srand(seed);

			while (remaining > 0 && (sentinel = entry->Prev) != 0)
			{
				Swap(Iterand<_Kind_>::Put(entry),this->IterandAt(rand()%remaining));
				entry = sentinel;
				-- remaining;
			}
		}

		return *this;
	}

	typedef void (*SortFunction) (LinkedList<_Kind_,_Policy_> & list);

	OrderedList & Sort(int order, bool stable=false) {SortTree(order);return *this;}
	OrderedList & Sort(bool stable=false) {SortTree(this->Initial->Base->Order());return *this;}
	OrderedList & Sort(SortFunction sort) {(*sort)(*this);return *this;}

	void SortTree(int order)
	{

		if (order == ORDER_DEFAULT)
		{
			order = ORDER_ASCENDING;
		}
		else
		if (order != ORDER_ASCENDING && order != ORDER_DESCENDING)
		{
			OutputError("List:Sort - Sort failed, an invalid order was specified.\n");
			return;
		}

		if (!this->First || !this->Last)
			return;

		Entry * root = 0;
		Entry * node = 0;

		Entry * min = 0;
		Entry * max = 0;

		Entry * entry = 0;

		root = this->First;
		this->First = this->First->Next;
		root->Prev = 0;
		root->Next = 0;

		min = max = root;

		while (this->First)
		{
			entry = this->First;
			this->First = this->First->Next;
			entry->Prev = entry->Next = 0;

			node = root;

			if (min)
			{
				int compare = this->Initial->Base->Compare(entry,min);

				if (compare == 0)
				{
					node = min;
				}
				else
				if (compare < 0)
				{
					min->Prev = entry;
					min = entry;
					continue;
				}
			}

			if (max)
			{
				int compare = this->Initial->Base->Compare(entry,max);

				if (compare == 0)
				{
					node = max;
				}
				else
				if (compare > 0)
				{
					max->Next = entry;
					max = entry;
					continue;
				}
			}

			while (node)
			{

				int compare = this->Initial->Base->Compare(entry,node);	

				if (compare == 0)
				{
					if (order == ORDER_ASCENDING)
					{

						if (node->Next == 0)
						{
							node->Next = entry;
							if (node == max)
								max = entry;
							break;
						}

						node = node->Next;					
					}
					else
					{

						if (node->Prev == 0)
						{
							node->Prev = entry;
							if (node == min)
								min = entry;
							break;
						}

						node = node->Prev;
					}
				}
				else
				if (compare < 0)
				{

					if (node->Prev == 0)
					{
						node->Prev = entry;
						if (node == min)
							min = entry;
						break;
					}

					node = node->Prev;
				}
				else
				{

					if (node->Next == 0)
					{
						node->Next = entry;
						if (node == max)
							max = entry;
						break;
					}

					node = node->Next;
				}
			}
		}

		this->First = this->Last = 0;

		Reason::Structure::Array<Entry *> stack;

		stack.Allocate(8);

		node = root;

		if (order == ORDER_ASCENDING)
		{

			while (node->Prev) 
			{
				stack.Append(node);
				node = node->Prev;
			}

			while (node)
			{
				entry = node;

				if (node->Next)
				{
					node = node->Next;
					while (node->Prev) 
					{
						stack.Append(node);
						node = node->Prev;				
					}
				}
				else
				{
					if (stack.Size > 0)
						node = stack.RemoveAt(stack.Size-1)();
					else
						node = 0;
				}

				if (!this->First)
				{
					this->First = this->Last = entry;
					entry->Prev = 0;
					entry->Next = 0;
				}
				else
				{			
					entry->Prev = this->Last;
					entry->Next = 0;
					this->Last->Next = entry;
					this->Last = entry;
				}
			}

		}
		else
		{

			while (node->Next) 
			{
				stack.Append(node);
				node = node->Next;
			}

			while (node)
			{
				entry = node;

				if (node->Prev)
				{
					node = node->Prev;
					while (node->Next) 
					{
						stack.Append(node);
						node = node->Next;				
					}
				}
				else
				{
					if (stack.Size > 0)
						node = stack.RemoveAt(stack.Size-1)();
					else
						node = 0;
				}

				if (!this->First)
				{
					this->First = this->Last = entry;
					entry->Prev = 0;
					entry->Next = 0;
				}
				else
				{			
					entry->Prev = this->Last;
					entry->Next = 0;
					this->Last->Next = entry;
					this->Last = entry;
				}
			}
		}
	}

	void SortQuickStable(Entry ** first, Entry ** last, int order)
	{

		if (order == ORDER_DEFAULT)
		{
			order = ORDER_ASCENDING;
		}
		else
		if (order != ORDER_ASCENDING && order != ORDER_DESCENDING)
		{
			OutputError("List:Sort - Sort failed, an invalid order was specified.\n");
			return;
		}

		Entry * lhsFirst=0, * lhsLast=0, *rhsFirst=0, * rhsLast=0;
		int side=0;

		while (*first != 0)
		{			
			Entry * leFirst=0, * leLast=0;
			Entry * eqFirst=0, * eqLast=0;
			Entry * grFirst=0, * grLast=0;
			int leSize=0,grSize=0;

			Entry * pivot = (Entry*)*first;
			Entry * entry = 0;

			while (*first != 0) 
			{
				entry = (Entry*)*first;
				*first = entry->Next;
				entry->Next = entry->Prev = 0;

				int compare = this->Initial->Base->Compare(entry,pivot);

				if (order == ORDER_DESCENDING) compare *= -1;

				if (compare < 0)
				{
					if (leFirst == 0)
					{
						leFirst = leLast = entry;
					}
					else
					{
						leLast->Next = entry;
						entry->Prev = leLast;
						leLast = entry;
					}

					++ leSize;
				}
				else 
				if (compare > 0)
				{
					if (grFirst == 0)
					{
						grFirst = grLast = entry;
					}
					else
					{
						grLast->Next = entry;
						entry->Prev = grLast;
						grLast = entry;
					}

					++ grSize;
				}
				else
				{
					if (eqFirst == 0)
					{
						eqFirst = eqLast = entry;
					}
					else
					{
						eqLast->Next = entry;
						entry->Prev = eqLast;
						eqLast = entry;
					}
				}
			}

			Entry * opFirst=0, * opLast=0;
			if (leSize <= grSize)
			{
				if (leFirst)
				{
					SortQuickStable(&leFirst, &leLast,order);

					leLast->Next = eqFirst;
					eqFirst->Prev = leLast;

					opFirst = leFirst;
					opLast = eqLast;
				}
				else
				{
					opFirst = eqFirst;
					opLast = eqLast;
				}

				*first = grFirst;
				*last = grLast;		

				if (lhsFirst == 0)
				{
					lhsFirst = opFirst;
					lhsLast = opLast;
				}
				else
				{
					lhsLast->Next = opFirst;
					opFirst->Prev = lhsLast;
					lhsLast = opLast;
				}

			}
			else
			{
				if (grFirst)
				{
					SortQuickStable(&grFirst, &grLast,order);

					grFirst->Prev = eqLast;
					eqLast->Next = grFirst;

					opFirst = eqFirst;
					opLast = grLast;
				}
				else
				{
					opFirst = eqFirst;
					opLast = eqLast;
				}

				*first = leFirst;
				*last = leLast;						

				if (rhsFirst == 0)
				{
					rhsFirst = opFirst;
					rhsLast = opLast;
				}
				else
				{
					rhsFirst->Prev = opLast;
					opLast->Next = rhsFirst;
					rhsFirst = opFirst;
				}
			}
		}

		if (lhsFirst)
		{
			if (rhsFirst)
			{
				lhsLast->Next = rhsFirst;
				rhsFirst->Prev = lhsLast;
				*first = lhsFirst;
				*last = rhsLast;
			}
			else
			{
				*first = lhsFirst;
				*last = lhsLast;
			}
		}
		else
		{
			*first = rhsFirst;
			*last = rhsLast;
		}

	}

	void SortQuick(Entry * first, Entry * last, int order)
	{

		if (first == 0 || last == 0)
		{
			if (first == 0 && last == 0)
			{
				first = this->First;
				last = this->Last;
			}
			else
			{
				return;
			}
		}

		if (order == ORDER_DEFAULT)
		{
			order = ORDER_ASCENDING;
		}
		else
		if (order != ORDER_ASCENDING && order != ORDER_DESCENDING)
		{
			OutputError("List:Sort - Sort failed, an invalid order was specified.\n");
			return;
		}

		while (first != last)

		{

			Entry * right	= (Entry*)last;
			Entry * left	= (Entry*)first;
			Entry * swap = 0;
			int shift=0;

			Entry * pivot=left;
			Entry * pivotNext=pivot;
			int size=0, pivotSize=0, pivotNextSize=0;
			while (left != right)
			{
				left = left->Next;
				++size;
				if (size > (pivotNextSize<<1))
				{
					pivot = pivotNext;
					pivotSize = pivotNextSize;
					pivotNext = left;
					pivotNextSize = size;	
				}
			}

			left = (Entry*)first;

			if (this->Initial->Base->Compare(left,pivot) >= 0)

			{
				if (this->Initial->Base->Compare(right,pivot) >= 0)

				{
					if (this->Initial->Base->Compare(left,right) >= 0)

					{

						pivot = right;
					}
					else
					{

						pivot = left;
					}
				}
				else
				{

					pivot = pivot;
				}
			}
			else
			if (this->Initial->Base->Compare(right,pivot) >= 0)

			{

				pivot = pivot;
			}
			else
			if (this->Initial->Base->Compare(right,left) >= 0)

			{

				pivot = right;
			}
			else
			{

				pivot = left;
			}

			if (pivot != left)
			{
				Swap((Variant<_Kind_>*)first,(Variant<_Kind_>*)pivot);
				if (pivot == last) last = first;
				first = pivot;
			}

			right	= (Entry*)last;
			left	= (Entry*)first;

			while(left != right)
			{
				if (order == ORDER_ASCENDING)
				{
					while (left != right && this->Initial->Base->Compare(left,first) <= 0)				

					{
						left = left->Next;
						++shift;
					}

					while (right != left && this->Initial->Base->Compare(right,first) > 0)

					{
						right = right->Prev;
						--shift;
					}
				}
				else
				{
					while (left != right && this->Initial->Base->Compare(left,first) >= 0)

					{
						left = left->Next;
						++shift;
					}

					while (right != left && this->Initial->Base->Compare(right,first) < 0)

					{
						right = right->Prev;
						--shift;
					}
				}

				if (left != right)
				{
					Swap((Variant<_Kind_>*)left,(Variant<_Kind_>*)right);

					swap = left;

					if (left == first) first = right;

					left = right;				

					if (right == last) last = swap;

					right = swap;					
				}
			}

			if (	(order == ORDER_ASCENDING && this->Initial->Base->Compare(right,first) > 0) ||
					(order == ORDER_DESCENDING && this->Initial->Base->Compare(right,first) < 0 ) )

				right = right->Prev;

			Swap((Variant<_Kind_>*)first,(Variant<_Kind_>*)right);
			swap = (Entry*)first;

			if (first == left) left = right;

			first = right;				

			if (right == left) left = swap;
			if (right == last) last = swap;

			right = swap;		

			if (left == right)
				right = right->Prev;

			if (shift > 0)
			{

				SortQuick(left,last,order);
				last = right;
			}
			else
			{

				SortQuick(first,right,order);
				first = left;
			}
		}

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Structure {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> >
class List : public Abstract::OrderedList<_Kind_,_Policy_>
{
public:

	List(typename Template<_Kind_>::ConstantReference type)
	{
		Append(type);
	}

	List(const List & list, bool consume=false)
	{

		if (consume)
			Abstract::OrderedList<_Kind_,_Policy_>::Consume(list);
		else
			operator = (list);
	}

	List(const Iterable<_Kind_> & iterable)
	{
		operator = (iterable);
	}

	List()
	{

	}

	List & operator = (const List & list)
	{
		typename Abstract::OrderedList<_Kind_,_Policy_>::Entry * entry = list.First;
		while (entry != 0)
		{
			Insert((*entry)());
			entry = entry->Next;
		}

		return *this;
	}

	List & operator = (const Iterable<_Kind_> & iterable)
	{
		Reason::Structure::Iterator<_Kind_> iterator = ((Iterable<_Kind_>&)iterable).Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());

		return *this;
	}

	~List()
	{

	}

	bool Contains(typename Template<_Kind_>::ConstantReference kind) {return Abstract::OrderedList<_Kind_,_Policy_>::Select(kind)!=0;}
	int IndexOf(typename Template<_Kind_>::ConstantReference kind) {return Abstract::OrderedList<_Kind_,_Policy_>::IndexOf(kind);}

	List & operator = (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;}
	List & operator , (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;}

	List & operator << (typename Template<_Kind_>::ConstantReference type) {Append(type);return *this;};
	List & operator >> (typename Template<_Kind_>::ConstantReference type) {Prepend(type);return *this;};

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

