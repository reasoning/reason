
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
#ifndef STRUCTURE_TABLE_H
#define STRUCTURE_TABLE_H

#include "reason/generic/generic.h"
#include "reason/structure/list.h"
#include "reason/structure/array.h"
#include "reason/structure/hashable.h"
#include "reason/system/interface.h"

using namespace Reason::Generic;
using namespace Reason::Structure;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Structure { namespace Abstract {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_, typename _Policy_=Default<_Kind_> >

class Hashtable : public Reason::Structure::Iterable<_Kind_>, public Reason::System::Comparable
{
public:

	Hashtable():
		Iterable<_Kind_>(new _Policy_())
	{
	}

	~Hashtable()
	{
	}

	virtual void Release()
	{
	}

	virtual void Destroy()
	{
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind, int key, bool unique=false)=0;
	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind, bool unique=false)=0;
	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & iterand, bool unique=false)=0;
	virtual Iterand<_Kind_> InsertKey(int key)=0;

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> & iterand)=0;
	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference kind, int key=0)=0;
	virtual Iterand<_Kind_> SelectKey(int key, Iterand<_Kind_> prev=0)=0;

	virtual Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference kind, int key=0)=0;
	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> & iterand)=0;

	virtual Iterand<_Kind_> Delete(typename Template<_Kind_>::ConstantReference kind, int key=0)=0;
	virtual Iterand<_Kind_> Delete(Iterand<_Kind_> & iterand)=0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 
class ChainedHashtable : public Hashtable<_Kind_,_Policy_>
{
public:

	static Identity Instance;

	Identity & Identify() {return Instance;}

	typedef typename LinkedList<_Kind_>::Entry Entry;

	virtual Entry * New()
	{
		void * data = Iterable<_Kind_>::New(sizeof(Entry));
		return new (data) Entry();
	}

	virtual Entry * New(Entry * entry)
	{
		void * data = Iterable<_Kind_>::New(sizeof(Entry));
		return new (data) Entry(entry);
	}

	virtual Entry * New(typename Template<_Kind_>::ConstantReference kind, int key)
	{
		void * data = Iterable<_Kind_>::New(sizeof(Entry));
		return new (data) Entry(kind,key);
	}

	using Iterable<_Kind_>::Del;
	virtual void Del(Entry * entry)
	{
		entry->~Entry();
		Iterable<_Kind_>::Del((void*)entry);
	}

	using Iterable<_Kind_>::Compare;	
	virtual int Compare(Iterand<_Kind_> & left, Iterand<_Kind_> & right)
	{
		return Iterable<_Kind_>::Compare(left,right);
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

	virtual void Dispose()
	{	
		Entry *entry;
		for (int i=0;i<Capacity;++i)
		{
			while (Entries[i] != 0)
			{
				entry = Entries[i];
				Entries[i] = entry->Next;
				Dispose(entry);
			}
		}

		Count=0;
		Resized=0;
		Modified=0;
	}

	virtual void Release(Iterand<_Kind_> & iterand)
	{	
		if (iterand.Proxy == 0 && iterand.Iteration && iterand.Iteration->Iterable() == this)
		{
			Release((Entry*)&iterand);
		}
	}

	virtual void Release(Entry * entry)
	{
		Reason::Structure::Disposer<_Kind_>::Release((*entry)());
		Del(entry);
	}

	virtual void Destroy(Iterand<_Kind_> & iterand)
	{
		if (iterand.Proxy == 0 && iterand.Iteration && iterand.Iteration->Iterable() == this)
		{
			Destroy((Entry*)&iterand);
		}
	}

	virtual void Destroy(Entry * entry)
	{
		Reason::Structure::Disposer<_Kind_>::Destroy((*entry)());
		Del(entry);

	}	

public:

	float Threshold;	
	int Capacity;
	int Count;			

	int Modified;	
	int Resized;	

	Entry ** Entries;

	ChainedHashtable(const ChainedHashtable & table):
		Threshold(0.75),Capacity(0),Count(0),Modified(0),Resized(0),Entries(0)
	{
		operator = (table);
	}

	ChainedHashtable(Iterable<_Kind_> & iterable):
		Threshold(0.75),Capacity(0),Count(0),Modified(0),Resized(0),Entries(0)
	{
		operator = (iterable);
	}

	ChainedHashtable(int capacity):
		Threshold(0.75),Capacity(0),Count(0),Modified(0),Resized(0),Entries(0)
	{
		Allocate(capacity);
	}

	ChainedHashtable():
		Threshold(0.75),Capacity(0),Count(0),Modified(0),Resized(0),Entries(0)
	{

	}

	~ChainedHashtable()
	{
		Dispose();
		if (Entries)
			delete [] Entries;
	}

	void Allocate(int amount=0)
	{

		if (amount < 0)
			return;

		if (Capacity)
		{

			amount = (amount==0)?(int)(Capacity * 2+1):Capacity + amount;

			++Resized;

			Entry ** entries = new Entry * [amount];
			memset(entries,0, amount * sizeof(Entry *));

			Entry *entry;
			int index;

			for (int i=0;i < Capacity;++i)
			{
				while (Entries[i] != 0)
				{
					entry = Entries[i];
					Entries[i] = Entries[i]->Next;

					index = (unsigned int)entry->Key % amount;

					entry->Next = entries[index];
					entries[index] = entry;
				}
			}

			Capacity = amount;
			delete [] Entries;
			Entries = entries;

		}
		else
		{
			OutputAssert(Entries == 0 && Capacity == 0);
			Entries = new Entry * [amount];
			memset(Entries,0, amount * sizeof(Entry *));
			Capacity = amount;
		}
	}

	ChainedHashtable & operator = (const ChainedHashtable & table)
	{
		if (&table == this) return *this;

		Modified = Resized = 0;
		Allocate(table.Capacity);
		if (Count == 0)
		{

			Count = table.Count;

			int key = 0;			
			do 
			{
				while(key < (table.Capacity-1) && !table.Entries[key])
					++key;

				Entry * from = (key < (table.Capacity-1))?table.Entries[key]:0;

				if (from)
				{
					Entry *& to = Entries[key];	
					to = New(from);		

					while(from->Next)
					{
						to->Next = New(from->Next);
						to->Next->Prev = to;							
						to = to->Next;
						from = from->Next;					
					}
				}				
			}
			while(++key < table.Capacity-1);

			return *this;
		}
		else
		{
			return operator = ((Iterable<_Kind_>&)table);
		}
	}

	ChainedHashtable & operator = (Iterable<_Kind_> & iterable)
	{
		Modified = Resized = 0;
		Iterand<_Kind_> iterand = iterable.Forward();
		while(iterand != 0)
		{
			Insert(iterand());
			++iterand;
		}
		return *this;
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind, int key, bool unique=false)
	{

		Entry * entry = New(kind,key);
		Iterand<_Kind_> insert = Insert(Iterand<_Kind_>::Put(entry),unique);
		return insert;
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind, bool unique=false)
	{
		Entry * entry = New(kind,0);
		Iterand<_Kind_> insert = Insert(Iterand<_Kind_>::Put(entry),unique);
		return insert;
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_>& kind, bool unique=false)
	{
		if (!kind) return Iterand<_Kind_>::Null();

		Entry * entry = (Entry*) this->Put(kind);
		if (!entry) entry = New(kind(),0);

		if (Capacity == 0)
		{
			Allocate(31);
		}
		else
		if (Count == (int)(Threshold*Capacity))
		{
			Allocate();
		}

		if (entry->Key==0)
			entry->Key = Hashable<_Kind_>::Hash((*entry)());

		int index = (unsigned int) entry->Key % Capacity;

		if (unique)
		{
			Iterand<_Kind_> select = Select(kind);
			if (!select)
			{
				entry->Next = Entries[index];
				Entries[index] = entry;
				++Count;
				++Modified;
			}
			else
			{
				Dispose(entry);
				return select.Unique();
			}
		}
		else
		{
			entry->Next = Entries[index];
			Entries[index] = entry;
			++Count;
			++Modified;

		}

		return Iterand<_Kind_>(this->Initial,entry);
	}

	virtual Iterand<_Kind_> InsertKey(int key)
	{	

		Entry * entry = New();
		entry->Key = key;
		Iterand<_Kind_> insert = Insert(Iterand<_Kind_>::Put(entry));
		return insert;
	}

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> & kind)
	{
		if (Count == 0 || !kind) return Iterand<_Kind_>::Null();

		Entry * entry = (Entry*) this->Get(kind);
		if (!entry) return Select(kind());

		int key = entry->Key;
		entry = Entries[(unsigned int)key % Capacity];
		while (entry != 0)
		{
			if (entry->Key == key && Compare((*entry)(),kind()) == 0)
				break;

			entry = entry->Next;
		}

		return Iterand<_Kind_>(this->Initial,entry);
	}

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference kind, int key=0)
	{
		if (Count == 0) return Iterand<_Kind_>::Null();

		if (key == 0) key = Hashable<_Kind_>::Hash(kind);

		Entry * entry = Entries[(unsigned int)key % Capacity];
		while (entry != 0)
		{
			if (entry->Key == key && Compare((*entry)(),kind) == 0)
				break;

			entry = entry->Next;
		}

		return Iterand<_Kind_>(this->Initial,entry);
	}

	virtual Iterand<_Kind_> SelectKey(int key, Iterand<_Kind_> prev=0)
	{
		if (Count == 0) return Iterand<_Kind_>::Null();

		Entry * entry = 0;
		if (prev == 0)
		{
			entry = Entries[((unsigned int)key % Capacity)];
		}
		else
		{
			if (this->Get(prev))
				entry = ((Entry*)&prev)->Next;
		}

		while (entry != 0)
		{
			if (entry->Key == key)
				break;

			entry = entry->Next;
		}

		return Iterand<_Kind_>(this->Initial,entry);
	}

	virtual Iterand<_Kind_> Update(typename Template<_Kind_>::ConstantReference kind)
	{
		Entry * entry = New(kind,0);
		return Update(Iterand<_Kind_>::Put(entry));
	}

	virtual Iterand<_Kind_> Update(Iterand<_Kind_> & kind)
	{

		Iterand<_Kind_> insert = Insert(kind,false);
		if (!insert)
		{
			if ((insert.Option&Iterand<_Kind_>::STATE_UNIQUE))
			{

				Entry * update = (Entry*)&kind;
				Entry * entry = (Entry*)&insert;

				if (entry->Prev && entry->Prev->Next == entry)
					entry->Prev->Next = update;
				if (entry->Next && entry->Next->Prev == entry)
					entry->Next->Prev = update;

				Dispose(entry);

				return Iterand<_Kind_>(this->Initial, update);
			}
		}

		return insert;
	}

	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> &kind)
	{
		if (!kind) return Iterand<_Kind_>::Null();

		Entry * entry = (Entry *) this->Get(kind);
		if (!entry) return Remove(kind());

		if (entry->Prev != 0)
		{
			entry->Prev->Next = entry->Next;
		}
		else
		{
			Entries[(unsigned int)entry->Key % Capacity] = entry->Next;
		}

		entry->Prev = 0;
		entry->Next = 0;

		--Count;
		++Modified;

		Iterand<_Kind_> remove = Iterand<_Kind_>(this->Initial,entry).Dispose(Disposable::DISPOSE_POLICY);			
		return remove;
	}

	virtual Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference kind, int key=0)
	{
		Iterand<_Kind_> select = Select(kind,key);
		if (select) return Remove(select);

		return Iterand<_Kind_>::Null();
	}

	virtual Iterand<_Kind_> Delete(typename Template<_Kind_>::ConstantReference kind, int key=0)
	{
		Iterand<_Kind_> select = Select(kind,key);
		if (select) 
		{
			Iterand<_Kind_> remove = Remove(select);
			return remove.Dispose(Disposable::DISPOSE_DESTROY);
		}

		return Iterand<_Kind_>::Null();
	}

	virtual Iterand<_Kind_> Delete(Iterand<_Kind_> & kind)
	{
		Iterand<_Kind_> remove = Remove(kind);
		if (remove)
		{
			return remove.Dispose(Disposable::DISPOSE_DESTROY);
		}

		return Iterand<_Kind_>::Null();
	}

	void Release()
	{
		Entry *entry;
		for (int i=0;i<Capacity;++i)
		{
			while (Entries[i] != 0)
			{
				entry = Entries[i];
				Entries[i] = entry->Next;
				Release(entry);
			}
		}

		Count=0;
		Resized=0;
		Modified=0;
	}

	void Destroy()
	{
		Entry *entry;
		for (int i=0;i<Capacity;++i)
		{
			while (Entries[i] != 0)
			{
				entry = Entries[i];
				Entries[i] = entry->Next;
				Destroy(entry);
			}
		}

		Count=0;
		Resized=0;
		Modified=0;

		if (Entries)
			delete [] Entries;

		Entries=0;
		Capacity=0;
	}

	ChainedHashtable<_Kind_> & operator = (typename Template<_Kind_>::ConstantReference type) {Insert(type);return *this;}
	ChainedHashtable<_Kind_> & operator , (typename Template<_Kind_>::ConstantReference type) {Insert(type);return *this;}

	ChainedHashtable<_Kind_> & operator << (typename Template<_Kind_>::ConstantReference type) {Insert(type);return *this;};

public:

	class EntryIteration : public Iteration<_Kind_>
	{
	public:

		EntryIteration(ChainedHashtable<_Kind_> * hashtable):Iteration<_Kind_>(hashtable)
		{

		}

		Iteration<_Kind_> * Clone()
		{

			return this;
		}

		void Forward(Iterand<_Kind_> & iterand)
		{
			iterand.Forward();

			Entry * entry = (Entry*)&iterand;
			ChainedHashtable<_Kind_,_Policy_> * table = (ChainedHashtable<_Kind_,_Policy_> *)this->Iterable();

			int key = 0;
			while(key < (table->Capacity-1) && !table->Entries[key])
				++key;

			entry = (key < (table->Capacity-1))?table->Entries[key]:0;
			iterand.Variant(entry);
		}

		void Reverse(Iterand<_Kind_> & iterand)
		{
			iterand.Reverse();

			Entry * entry = (Entry*)&iterand;
			ChainedHashtable<_Kind_,_Policy_> * table = (ChainedHashtable<_Kind_,_Policy_> *)this->Iterable();

			int key = table->Capacity-1;
			while(key > 0 && !table->Entries[key])
				-- key;

			if (key > 0)
			{
				entry = table->Entries[key];
				while(entry->Next)
					entry = entry->Next;
			}
			else
			{
				entry = 0;
			}

			iterand.Variant(entry);
		}

		void Move(Iterand<_Kind_> & iterand)
		{
			Move(iterand,1);
		}

		void Move(Iterand<_Kind_> & iterand, int amount)
		{

			amount *= iterand.Move();

			Entry * entry = (Entry*)&iterand;
			ChainedHashtable<_Kind_,_Policy_> * table = (ChainedHashtable<_Kind_,_Policy_> *)this->Iterable();

			if (entry)
			{
				int key = (unsigned int)entry->Key % table->Capacity;

				if (amount > 0)
				{
					while (entry != 0 && amount-- > 0)
					{
						if (!entry->Next)
						{
							++key;
							while(key < (table->Capacity-1) && !table->Entries[key])
								++key;

							entry = (key < (table->Capacity-1))?table->Entries[key]:0;
						}
						else
						{
							entry = entry->Next;
						}
					}
				}
				else
				if (amount < 0)
				{
					while (entry != 0 && amount++ < 0)
					{

						if (!entry->Prev)
						{
							--key;
							while(key > 0 && !table->Entries[key])
								-- key;

							if (key > 0)
							{
								entry = table->Entries[key];
								while(entry->Next)
									entry = entry->Next;
							}
							else
							{
								entry = 0;
							}

						}
						else
						{
							entry = entry->Prev;
						}
					}
				}			
			}

			iterand.Variant(entry);
		}

	};

	Structure::Iterator<_Kind_> Iterate()
	{
		return Structure::Iterator<_Kind_>(new EntryIteration(this));
	}

protected:

};

template<typename _Kind_,typename _Policy_>
Identity ChainedHashtable<_Kind_,_Policy_>::Instance;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_, typename _Policy_=Default<_Kind_> >
class StaticHashtable : public ChainedHashtable<_Kind_,_Policy_>
{
public:

};

template<typename _Kind_, typename _Policy_=Default<_Kind_> >
class DynamicHashtable : public ChainedHashtable<_Kind_,_Policy_>
{
public:

	typedef typename ChainedHashtable<_Kind_,_Policy_>::Entry Entry;

	DynamicHashtable(int amount=0):
		ChainedHashtable(amount)
	{
	}	

	~DynamicHashtable()
	{
	}

	virtual void Allocate(int amount=0)
	{

		if (amount < 0)
			return;

		if (Capacity)
		{

			amount = (amount==0)?(int)(Capacity * 2+1):Capacity + amount;

			Entry ** entries = new Entry* [amount];
			memset(entries,0, amount * sizeof(Entry *));

			Entry *entry;
			int index;

			for (int i=0;i < Capacity;++i)
			{
				while (Entries[i] != 0)
				{
					entry = Entries[i];
					Entries[i] = Entries[i]->Next;

					index = (unsigned int)entry->Key % amount;

					entry->Next = storage[index];
					entries[index] = entry;
				}
			}

			Capacity = amount;
			delete [] Entries;
			Entries = entries;

		}
		else
		{
			OutputAssert(Entries == 0 && Capacity == 0);
			amount = (amount==0)?31:amount;

			Entries = new Entry * [amount];
			memset(Entries,0, amount * sizeof(Entry *));
			Capacity = amount;
		}
	}

	virtual void Release()=0;
	virtual void Destroy()=0;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_, typename _Policy_=Default<_Kind_> >
class BinaryHashtable : public DynamicHashtable<_Kind_,_Policy_>
{
public:

	typedef typename DynamicHashtable<_Kind_,_Policy_>::Entry Entry;

	BinaryHashtable(int amount=0):
		DynamicHashtable<_Kind_,_Policy_>(amount)
	{
	}

	~BinaryHashtable()
	{
	}

	void Allocate(int amount=0)
	{

		if (amount < 0)
			return;

		if (this->Capacity)
		{

			amount = (amount==0)?(this->Capacity<<1):(1<<(Binary::Power(amount)+1));

			Entry ** entries = new Entry* [amount];
			memset(entries,0, amount * sizeof(Entry *));

			Entry *entry;
			int index;

			for (int i=0;i < this->Capacity;++i)
			{
				while (this->Entries[i] != 0)
				{
					entry = this->Entries[i];
					this->Entries[i] = this->Entries[i]->Next;

					index = (unsigned int)entry->Key % amount;

					entry->Next = entries[index];
					entries[index] = entry;
				}
			}

			this->Capacity = amount;
			delete [] this->Entries;
			this->Entries = entries;

		}
		else
		{
			OutputAssert(Entries == 0 && this->Capacity == 0);
			amount = (amount==0)?16:(1<<(Binary::Power(amount)));

			this->Entries = new Entry * [amount];
			memset(Entries,0, amount * sizeof(Entry *));
			this->Capacity = amount;
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

template <typename _Kind_, typename _Policy_=Default<_Kind_> >
class LinearHashtable : public DynamicHashtable<_Kind_,_Policy_>
{
public:

};

template <typename _Kind_>
class DoubleHashtable
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class QuadraticHashtable
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class SparseHashtable
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class PerfectHashtable
{

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class CuckooHashtable
{

};

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
class Hashtable : public Abstract::ChainedHashtable<_Kind_,_Policy_>
{
public:

	Hashtable()
	{

	}

	Hashtable(int capacity):Abstract::ChainedHashtable<_Kind_>(capacity)
	{

	}

	Hashtable(const Iterable<_Kind_> & iterable)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());
	}

	Hashtable & operator = (const Iterable<_Kind_> & iterable)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());

		return *this;
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

