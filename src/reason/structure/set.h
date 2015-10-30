
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
#ifndef STRUCTURE_SET_H
#define STRUCTURE_SET_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/platform/platform.h"

#include "reason/generic/generic.h"
#include "reason/structure/comparable.h"
#include "reason/structure/tree.h"
#include "reason/structure/table.h"
#include "reason/structure/array.h"
#include "reason/structure/policy.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Reason { namespace Structure { namespace Abstract {
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Set
{
public:

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & iterand)=0;

	virtual Iterand<_Kind_> Update(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Update(Iterand<_Kind_> & iterand)=0;

	virtual Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> & iterand)=0;

	virtual Iterand<_Kind_> Delete(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Delete(Iterand<_Kind_> & iterand)=0;

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Select(Iterand<_Kind_> & iterand)=0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 
class OrderedSet : public RedBlackTree<_Kind_,_Policy_>
{
public:

	typedef typename RedBlackTree<_Kind_>::Node Node;

	Node * Min;
	Node * Max;

	OrderedSet(const OrderedSet & set):Min(0),Max(0)
	{
		operator = (set);
	}

	OrderedSet(Iterable<_Kind_> & iterable):Min(0),Max(0)
	{
		operator = (iterable);
	}

	OrderedSet(int comparitor=RedBlackTree<_Kind_>::COMPARE_GENERAL):
		RedBlackTree<_Kind_>(comparitor),Min(0),Max(0)
	{
	}

	~OrderedSet()
	{
	}

	OrderedSet & operator = (const OrderedSet & set)
	{
		if (&set == this) return *this;

		if (this->Count == 0)
		{
			RedBlackTree<_Kind_>::operator = (set);
			Min = (Node *) &RedBlackTree<_Kind_>::Minimum();
			Max = (Node *) &RedBlackTree<_Kind_>::Maximum();
			return *this;
		}
		else
		{
			return operator = ((Iterable<_Kind_>&)set);
		}		
	}

	OrderedSet & operator = (Iterable<_Kind_> & iterable)
	{
		Iterand<_Kind_> iterand = iterable.Forward();
		while (iterand != 0)
		{	
			RedBlackTree<_Kind_>::Insert(iterand(),true);
			++iterand;
		}

		Min = (Node *) &RedBlackTree<_Kind_>::Minimum();
		Max = (Node *) &RedBlackTree<_Kind_>::Maximum();
		return *this;
	}

	OrderedSet & operator = (typename Template<_Kind_>::ConstantReference type) {RedBlackTree<_Kind_>::Insert(type,true);return *this;}
	OrderedSet & operator , (typename Template<_Kind_>::ConstantReference type) {RedBlackTree<_Kind_>::Insert(type,true);return *this;}

	OrderedSet & operator << (typename Template<_Kind_>::ConstantReference type) {RedBlackTree<_Kind_>::Insert(type,true);return *this;};

	int Length()
	{
		return this->Count;
	}

	bool IsEmpty()
	{
		return this->Count==0;
	}

    virtual Iterand<_Kind_> Minimum()
    {
		return (Variant<_Kind_>*)Min;
    }

    virtual Iterand<_Kind_> Maximum()
    {
		return (Variant<_Kind_>*)Max;
    }

	void Take(OrderedSet<_Kind_> & set)
	{
		if (&set == this) return;

		Release();

		this->Count = set.Count;
		this->Min = set.Min;
		this->Max = set.Max;
		this->Root = set.Root;

		set.Count = 0;
		set.Min = 0;
		set.Max = 0;
		set.Root = 0;
	}

	void Take(OrderedSet<_Kind_> & set, Iterand<_Kind_> & iterand)
	{
		if (&set == this) return;

		set.Remove(iterand);
		Insert(iterand);
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & iterand)
	{
		Node * node = (Node*) &iterand;

		Iterand<_Kind_> insert = RedBlackTree<_Kind_>::Insert(iterand,true);
		if (insert)
		{
			if (Min == 0 && Max == 0)
			{
				Min = Max = node;
			}
			else
			if (node->Parent == Min && node->IsLeft())
			{
				Min = node;
			}
			else
			if (node->Parent == Max && node->IsRight())
			{
				Max = node;
			}		

			return insert;
		}
		else
		{

			return Iterand<_Kind_>::Null();
		}
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind)
	{

		Node * node = RedBlackTree<_Kind_>::New(kind);
		Iterand<_Kind_> insert = RedBlackTree<_Kind_>::Insert(Iterand<_Kind_>::Put(node),true);
		if (insert)
		{
			if (Min == 0 && Max == 0)
			{
				Min = Max = node;
			}
			else
			if (node->Parent == Min && node->IsLeft())
			{
				Min = node;
			}
			else
			if (node->Parent == Max && node->IsRight())
			{
				Max = node;
			}		
		}

		return insert;
	}

	using RedBlackTree<_Kind_,_Policy_>::Remove;
	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> & kind)
	{
		if (!kind) return Iterand<_Kind_>::Null();

		Node * node = (Node*) this->Get(kind);
		if (!node)
		{
			return RedBlackTree<_Kind_>::Remove(kind());
		}

		if (node == Min)
		{

			Min = (Node*)node->Right;
			while(Min && Min->Left) Min = (Node*)Min->Left;
			if (Min == 0) Min = (Node*)node->Parent;
		}

		if (node == Max)
		{

			Max = (Node*)node->Left;
			while (Max && Max->Right) Max = (Node*)Max->Right;
			if (Max == 0) Max = (Node*)node->Parent;
		}		

		Iterand<_Kind_> remove = RedBlackTree<_Kind_>::Remove(kind);

		return remove;
	}

	virtual Iterand<_Kind_> Contains(typename Template<_Kind_>::ConstantReference kind)
	{
		return RedBlackTree<_Kind_>::Select(kind);
	}

	virtual Iterand<_Kind_> Contains(Iterand<_Kind_> & iterand)
	{
		return RedBlackTree<_Kind_>::Select(iterand);
	}

	void Release()
	{
		RedBlackTree<_Kind_>::Release();
		Min=0;
		Max=0;
	}

	void Destroy()
	{
		RedBlackTree<_Kind_>::Destroy();
		Min=0;
		Max=0;
	}
};

template <typename _Kind_, typename _Policy_=Default<_Kind_> >
class UnorderedSet : public ChainedHashtable<_Kind_,_Policy_>
{
public:

	typedef typename ChainedHashtable<_Kind_,_Policy_>::Entry Entry;

	UnorderedSet(const UnorderedSet & set)
	{
		operator = (set);
	}

	UnorderedSet(Iterable<_Kind_> & iterable)
	{
		operator = (iterable);
	}

	UnorderedSet(int comparitor=Reason::System::Comparable::COMPARE_GENERAL)
	{
		this->Initial->Base->Compare.Option = comparitor;
	}

	~UnorderedSet()
	{
	}

	UnorderedSet & operator = (const UnorderedSet & set)
	{
		this->Initial->Base->Compare.Option = set.Initial->Base->Compare.Option;
		if (this->Count == 0)
		{
			return (UnorderedSet&) ChainedHashtable<_Kind_>::operator = (set);		
		}
		else
		{
			return operator = ((Iterable<_Kind_>&)set);
		}
	}

	UnorderedSet & operator = (Iterable<_Kind_> & iterable)
	{	
		this->Modified = this->Resized = 0;
		Iterand<_Kind_> iterand = iterable.Forward();
		while(iterand != 0)
		{
			ChainedHashtable<_Kind_>::Insert(iterand(),true);
			++iterand;
		}
		return *this;
	}

	int Length()
	{
		return this->Count;
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & iterand)
	{
		return ChainedHashtable<_Kind_>::Insert(iterand,true);
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind)
	{
		return ChainedHashtable<_Kind_>::Insert(kind,true);
	}

	virtual Iterand<_Kind_> Contains(typename Template<_Kind_>::ConstantReference kind)
	{
		return ChainedHashtable<_Kind_>::Select(kind);
	}

	virtual Iterand<_Kind_> Contains(Iterand<_Kind_> iterand)
	{
		return ChainedHashtable<_Kind_>::Select(iterand);
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
class Set : public Abstract::OrderedSet<_Kind_,_Policy_>
{
public:

	Set(const Set & set)
	{
		operator = (set);
	}

	Set(Iterable<_Kind_> & iterable)
	{
		operator = (iterable);
	}

	Set(int comparitor=Abstract::OrderedSet<_Kind_>::COMPARE_GENERAL):
		Abstract::OrderedSet<_Kind_,_Policy_>(comparitor)
	{
	}

	Set(const Iterable<_Kind_> & iterable,int comparitor=Abstract::OrderedSet<_Kind_>::COMPARE_GENERAL):
		Abstract::OrderedSet<_Kind_,_Policy_>(comparitor)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());
	}

	using Abstract::OrderedSet<_Kind_,_Policy_>::operator =;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> >
class Hashset : public Abstract::UnorderedSet<_Kind_,_Policy_>
{
public:

	Hashset(int comparitor=Reason::System::Comparable::COMPARE_GENERAL):
		Abstract::UnorderedSet<_Kind_,_Policy_>(comparitor)
	{
	}

	Hashset(const Iterable<_Kind_> & iterable,int comparitor=Reason::System::Comparable::COMPARE_GENERAL):
		Abstract::UnorderedSet<_Kind_,_Policy_>(comparitor)
	{
		Iterator<_Kind_> iterator = iterable.Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
			Insert(iterator());
	}

	~Hashset()
	{
	}

	Hashset & operator = (const Iterable<_Kind_> & iterable)
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

template <typename _Kind_, typename _Policy_=Default<_Kind_> >
class Arrayset : public Abstract::OrderedArray<_Kind_,_Policy_>
{
public:

	Arrayset()
	{

	}

	virtual Iterand<_Kind_> Update(typename Template<_Kind_>::ConstantReference kind)
	{
		Iterand<_Kind_> select = this->Select(kind);
		if (!select)
		{
			return this->Insert(kind);
		}
		else
		{
			Dispose(select);
			*((_Kind_*)&select) = (typename Template<_Kind_>::Reference)kind;
			return select;
		}
	}

	virtual Iterand<_Kind_> Update(Iterand<_Kind_> & kind)
	{
		if (!kind) return Iterand<_Kind_>::Null();

		return Update(kind());

	}

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

