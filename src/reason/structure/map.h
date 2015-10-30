
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
#ifndef STRUCTURE_MAP_H
#define STRUCTURE_MAP_H

#include "reason/generic/generic.h"
#include "reason/structure/comparable.h"
#include "reason/structure/disposable.h"
#include "reason/structure/set.h"
#include "reason/system/object.h"
#include "reason/system/interface.h"
#include "reason/structure/iterator.h"
#include "reason/structure/policy.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Structure {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Key_, typename _Value_> 
class Mapped: public Object
{
public:

	static Identity Instance;
	virtual Identity& Identify(){return Instance;};

public:
	Variable<_Key_> Key;
	Variable<_Value_> Value;

	Mapped()
	{
	}

	Mapped(const Mapped & mapped):Key(mapped.Key),Value(mapped.Value)
	{
	}

	Mapped(typename Template<_Key_>::ConstantReference key):Key(key)
	{
	}

	Mapped(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value):Key(key),Value(value)
	{
	}

	~Mapped()
	{
	}

	Mapped & operator = (const Mapped & mapped)
	{
		Key = mapped.Key;
		Value = mapped.Value;
		return *this;
	}

	int Compare(Object * object, int comparitor = Comparable::COMPARE_GENERAL)
	{	
		if (object->InstanceOf(this))
		{
			Mapped<_Key_,_Value_> * mapped = (Mapped<_Key_,_Value_> *)object;
			return Reason::Structure::Comparer<_Key_>::Compare(this->Key(),mapped->Key(),comparitor);
		}
		return Identity::Error;
	}

	int Hash()
	{
		return Hashable<_Key_>::Hash(Key());
	}
};

template <typename _Key_, typename _Value_> 
Identity Mapped<_Key_,_Value_>::Instance;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

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

template <typename _Key_, typename _Value_, typename _Policy_=Default< Mapped<_Key_,_Value_> > >
class UnorderedMap : public UnorderedSet< Mapped<_Key_,_Value_>, _Policy_>
{
public:

	typedef typename UnorderedSet<Mapped<_Key_,_Value_> >::Entry Entry;

	UnorderedMap(const UnorderedMap & map)
	{
		operator = (map);
	}

	UnorderedMap(Iterable<_Key_> & iterable)
	{
		operator = (iterable);
	}

	UnorderedMap(Iterable< Mapped<_Key_,_Value_> > & iterable)
	{
		operator = (iterable);
	}

	UnorderedMap(int comparitor=Reason::System::Comparable::COMPARE_GENERAL):
		UnorderedSet< Mapped<_Key_,_Value_> >(comparitor)
	{
	}

	~UnorderedMap()
	{
	}

	using UnorderedSet< Mapped<_Key_,_Value_> >::operator =;

	UnorderedMap & operator = (Iterable<_Key_> & iterable)
	{
		Iterand<_Key_> iterand = iterable.Forward();
		while(iterand != 0)
		{
			UnorderedSet< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(iterand()));
			++iterand;
		}

		return *this;
	}	

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
	{
		return UnorderedSet< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(key,value));
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key)
	{
		return UnorderedSet< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(key));
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(Iterand<_Key_> & iterand)
	{

		return UnorderedSet< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(iterand()));
	}	

	using UnorderedSet< Mapped<_Key_,_Value_> >::Update;	
	virtual Iterand< Mapped<_Key_,_Value_> > Update(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
	{
		Iterand< Mapped<_Key_,_Value_> > result = UnorderedSet< Mapped<_Key_,_Value_> >::Select(key);
		if (result)
		{
			result().Value = value;
			return result;
		}
		else
		{
			return Insert(key,value);
		}	
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Update(typename Template<_Key_>::ConstantReference key)
	{
		Mapped<_Key_,_Value_> keyed(key);
		Iterand< Mapped<_Key_,_Value_> > update(keyed);
		return UnorderedSet< Mapped<_Key_,_Value_> >::Update(update);	
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Update(Iterand<_Key_> & iterand)
	{
		Mapped<_Key_,_Value_> keyed(iterand());
		Iterand< Mapped<_Key_,_Value_> > update(keyed);
		return UnorderedSet< Mapped<_Key_,_Value_> >::Update(update);
	}		

	using UnorderedSet< Mapped<_Key_,_Value_> >::Remove;
	virtual Iterand< Mapped<_Key_,_Value_> > Remove(typename Template<_Key_>::ConstantReference key)
	{
		Mapped<_Key_,_Value_> keyed(key);
		return UnorderedSet< Mapped<_Key_,_Value_> >::Remove(keyed);
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Remove(Iterand<_Key_> & iterand)
	{
		return UnorderedSet< Mapped<_Key_,_Value_> >::Remove(Mapped<_Key_,_Value_>(iterand()));
	}	

	using UnorderedSet< Mapped<_Key_,_Value_> >::Delete;
	virtual Iterand< Mapped<_Key_,_Value_> > Delete(typename Template<_Key_>::ConstantReference key)
	{
		return UnorderedSet< Mapped<_Key_,_Value_> >::Delete(Mapped<_Key_,_Value_>(key));
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Delete(Iterand<_Key_> & iterand)
	{
		return UnorderedSet< Mapped<_Key_,_Value_> >::Delete(Mapped<_Key_,_Value_>(iterand()));
	}	

	using UnorderedSet< Mapped<_Key_,_Value_> >::Contains;
	virtual Iterand< Mapped<_Key_,_Value_> > Contains(typename Template<_Key_>::ConstantReference key)
	{
		Mapped<_Key_,_Value_> keyed(key);
		return UnorderedSet< Mapped<_Key_,_Value_> >::Select(keyed);
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Contains(Iterand<_Key_> & iterand)
	{
		return UnorderedSet< Mapped<_Key_,_Value_> >::Select(Mapped<_Key_,_Value_>(iterand()));
	}	

	void Destroy()
	{

		Reason::Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}

		ChainedHashtable< Mapped<_Key_,_Value_> >::Destroy();
	}

	void DestroyKeys()
	{
		Reason::Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
		}
	}

	void DestroyValues()
	{
		Reason::Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Key_, typename _Value_, typename _Policy_=Default< Mapped<_Key_,_Value_> > >
class OrderedMap : public OrderedSet< Mapped<_Key_,_Value_>, _Policy_ >
{
public:

	OrderedMap(const OrderedMap & map)
	{
		operator = (map);
	}

	OrderedMap(Iterable< Mapped<_Key_,_Value_> > & iterable)
	{
		operator = (iterable);
	}

	OrderedMap(Iterable<_Key_> & iterable)
	{
		operator = (iterable);
	}

	OrderedMap(int comparitor=Reason::System::Comparable::COMPARE_GENERAL):
		OrderedSet< Mapped<_Key_,_Value_> >(comparitor)
	{
	}

	~OrderedMap()
	{
	}

	using OrderedSet< Mapped<_Key_,_Value_> >::operator =;

	OrderedMap & operator = (Iterable<_Key_> & iterable)
	{
		Iterand<_Key_> iterand = iterable.Forward();
		while(iterand != 0)
		{
			OrderedSet< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(iterand()));
			++iterand;
		}

		return *this;
	}

	using OrderedSet< Mapped<_Key_,_Value_> >::Insert;

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
	{
		return OrderedSet< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(key,value));
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key)
	{
		return OrderedSet< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(key));
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(Iterand<_Key_> & iterand)
	{
		return OrderedSet< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(iterand()));
	}

	using OrderedSet< Mapped<_Key_,_Value_> >::Select;
	virtual Iterand< Mapped<_Key_,_Value_> > Select(typename Template<_Key_>::ConstantReference key)
	{
		return OrderedSet< Mapped<_Key_,_Value_> >::Select(Mapped<_Key_,_Value_>(key));
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Select(Iterand<_Key_> & iterand)
	{
		return OrderedSet< Mapped<_Key_,_Value_> >::Select(Mapped<_Key_,_Value_>(iterand()));
	}

	using OrderedSet< Mapped<_Key_,_Value_> >::Contains;
	virtual Iterand< Mapped<_Key_,_Value_> > Contains(typename Template<_Key_>::ConstantReference key)
	{

		return OrderedSet< Mapped<_Key_,_Value_> >::Select(key);
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Contains(Iterand<_Key_> & key)
	{

		return OrderedSet< Mapped<_Key_,_Value_> >::Select(key());
	}

	using OrderedSet< Mapped<_Key_,_Value_> >::Update;
	virtual Iterand< Mapped<_Key_,_Value_> > Update(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
	{

		Iterand< Mapped<_Key_,_Value_> > iterand = OrderedSet< Mapped<_Key_,_Value_> >::Select(key);
		if (iterand)
		{
			iterand().Value = value;
			return iterand;
		}
		else
		{
			return Insert(key,value);
		}	
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Update(typename Template<_Key_>::ConstantReference key)
	{
		Iterand< Mapped<_Key_,_Value_> > select = OrderedSet< Mapped<_Key_,_Value_> >::Select(key);
		if (!select)
		{
			Iterand< Mapped<_Key_,_Value_> > insert = Insert(key);
			return insert;
		}

		return select;		

	}

	virtual Iterand< Mapped<_Key_,_Value_> > Update(Iterand<_Key_> & key)
	{
		Iterand< Mapped<_Key_,_Value_> > select = OrderedSet< Mapped<_Key_,_Value_> >::Select(key());
		if (!select)
		{
			Iterand< Mapped<_Key_,_Value_> > insert = Insert(key());
			return insert;
		}

		return select;	

	}	

	using OrderedSet< Mapped<_Key_,_Value_> >::Delete;
	virtual Iterand< Mapped<_Key_,_Value_> > Delete(typename Template<_Key_>::ConstantReference key)
	{
		return OrderedSet< Mapped<_Key_,_Value_> >::Delete(Mapped<_Key_,_Value_>(key));

	}

	virtual Iterand< Mapped<_Key_,_Value_> > Delete(Iterand<_Key_> & iterand)
	{
		return OrderedSet< Mapped<_Key_,_Value_> >::Delete(Mapped<_Key_,_Value_>(iterand()));
	}		

	using OrderedSet< Mapped<_Key_,_Value_> >::Remove;
	virtual Iterand< Mapped<_Key_,_Value_> > Remove(typename Template<_Key_>::ConstantReference key)
	{

		return OrderedSet< Mapped<_Key_,_Value_> >::Remove(Mapped<_Key_,_Value_>(key));
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Remove(Iterand<_Key_> & iterand)
	{
		return OrderedSet< Mapped<_Key_,_Value_> >::Remove(Mapped<_Key_,_Value_>(iterand()));
	}		

	void Destroy()
	{

		Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}
	}

	void DestroyKeys()
	{
		Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
		}
	}

	void DestroyValues()
	{
		Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}
	}

	class KeyNode : public Variant<_Key_>
	{
	public:

		typename OrderedSet< Mapped<_Key_,_Value_> >::Node * Node;

		KeyNode(typename OrderedSet< Mapped<_Key_,_Value_> >::Node * node):Node(node)
		{		
		}

		KeyNode():Node(0)
		{		
		}

		int Compare(typename Template<_Key_>::ConstantReference type, int comparitor = Comparer<_Key_>::COMPARE_GENERAL)
		{
			return (*Node)().Key.Compare(type,comparitor);
		}

		int Compare(Variant<_Key_> & value, int comparitor = Comparer<_Key_>::COMPARE_GENERAL)
		{
			return (*Node)().Key.Compare(value,comparitor);
		}

		typename Type<_Key_>::Pointer Pointer() {return (*Node)().Key.Pointer();}
		typename Type<_Key_>::Reference Reference() {return (*Node)().Key.Reference();}
		typename Template<_Key_>::Reference operator () (void) {return (*Node)().Key();}

	};

	class KeyNodeIteration : public Iteration<_Key_>
	{
	public:

		typename OrderedSet< Mapped<_Key_,_Value_> >::NodeIteration NodeIteration;

		KeyNodeIteration(Structure::Iterable< Mapped<_Key_,_Value_> > * iterable):
			Iteration<_Key_>(0),NodeIteration(iterable)
		{

		}	

		bool Valid(Iterand<_Key_> & iterand)
		{
			KeyNode * key = (KeyNode*)&iterand;
			if (!key) return false;
			OutputAssert(iterand.Iteration == this);
			Iterand< Mapped<_Key_,_Value_> > node = key->Node;
			return NodeIteration.Valid(node);
		}

		void Forward(Iterand<_Key_> & iterand)
		{	

			Iterand< Mapped<_Key_,_Value_> > node;
			NodeIteration.Forward(node);			

			iterand.Variant(new KeyNode((typename OrderedSet< Mapped<_Key_,_Value_> >::Node *)&node));
			iterand.Count();			
			iterand.Forward();
		}

		void Reverse(Iterand<_Key_> & iterand)
		{

			Iterand< Mapped<_Key_,_Value_> > node;
			NodeIteration.Forward(node);

			iterand.Variant(new KeyNode((typename OrderedSet< Mapped<_Key_,_Value_> >::Node *)&node));
			iterand.Count();		
			iterand.Reverse();

		}

		void Move(Iterand<_Key_> & iterand, int amount)
		{	

			KeyNode * key = (KeyNode*)&iterand;

			Iterand< Mapped<_Key_,_Value_> > node = key->Node;
			node.Option = (iterand.Option&Iterand<_Key_>::MODE);
			NodeIteration.Move(node,amount);

			if (node)
				key->Node = (typename OrderedSet< Mapped<_Key_,_Value_> >::Node *)&node;
			else 
				iterand = Iterand<_Key_>();
		}

	};

	Reason::Structure::Iterator<_Key_> Keys()
	{
		return Structure::Iterator<_Key_>(new KeyNodeIteration(this));
	}

	class ValueNode : public Variant<_Value_>
	{
	public:

		typename OrderedSet< Mapped<_Key_,_Value_> >::Node * Node;

		ValueNode(typename OrderedSet< Mapped<_Key_,_Value_> >::Node * node):Node(node)
		{		
		}

		ValueNode():Node(0)
		{		
		}

		int Compare(typename Template<_Value_>::ConstantReference type, int comparitor = Comparer<_Value_>::COMPARE_GENERAL)
		{
			return (*Node)().Value.Compare(type,comparitor);
		}

		int Compare(Variant<_Value_> & value, int comparitor = Comparer<_Value_>::COMPARE_GENERAL)
		{
			return (*Node)().Value.Compare(value,comparitor);
		}

		typename Type<_Value_>::Pointer Pointer() {return (*Node)().Value.Pointer();}
		typename Type<_Value_>::Reference Reference() {return (*Node)().Value.Reference();}
		typename Template<_Value_>::Reference operator () (void) {return (*Node)().Value();}

	};

	class ValueNodeIteration : public Iteration<_Value_>
	{
	public:

		typename OrderedSet< Mapped<_Key_,_Value_> >::NodeIteration NodeIteration;

		ValueNodeIteration(Structure::Iterable< Mapped<_Key_,_Value_> > * iterable):
			Iteration<_Value_>(0),NodeIteration(iterable)
		{
		}

		bool Valid(Iterand<_Value_> & iterand)
		{
			ValueNode * value = (ValueNode*)&iterand;
			if (!value) return false;
			OutputAssert(iterand.Iteration == this);
			Iterand< Mapped<_Key_,_Value_> > node = value->Node;
			return NodeIteration.Valid(node);
		}

		void Forward(Iterand<_Value_> & iterand)
		{	
			Iterand< Mapped<_Key_,_Value_> > node;
			NodeIteration.Forward(node);

			iterand.Variant(new ValueNode((typename OrderedSet< Mapped<_Key_,_Value_> >::Node *)&node));
			iterand.Count();
			iterand.Forward();
		}

		void Reverse(Iterand<_Value_> & iterand)
		{
			Iterand< Mapped<_Key_,_Value_> > node;
			NodeIteration.Forward(node);

			iterand.Variant(new ValueNode((typename OrderedSet< Mapped<_Key_,_Value_> >::Node *)&node));
			iterand.Count();		
			iterand.Reverse();
		}

		void Move(Iterand<_Value_> & iterand, int amount)
		{	
			ValueNode * value = (ValueNode*)&iterand;

			Iterand< Mapped<_Key_,_Value_> > node = value->Node;
			node.Option = (iterand.Option&Iterand<_Key_>::MODE);
			NodeIteration.Move(node,amount);

			if (node)
				value->Node = (typename OrderedSet< Mapped<_Key_,_Value_> >::Node *)&node;
			else 
				iterand = Iterand<_Value_>();
		}

	};

	Reason::Structure::Iterator<_Value_> Values()
	{
		return Structure::Iterator<_Value_>(new ValueNodeIteration(this));
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class UnorderedMultimap: public ChainedHashtable<_Kind_>
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class OrderedMultimap
{
public:

};

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

template <typename _First_, typename _Last_>
class Range
{
public:

	_First_ First;
	_Last_ Last;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _First_, typename _Second_, typename _Third_,typename _Fourth_=None, typename _Fifth_=None>
class Tuple
{
public:

	Variable<_First_> First;
	Variable<_Second_> Second;
	Variable<_Third_> Third;
	Variable<_Fourth_> Fourth;
	Variable<_Fifth_> Fifth;

};

template <typename _First_, typename _Second_, typename _Third_>
class Tuple<_First_,_Second_,_Third_>
{
public:

	Variable<_First_> First;
	Variable<_Second_> Second;
	Variable<_Third_> Third;

};

template <typename _First_, typename _Second_, typename _Third_, typename _Fourth_>
class Tuple<_First_,_Second_,_Third_,_Fourth_>
{
public:

	Variable<_First_> First;
	Variable<_Second_> Second;
	Variable<_Third_> Third;
	Variable<_Fourth_> Fourth;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _First_, typename _Second_> class Pair: public Object
{
public:

	static Identity Instance;
	virtual Identity& Identify(){return Instance;};

public:
	Variable<_First_> First;
	Variable<_Second_> Second;

	Pair()
	{
	}

	Pair(const Pair<_First_,_Second_> & pair):First(pair.First),Second(pair.Second)
	{
	}

	Pair(typename Template<_First_>::ConstantReference first):First(first)
	{
	}

	Pair(typename Template<_First_>::ConstantReference first, typename Template<_Second_>::ConstantReference second):First(first),Second(second)
	{
	}

	~Pair()
	{
	}

	int Compare(Object * object, int comparitor = Comparable::COMPARE_GENERAL)
	{	
		if (object->InstanceOf(this))
		{
			Pair<_First_,_Second_> * pair = (Pair<_First_,_Second_> *)object;
			int result = Reason::Structure::Comparer<_First_>::Compare(this->First(),pair->First(),comparitor);
			if (result == 0)
				return Reason::Structure::Comparer<_Second_>::Compare(this->Second(),pair->Second(),comparitor);
			else 
				return result;
		}
		return Identity::Error;
	}

	int Hash()
	{

		int hash = (Reason::Structure::Hashable<_First_>::Hash(this->First())<<8) ^ Reason::Structure::Hashable<_Second_>::Hash(this->Second());
		return hash;
	}
};

template <typename _First_, typename _Second_> 
Identity Pair<_First_,_Second_>::Instance;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Key_, typename _Value_, typename _Policy_=Default< Mapped<_Key_,_Value_> > > 
class Map: public Abstract::OrderedMap<_Key_,_Value_,_Policy_>
{
public:

	typedef typename Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Node Node;

	static Variable<_Value_> Null;

public:

	Map(const Map & map)
	{
		operator = (map);
	}

	Map(Iterable< Mapped<_Key_,_Value_> > & iterable)
	{
		operator = (iterable);
	}

	Map(Iterable<_Key_> & iterable)
	{
		operator = (iterable);
	}

	Map(int comparitor=Reason::System::Comparable::COMPARE_GENERAL):
		Abstract::OrderedMap<_Key_,_Value_>(comparitor)
	{

	}

	~Map()
	{
	}

	using Abstract::OrderedMap<_Key_,_Value_>::Insert;
	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
	{
		return Abstract::OrderedMap<_Key_,_Value_>::Insert(Mapped<_Key_,_Value_>(key,value));
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key)
	{
		return Abstract::OrderedMap<_Key_,_Value_>::Insert(Mapped<_Key_,_Value_>(key));
	}

	using Abstract::OrderedMap<_Key_,_Value_>::Update;
	virtual Iterand< Mapped<_Key_,_Value_> > Update(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
	{
		return Abstract::OrderedMap<_Key_,_Value_>::Update(key,value);
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Update(typename Template<_Key_>::ConstantReference key)
	{
		return Abstract::OrderedMap<_Key_,_Value_>::Update(key);
	}

	using Abstract::OrderedMap<_Key_,_Value_>::Delete;
	virtual Iterand< Mapped<_Key_,_Value_> > Delete(typename Template<_Key_>::ConstantReference key)
	{

		return Abstract::OrderedMap<_Key_,_Value_>::Delete(key);

	}

	using Abstract::OrderedMap<_Key_,_Value_>::Remove;
	virtual Iterand< Mapped<_Key_,_Value_> > Remove(typename Template<_Key_>::ConstantReference key)
	{

		return Abstract::OrderedMap<_Key_,_Value_>::Remove(key);
	}

	using Abstract::OrderedMap<_Key_,_Value_>::operator =;

	typename Template<_Value_>::Reference operator [] (typename Template<_Key_>::ConstantReference key)
	{

		Iterand< Mapped<_Key_,_Value_> > select = Select(key);
		if (select)
			return select().Value();

		return Null();
	}

	void Destroy()
	{

		Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}

		Abstract::OrderedMap<_Key_,_Value_>::Destroy();
	}

	void DestroyKeys()
	{
		Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
		}
	}

	void DestroyValues()
	{
		Structure::Iterator< Mapped<_Key_,_Value_> > iterator = this->Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}
	}	

};

template <typename _Key_, typename _Value_, typename _Policy_> 
Variable<_Value_> Map<_Key_,_Value_,_Policy_>::Null;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Key_, typename _Value_, typename _Policy_=Default< Mapped<_Key_,_Value_> > > 
class Multimap : public Abstract::RedBlackTree< Mapped<_Key_,_Value_>, _Policy_>
{
public:

	typedef typename Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Node Node;
	typedef typename Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::NodeIteration NodeIteration;

	Multimap(const Multimap & map)
	{
		operator = (map);
	}

	Multimap(Iterable< Mapped<_Key_,_Value_> > & iterable)
	{
		operator = (iterable);
	}

	Multimap(Iterable<_Key_> & iterable)
	{
		operator = (iterable);
	}

	Multimap(int comparitor=Reason::System::Comparable::COMPARE_GENERAL):
		Abstract::RedBlackTree< Mapped<_Key_,_Value_> >(comparitor)
	{
	}

	~Multimap()
	{
	}

	Multimap & operator = (const Multimap & map)
	{
		return Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::operator = (map);
	}

	Multimap & operator = (Iterable< Mapped<_Key_,_Value_> > & iterable)
	{
		return Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::operator = (iterable);
	}

	Multimap & operator = (Iterable<_Key_> & iterable)
	{
		Iterand<_Key_> iterand = iterable.Forward();
		while(iterand != 0)
		{
			Abstract::RedBlackTree<Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(iterand()),false);
			++iterand;
		}
		return *this;
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
	{
		return Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(key,value),false);
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key)
	{
		return Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(key),false);
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Remove(typename Template<_Key_>::ConstantReference key)
	{

		return Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Remove(key);
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Contains(typename Template<_Key_>::ConstantReference key)
	{

		return Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Select(key);
	}

	typename Template<_Value_>::Value operator [] (typename Template<_Key_>::ConstantReference key)
	{

		Reason::Structure::Iterand<Mapped<_Key_,_Value_> > select = Select(key);
		if (select)
		{
			return select().Value();
		}
		else
		{

			return Variable<_Value_>()::Null();
		}
	}

	void Destroy()
	{

		typename Multimap<_Key_,_Value_>::Iterator iterator = Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}

		Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Destroy();
	}

	void DestroyKeys()
	{

		typename Multimap<_Key_,_Value_>::Iterator iterator = Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Key_>::Destroy(iterator().Key());
		}
	}

	void DestroyValues()
	{

		typename Multimap<_Key_,_Value_>::Iterator iterator = Iterate();
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			Reason::Structure::Disposer<_Value_>::Destroy(iterator().Value());
		}
	}

	using Abstract::RedBlackTree< Mapped<_Key_,_Value_> >::Iterate;

	Structure::Iterator< Mapped<_Key_,_Value_> > Iterate(typename Template<_Key_>::ConstantReference key)
	{	
		NodeIteration * iteration = new NodeIteration(this);
		Iterand< Mapped<_Key_,_Value_> > iterand(iteration);

		Mapped<_Key_,_Value_> mapped(key);

		Iterand< Mapped<_Key_,_Value_> > minimum = this->Minimum();

		Iterand< Mapped<_Key_,_Value_> > select = this->Select(minimum,mapped);

		iterand.Forward();

		Node * node = (Node*)&select;

		iteration->First = node;

		iterand.Variant(node);
		while(iterand)
		{
			if (iterand().Compare(&mapped) != 0)
				break;

			node = (Node*)&iterand;
			++iterand;
		}

		iteration->Last = node;

		Structure::Iterator< Mapped<_Key_,_Value_> > iterator(iterand);

		return iterator; 
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Key_, typename _Value_, typename _Policy_=Default< Mapped<_Key_,_Value_> > >
class Hashmap : public Abstract::UnorderedMap<_Key_,_Value_,_Policy_>
{
public:

	Hashmap(const Hashmap & map)
	{
		operator = (map);
	}

	Hashmap(Iterable< Mapped<_Key_,_Value_> > & iterable)
	{
		operator = (iterable);
	}

	Hashmap(Iterable<_Key_> & iterable)
	{
		operator = (iterable);
	}

	Hashmap(int comparitor=Reason::System::Comparable::COMPARE_GENERAL):Abstract::UnorderedMap<_Key_, _Value_>(comparitor)
	{
	}

	~Hashmap()
	{
	}

	using Abstract::UnorderedMap<_Key_,_Value_>::operator =;

	typename Template<_Value_>::Value operator [] (typename Template<_Key_>::ConstantReference key)
	{
		typename Abstract::ChainedHashtable< Mapped<_Key_,_Value_> >::Entry * entry = (typename Abstract::ChainedHashtable< Mapped<_Key_,_Value_> >::Entry *) &Select(key);
		if (entry)
		{
			return (*entry)().Value();
		}
		else
		{
			return Variable<_Value_>()();
		}
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Key_, typename _Value_, typename _Policy_=Default< Mapped<_Key_,_Value_> > >
class Arraymap : public Arrayset< Mapped<_Key_,_Value_>,_Policy_ >
{
public:

	using Arrayset< Mapped<_Key_,_Value_> >::Insert;
	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
	{
		return Arrayset< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(key,value));
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Insert(typename Template<_Key_>::ConstantReference key)
	{
		return Arrayset< Mapped<_Key_,_Value_> >::Insert(Mapped<_Key_,_Value_>(key));
	}

	using Arrayset< Mapped<_Key_,_Value_> >::Update;
	virtual Iterand< Mapped<_Key_,_Value_> > Update(typename Template<_Key_>::ConstantReference key, typename Template<_Value_>::ConstantReference value)
	{
		return Arrayset< Mapped<_Key_,_Value_> >::Update(Mapped<_Key_,_Value_>(key,value));
	}

	virtual Iterand< Mapped<_Key_,_Value_> > Contains(typename Template<_Key_>::ConstantReference key)
	{
		Mapped<_Key_,_Value_> keyed(key);
		return Arrayset< Mapped<_Key_,_Value_> >::Select(keyed);
	}

	typename Template<_Value_>::Value operator [] (typename Template<_Key_>::ConstantReference key)
	{

		Reason::Structure::Iterand< Reason::Structure::Mapped<_Key_,_Value_> > iterand = Select(key);
		if (iterand)
			return iterand().Value();
		else
		{

			return Variable<_Value_>()();
		}
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

