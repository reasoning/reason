
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
#ifndef STRUCTURE_POLICY_H
#define STRUCTURE_POLICY_H

#pragma warning(disable: 4503)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/generic/generic.h" 
#include "reason/system/interface.h" 
#include "reason/messaging/callback.h"
#include "reason/structure/disposable.h"

using namespace Reason::Messaging;
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

namespace Policy {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Options
{
public:

	short Option;

	Options(int option):Option(option)
	{

	}

	Options():Option(0)
	{

	}

	int operator = (int option)
	{
		Option = option;
	}

	int operator () ()
	{
		return Option;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
class Hash 
{
public:

	virtual int operator () (typename Reason::Generic::Template<_Kind_>::ConstantReference kind)
	{
		return Reason::Structure::Hashable<_Kind_>::Hash(kind);
	}

	virtual int operator () (Variant<_Kind_> * var)
	{
		return operator() ((*var)());
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
class Compare : public Options
{
public:

	struct CompareGeneral {};
	struct ComparePrecise {};
	struct CompareInstance {};

	Compare(int option=Comparable::COMPARE_GENERAL):Options(option)
	{

	}

	virtual int operator () (typename Reason::Generic::Template<_Kind_>::ConstantReference left, typename Reason::Generic::Template<_Kind_>::ConstantReference right, int compare)
	{
		return Reason::Structure::Comparer<_Kind_>::Compare(left,right,(compare==Comparable::COMPARE_POLICY)?Option:compare);
	}

	virtual int operator () (typename Reason::Generic::Template<_Kind_>::ConstantReference left, typename Reason::Generic::Template<_Kind_>::ConstantReference right)
	{
		return Reason::Structure::Comparer<_Kind_>::Compare(left,right,Option);
	}

	virtual int operator () (Variant<_Kind_> * left, Variant<_Kind_> * right, int compare)
	{
		return operator() ((*left)(),(*right)(),compare);
	}

	virtual int operator () (Variant<_Kind_> * left, Variant<_Kind_> * right)
	{
		return operator() ((*left)(),(*right)());
	}

};

template<typename _Kind_>
class CompareGeneral : public Compare<_Kind_>
{
public:	

	int operator () (typename Reason::Generic::Template<_Kind_>::ConstantReference left, typename Reason::Generic::Template<_Kind_>::ConstantReference right)
	{
		return Reason::Structure::Comparer<_Kind_>::Compare(left,right,Comparable::COMPARE_GENERAL);
	}	
};

template<typename _Kind_>
class ComparePrecise : public Compare<_Kind_>
{
public:	

	int operator () (typename Reason::Generic::Template<_Kind_>::ConstantReference left, typename Reason::Generic::Template<_Kind_>::ConstantReference right)
	{
		return Reason::Structure::Comparer<_Kind_>::Compare(left,right,Comparable::COMPARE_PRECISE);
	}	
};

template<typename _Kind_>
class CompareInstance : public Compare<_Kind_>
{
public:	

	int operator () (typename Reason::Generic::Template<_Kind_>::ConstantReference left, typename Reason::Generic::Template<_Kind_>::ConstantReference right)
	{
		return Reason::Structure::Comparer<_Kind_>::Compare(left,right,Comparable::COMPARE_INSTANCE);
	}	
};

template<typename _Kind_>
class CompareCallback : public Compare<_Kind_>
{
public:

	Reason::Messaging::Callback<int,_Kind_,_Kind_,int> Callback;

	int operator () (typename Reason::Generic::Template<_Kind_>::ConstantReference left, typename Reason::Generic::Template<_Kind_>::ConstantReference right, int compare)
	{
		if (!Callback) return 0;

		return Callback(left,right,compare);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
class Dispose : public Options
{
public:

	struct DisposeNone {};
	struct DisposePointer {};
	struct DisposeArray {};
	struct DisposeRelease {};
	struct DispiseDestroy {};

	Dispose(int option=Disposable::DISPOSE_RELEASE):Options(option)
	{

	}	

	virtual void operator() (typename Reason::Generic::Template<_Kind_>::Reference kind)
	{
		switch(Option)
		{
			case Disposable::DISPOSE_RELEASE: Reason::Structure::Disposer<_Kind_>::Release(kind); break;
			case Disposable::DISPOSE_DESTROY: Reason::Structure::Disposer<_Kind_>::Destroy(kind); break;			
		}
	}

	virtual void operator() (typename Reason::Generic::Template<_Kind_>::Reference kind, int dispose)
	{
		switch(dispose)
		{
			case Disposable::DISPOSE_RELEASE: Reason::Structure::Disposer<_Kind_>::Release(kind); break;
			case Disposable::DISPOSE_DESTROY: Reason::Structure::Disposer<_Kind_>::Destroy(kind); break;			
		}
	}

};

template<typename _Kind_>
class DisposeRelease : public Dispose<_Kind_>
{
public:

	DisposeRelease():Dispose<_Kind_>(Disposable::DISPOSE_RELEASE){}

	virtual void operator() (typename Reason::Generic::Template<_Kind_>::Reference kind)
	{
		Reason::Structure::Disposer<_Kind_>::Release(kind);
	}

};

template<typename _Kind_>
class DisposeDestroy : public Dispose<_Kind_>
{
public:

	DisposeDestroy():Dispose<_Kind_>(Disposable::DISPOSE_DESTROY){}

	virtual void operator() (typename Reason::Generic::Template<_Kind_>::Reference kind)
	{
		Reason::Structure::Disposer<_Kind_>::Destroy(kind);
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
class Allocate : public Options
{
public:

	Allocate(int option):Options(option)
	{

	}

	virtual int operator() ()
	{
		return 0;		
	}

	template<typename _Alloc_>
	void * New()
	{
		char * data = New(sizeof(_Alloc_));
		return (void*) data;
	}

	void * New(int size)
	{
		char * data = new char[size];
		return (void*)data;
	}

	template<typename _Alloc_>
	void Del(void * data)
	{
		delete (_Alloc_*)data;
	}	

	void Del(void * data)
	{
		delete data;
	}	

	void Del(void * data, int size)
	{
		delete data;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Order : public Options
{
public:

	Order(int option=Orderable::ORDER_DEFAULT):Options(option) {}

	virtual int operator()()
	{
		return this->Option;
	}

};

class OrderAscending : public Order
{
public:
	OrderAscending():Order(Orderable::ORDER_ASCENDING) {}	

	int operator() ()
	{
		return Orderable::ORDER_ASCENDING;
	}
};

class OrderDescending : public Order
{
public:
	OrderDescending():Order(Orderable::ORDER_DESCENDING) {}	

	int operator() ()
	{
		return Orderable::ORDER_DESCENDING;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
class Base
{
public:	

	Policy::Order & Order;

	Policy::Compare<_Kind_> & Compare;
	Policy::Dispose<_Kind_> & Dispose;
	Policy::Allocate<_Kind_> & Allocate;

	int Option;

	Base(Policy::Order & order, Policy::Compare<_Kind_> & compare, Policy::Dispose<_Kind_> & dispose, Policy::Allocate<_Kind_> & allocate, int option=0):
		Order(order),Compare(compare),Dispose(dispose),Allocate(allocate),Option(option)
	{

	}

	virtual ~Base()
	{

	}

};

template<typename _Kind_, typename _Compare_=Compare<_Kind_>, typename _Dispose_=Dispose<_Kind_>, typename _Allocate_=Allocate<_Kind_> > 
class Default : public Base<_Kind_>, private Order, private _Compare_, private _Dispose_, private _Allocate_
{
public:

	Default(int order=Orderable::ORDER_DEFAULT, int compare=Comparable::COMPARE_GENERAL, int dispose=Disposable::DISPOSE_RELEASE, int allocate=0, int option=0):
	  Base<_Kind_>((Policy::Order&)*this,(_Compare_&)*this, (_Dispose_&)*this, (_Allocate_&)*this, option),
		Order(order),_Compare_(compare),_Dispose_(dispose),_Allocate_(allocate)
	{

	}

	Default(const Default & def):
		Base<_Kind_>((Order&)*this,(_Compare_&)*this,(_Dispose_&)*this,(_Allocate_&)*this,def.Option),
		Order(def.Order.Option),_Compare_(def.Compare.Option),_Dispose_(def.Dispose.Option),_Allocate_(def.Allocate.Option)
	{

	}

	Default(const Base<_Kind_> & base):
		Base<_Kind_>((Order&)*this,(_Compare_&)*this,(_Dispose_&)*this,(_Allocate_&)*this,base.Option),
		Order(base.Order.Option),_Compare_(base.Compare.Option),_Dispose_(base.Dispose.Option),_Allocate_(base.Allocate.Option)
	{

	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_, typename _Compare_=Compare<_Kind_>, typename _Dispose_=Dispose<_Kind_>, typename _Allocate_=Allocate<_Kind_> > 
class Ascending : public Base<_Kind_>, private Order, private _Compare_, private _Dispose_, private _Allocate_
{
public:

	Ascending(int order=Orderable::ORDER_ASCENDING, int compare=Comparable::COMPARE_GENERAL, int dispose=Disposable::DISPOSE_RELEASE, int allocate=0, int option=0):
	  Base<_Kind_>((Policy::Order&)*this,(_Compare_&)*this, (_Dispose_&)*this, (_Allocate_&)*this, option),
		Order(order),_Compare_(compare),_Dispose_(dispose),_Allocate_(allocate)
	{

	}

	Ascending(const Default<_Kind_> & policy):
		Base<_Kind_>((Order&)*this,(_Compare_&)*this,(_Dispose_&)*this,(_Allocate_&)*this,policy.Option),
		Order(policy.Order.Option),_Compare_(policy.Compare.Option),_Dispose_(policy.Dispose.Option),_Allocate_(policy.Allocate.Option)
	{

	}

	Ascending(const Base<_Kind_> & base):
		Base<_Kind_>((Order&)*this,(_Compare_&)*this,(_Dispose_&)*this,(_Allocate_&)*this,base.Option),
		Order(base.Order.Option),_Compare_(base.Compare.Option),_Dispose_(base.Dispose.Option),_Allocate_(base.Allocate.Option)
	{

	}

};

template<typename _Kind_, typename _Compare_=Compare<_Kind_>, typename _Dispose_=Dispose<_Kind_>, typename _Allocate_=Allocate<_Kind_> > 
class Descending : public Base<_Kind_>, private Order, private _Compare_, private _Dispose_, private _Allocate_
{
public:

	Descending(int order=Orderable::ORDER_DESCENDING, int compare=Comparable::COMPARE_GENERAL, int dispose=Disposable::DISPOSE_RELEASE, int allocate=0, int option=0):
		Base<_Kind_>((Order&)*this,(_Compare_&)*this, (_Dispose_&)*this, (_Allocate_&)*this, option),
		Order(order),_Compare_(compare),_Dispose_(dispose),_Allocate_(allocate)
	{

	}

	Descending(const Default<_Kind_> & policy):
		Base<_Kind_>((Order&)*this,(_Compare_&)*this,(_Dispose_&)*this,(_Allocate_&)*this,policy.Option),
		Order(policy.Order.Option),_Compare_(policy.Compare.Option),_Dispose_(policy.Dispose.Option),_Allocate_(policy.Allocate.Option)
	{

	}

	Descending(const Base<_Kind_> & base):
		Base<_Kind_>((Order&)*this,(_Compare_&)*this,(_Dispose_&)*this,(_Allocate_&)*this,base.Option),
		Order(base.Order.Option),_Compare_(base.Compare.Option),_Dispose_(base.Dispose.Option),_Allocate_(base.Allocate.Option)
	{

	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
