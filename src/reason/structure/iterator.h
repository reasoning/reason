
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
#ifndef STRUCTURE_ITERATOR_H
#define STRUCTURE_ITERATOR_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/reason.h"
#include "reason/generic/generic.h"
#include "reason/messaging/callback.h"

#include "reason/structure/disposable.h"
#include "reason/structure/comparable.h"
#include "reason/structure/policy.h"

using namespace Reason::Structure;
using namespace Reason::Structure::Policy;
using namespace Reason::Generic;
using namespace Reason::Messaging;

#ifdef REASON_USING_STL
#include <iterator>
#endif

#pragma warning(disable: 4715)
#pragma warning(disable: 4800)

namespace Reason { namespace Structure {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Iteration;

template <typename _Kind_>
class Iterator;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Iterand
{
public:

	#ifdef REASON_USING_STL

	#endif

	enum IterandType
	{
		TYPE_NULL		=0,
		TYPE_KIND		=1,
		TYPE_VARIANT	=2,
		TYPE_INDEX		=4,
	};

	enum IterandOptions
	{

		STATE_INVALID	=1<<(1),
		STATE_UNIQUE	=1<<(2),
		STATE_ERROR		=0x8000,	

		STATE = STATE_INVALID|STATE_UNIQUE|STATE_ERROR,

		MODE_FORWARD	=1<<(3),
		MODE_REVERSE	=1<<(4),
		MODE_PUT		=1<<(5),
		MODE_GET		=1<<(6),

		MODE = MODE_FORWARD|MODE_REVERSE|MODE_PUT|MODE_GET,

		PROXY_COUNTED	=1<<(7),
		PROXY_DISPOSE	=1<<(8),
		PROXY_CALLBACK	=1<<(9),
		PROXY = PROXY_COUNTED|PROXY_DISPOSE|PROXY_CALLBACK,

	};

	union IterandUnion
	{

		IterandUnion(Reason::Generic::Variant<_Kind_> * variant):Variant(variant) {}
		IterandUnion(typename Reason::Generic::Template<_Kind_>::Pointer kind):Kind(kind) {}
		IterandUnion():Kind(0) {}

		Reason::Generic::Variable<_Kind_> * Variable;
		Reason::Generic::Variant<_Kind_> * Variant;
		typename Reason::Generic::Template<_Kind_>::Pointer Kind;
		int Index;

		operator int () {return (int)Variant & (int)Kind;}

	};

	struct IterandProxy
	{
		short Dispose;
		short References;

		IterandProxy(int dispose=0):Dispose(dispose),References(0) {}

		virtual void Zero(Iterand<_Kind_> * iterand)
		{
		}
	};

	struct IterandCounted : public IterandProxy
	{
		IterandCounted(int dispose=Disposable::DISPOSE_POINTER):IterandProxy(dispose) {}

		virtual void Zero(Iterand<_Kind_> * iterand)
		{
			if (Dispose==Disposable::DISPOSE_POINTER)
			{
				switch(iterand->Type)
				{
					case TYPE_KIND:
					if (iterand->Union.Kind)
					{
						delete iterand->Union.Kind;
						iterand->Union.Kind = 0;
						iterand->Type = iterand->Option = 0;
					}
					break;
					case TYPE_VARIANT:
					if (iterand->Union.Variant)
					{
						delete iterand->Union.Variant;
						iterand->Union.Variant = 0;
						iterand->Type = iterand->Option = 0;
					}
					break;
				}
			}
			else
			{
				switch((int)this->Dispose)
				{
					case Disposable::DISPOSE_DESTROY:Iterand<_Kind_>::Destroy(*iterand);break;
					case Disposable::DISPOSE_RELEASE:Iterand<_Kind_>::Release(*iterand);break;
				}
			}
		}
	};

	struct IterandDisposable : public IterandProxy
	{
		IterandDisposable(int dispose=Disposable::DISPOSE_RELEASE):IterandProxy(dispose)
		{				
		}

		void Zero(Iterand<_Kind_> * iterand) 
		{

			Reason::Structure::Iteration<_Kind_> * iteration = (iterand->Iteration)?iterand->Iteration():0;

			if (iteration && iteration->Iterable)
			{

				Iterand<_Kind_>::IterandProxy * proxy = iterand->Proxy;
				iterand->Proxy = 0;

				switch((int)this->Dispose)
				{

					case Disposable::DISPOSE_DESTROY:iteration->Iterable->Destroy(*iterand);break;
					case Disposable::DISPOSE_RELEASE:iteration->Iterable->Release(*iterand);break;					

					case Disposable::DISPOSE_POLICY:iteration->Iterable->Dispose(*iterand);break;

				}

				iterand->Proxy = proxy;
			}	
			else
			{
				switch((int)this->Dispose)
				{
					case Disposable::DISPOSE_DESTROY:Iterand<_Kind_>::Destroy(*iterand);break;
					case Disposable::DISPOSE_RELEASE:Iterand<_Kind_>::Release(*iterand);break;
				}
			}	
		}		
	};

	struct IterandCallback : public IterandProxy
	{
		Reason::Messaging::Callback<void,Iterand<_Kind_>*> Callback;

		void Zero(Iterand<_Kind_> * iterand) 
		{
			Callback(iterand);
		}		
	};		

	short Type;
	short Option;

	IterandUnion Union;

	IterandProxy * Proxy;

	Strong< Reason::Structure::Iteration<_Kind_> * > Iteration;

	int Move() 
	{
		return (Option&MODE_REVERSE)?(-1):((Option&MODE_FORWARD)?1:0);
	}

	void Increment()
	{
		if (Proxy)
			++Proxy->References;
	}

	void Decrement()
	{
		if (Proxy)
		{
			--Proxy->References;
			if (Proxy->References == 0)
			{
				Proxy->Zero(this);
				delete Proxy;
				Proxy = 0;
			}
		}
	}

	static Iterand<_Kind_> Put(typename Template<_Kind_>::ConstantPointer kind)
	{
		return Iterand<_Kind_>(kind,MODE_PUT);
	}

	static Iterand<_Kind_> Put(Variant<_Kind_> * variant)
	{
		return Iterand<_Kind_>(variant,MODE_PUT);
	}

	static Iterand<_Kind_> Get(typename Template<_Kind_>::ConstantPointer kind)
	{
		return Iterand<_Kind_>(kind,MODE_GET);
	}

	static Iterand<_Kind_> Get(Variant<_Kind_> * variant)
	{
		return Iterand<_Kind_>(variant,MODE_GET);
	}

	static Iterand<_Kind_> Disposed(const Strong< Reason::Structure::Iteration<_Kind_> * > & iteration, typename Template<_Kind_>::ConstantReference kind, int dispose=Disposable::DISPOSE_RELEASE)
	{
		Iterand<_Kind_> & iterand = Disposed(kind,dispose);
		iterand.Iteration = iteration;
		return iterand;
	}

	static Iterand<_Kind_> Disposed(typename Template<_Kind_>::ConstantReference kind, int dispose=Disposable::DISPOSE_RELEASE)
	{
		Iterand<_Kind_> iterand = new typename Reason::Generic::Template<_Kind_>::Value(Reason::Generic::Reference<_Kind_>::Template(kind));
		iterand.Option |= PROXY_DISPOSE;
		iterand.Proxy = new IterandDisposable(dispose);
		iterand.Increment();
		return iterand;
	}

	static Iterand<_Kind_> Disposed(const Strong< Reason::Structure::Iteration<_Kind_> * > & iteration, typename Template<_Kind_>::ConstantPointer kind, int dispose=Disposable::DISPOSE_RELEASE)	
	{
		Iterand<_Kind_> & iterand = Disposed(kind,dispose);
		iterand.Iteration = iteration;
		return iterand;
	}

	static Iterand<_Kind_> Disposed(typename Template<_Kind_>::ConstantPointer kind, int dispose=Disposable::DISPOSE_RELEASE)
	{
		Iterand<_Kind_> iterand = kind;
		iterand.Option |= PROXY_DISPOSE;
		iterand.Proxy = new IterandDisposable(dispose);
		iterand.Increment();
		return iterand;
	}

	static Iterand<_Kind_> Disposed(const Strong< Reason::Structure::Iteration<_Kind_> * > & iteration, Variant<_Kind_> * variant, int dispose=Disposable::DISPOSE_RELEASE)
	{
		Iterand<_Kind_> & iterand = Disposed(variant,dispose);
		iterand.Iteration = iteration;
		return iterand;
	}	

	static Iterand<_Kind_> Disposed(Variant<_Kind_> * variant, int dispose=Disposable::DISPOSE_RELEASE)
	{
		Iterand<_Kind_> iterand = variant;
		iterand.Option |= PROXY_DISPOSE;
		iterand.Proxy = new IterandDisposable(dispose);
		iterand.Increment();
		return iterand;
	}

	static Iterand<_Kind_> Counted(typename Template<_Kind_>::ConstantReference kind)
	{
		Iterand<_Kind_> iterand = new typename Reason::Generic::Template<_Kind_>::Value(Reason::Generic::Reference<_Kind_>::Template(kind));
		iterand.Option |= PROXY_COUNTED;
		iterand.Proxy = new IterandCounted();
		iterand.Increment();
		return iterand;
	}

	static Iterand<_Kind_> Counted(typename Template<_Kind_>::ConstantPointer kind)
	{			
		Iterand<_Kind_> iterand = kind;
		iterand.Option |= PROXY_COUNTED;
		iterand.Proxy = new IterandCounted();
		iterand.Increment();
		return iterand;
	}

	static Iterand<_Kind_> Counted(Variant<_Kind_> * variant)
	{
		Iterand<_Kind_> iterand = variant;
		iterand.Option |= PROXY_COUNTED;
		iterand.Proxy = new IterandCounted();
		iterand.Increment();
		return iterand;
	}

	static Iterand<_Kind_> Indexed(int index)
	{
		Iterand<_Kind_> iterand;
		iterand.Type = TYPE_INDEX;
		iterand.Union.Index = index;
		return iterand;
	}

	static Iterand<_Kind_> Null()
	{
		return Iterand<_Kind_>();
	}

	void Release()
	{
		Dispose(Disposable::DISPOSE_RELEASE);
	}

	void Destroy()
	{
		Dispose(Disposable::DISPOSE_DESTROY);
	}

	Iterand<_Kind_> & Dispose(int dispose=Disposable::DISPOSE_RELEASE)
	{
		if ((Type&TYPE_INDEX)) return *this;

		if ((Option&PROXY_DISPOSE))
		{	

			((IterandDisposable*)Proxy)->Dispose = dispose;
			return *this;
		}

		OutputAssert(Proxy == 0 || Proxy->References == 1);
		IterandUnion uni = Union;
		Union.Variant = 0;
		Option |= PROXY_DISPOSE;
		Decrement();
		Proxy = new IterandDisposable(dispose);
		Increment();
		Union = uni;
		return *this;
	}

	Iterand<_Kind_> & Count()
	{
		if (!(Option&PROXY_COUNTED) && !(Type&TYPE_INDEX))
		{
			OutputAssert(Proxy == 0 || Proxy->References == 1);
			IterandUnion uni = Union;
			Union.Variant = 0;
			Option |= PROXY_COUNTED;			
			Decrement();
			Proxy = new IterandCounted();
			Increment();
			Union = uni;
		}

		return *this;
	}

	Iterand<_Kind_> & Forward() 
	{
		Option &= ~MODE;
		Option |= MODE_FORWARD;
		return *this;
	}

	Iterand<_Kind_> & Reverse() 
	{
		Option &= ~MODE;
		Option |= MODE_REVERSE;
		return *this;
	}

	Iterand<_Kind_> & Invalid() 
	{	

		Option |= STATE_INVALID;
		return *this;
	}

	Iterand<_Kind_> & Unique() 
	{
		Option |= STATE_UNIQUE;
		return *this;
	}

	Iterand<_Kind_> & Error() 
	{
		Option |= STATE_ERROR;
		return *this;
	}

	Iterand<_Kind_> & Iterate(const Strong< Reason::Structure::Iteration<_Kind_> * > & iteration)
	{
		Iteration = iteration;
		return *this;
	}

	Iterand<_Kind_> & Iterate(Reason::Structure::Iteration<_Kind_> * iteration)
	{
		Iteration = iteration;
		return *this;
	}

	Iterand<_Kind_> & Index(int index)
	{
		Decrement();
		Proxy = 0;

		if (Iteration) 
			Option &= ~(PROXY|STATE);
		else 
			Option &= ~(STATE);

		Type = TYPE_INDEX;
		Union.Index = index;
		return *this;
	}

	Iterand<_Kind_> & Variant(Reason::Generic::Variant<_Kind_> * variant)
	{
		Decrement();
		Proxy = 0;

		if (Iteration) 
			Option &= ~(PROXY|STATE);
		else 
			Option &= ~(STATE);

		Type = TYPE_VARIANT;
		Union = variant;
		return *this;
	}

	Iterand<_Kind_> & Kind(typename Template<_Kind_>::ConstantPointer kind)
	{
		Decrement();
		Proxy = 0;

		if (Iteration) 
			Option &= ~(PROXY|STATE);
		else 
			Option &= ~(STATE);

		Type = TYPE_KIND;
		Union = Reason::Generic::Pointer<_Kind_>::Template(kind);
		return *this;
	}

	Iterand<_Kind_> & Kind(typename Template<_Kind_>::ConstantReference kind)
	{
		Decrement();
		Proxy = 0;

		if (Iteration) 
			Option &= ~(PROXY|STATE);
		else 
			Option &= ~(STATE);

		Type = TYPE_KIND;
		Union = new typename Reason::Generic::Template<_Kind_>::Value(Reason::Generic::Reference<_Kind_>::Template(kind));

		Proxy = new IterandCounted();	
		Increment();
		return *this;
	}

	Iterand(typename Template<_Kind_>::ConstantReference kind, int option=0):
		Iteration(),Proxy(0),Type(TYPE_KIND),Option(option)
	{
		Option |= PROXY_COUNTED;
		Union = new typename Reason::Generic::Template<_Kind_>::Value(Reason::Generic::Reference<_Kind_>::Template(kind));
		Proxy = new IterandCounted();	
		Increment();
	}

	Iterand(typename Template<_Kind_>::ConstantPointer kind, int option=0):
		Iteration(),Proxy(0),Type(TYPE_KIND),Option(option),Union(Reason::Generic::Pointer<_Kind_>::Template(kind))
	{
	}

	Iterand(Reason::Generic::Variant<_Kind_> * variant, int option=0):
		Iteration(),Proxy(0),Type(TYPE_VARIANT),Option(option),Union(variant)
	{
	}

	Iterand(const Strong< Reason::Structure::Iteration<_Kind_> * > & iteration, typename Template<_Kind_>::ConstantReference kind, int option=0):
		Iteration(iteration),Proxy(0),Type(TYPE_KIND),Option(option)
	{
		Option |= PROXY_COUNTED;
		Union = new typename Reason::Generic::Template<_Kind_>::Value(Reason::Generic::Reference<_Kind_>::Template(kind));
		Proxy = new IterandCounted();	
		Increment();			
	}

	Iterand(const Strong< Reason::Structure::Iteration<_Kind_> * > & iteration, typename Template<_Kind_>::ConstantPointer kind, int option=0):
		Iteration(iteration),Proxy(0),Type(TYPE_KIND),Option(option),Union(Reason::Generic::Pointer<_Kind_>::Template(kind))
	{	
	}

	Iterand(const Strong< Reason::Structure::Iteration<_Kind_> * > & iteration, Reason::Generic::Variant<_Kind_> * variant, int option=0):
		Iteration(iteration),Proxy(0),Type(TYPE_VARIANT),Option(option),Union(variant)
	{
	}			

	Iterand(const Strong< Reason::Structure::Iteration<_Kind_> * > & iteration):
		Iteration(iteration),Proxy(0),Type(0),Option(0)
	{
	}

	Iterand(Reason::Structure::Iteration<_Kind_> * iteration):
		Iteration(iteration),Proxy(0),Type(0),Option(0)
	{
	}		

	Iterand(const Iterand<_Kind_> & iterand):
		Iteration(),Proxy(iterand.Proxy),Type(iterand.Type),Option(iterand.Option),Union(iterand.Union)
	{
		if (Proxy) Increment();

		if (iterand.Iteration)
			iterand.Iteration->Swap(*this,(Iterand<_Kind_> &)iterand);

	}

	Iterand(const Reason::Structure::Iterator<_Kind_> & iterator);

	Iterand():
		Iteration(),Proxy(0),Type(TYPE_NULL),Option(0)
	{
	}

	~Iterand() 
	{
		Decrement();
		Proxy = 0;
	}

	Iterand<_Kind_> & Swap(const Iterand<_Kind_> & iterand)
	{

		IterandUnion uni = iterand.Union;	

		IterandProxy * proxy = this->Proxy;			
		this->Proxy = iterand.Proxy;
		Increment();
		this->Proxy = proxy;
		proxy = iterand.Proxy;
		Decrement();
		this->Proxy = proxy;

		this->Union = uni;

		Type = iterand.Type;

		Option &= ~PROXY;
		Option |= (iterand.Option&PROXY);

		Option &= ~STATE;		

		if ((iterand.Option&MODE))
		{
			Option &= ~MODE;
			Option |= (iterand.Option&MODE);
		}

		if (iterand.Iteration)		
			iterand.Iteration->Swap(*this,(Iterand<_Kind_> &)iterand);
		else
			Iteration = 0;

		return *this;	
	}

	Iterand<_Kind_> & operator = (Reason::Generic::Variant<_Kind_> * variant) 
	{

		return Swap(variant);
	}

	Iterand<_Kind_> & operator = (typename Template<_Kind_>::ConstantPointer kind) 
	{

		return Swap(kind);
	}

	Iterand<_Kind_> & operator = (typename Template<_Kind_>::ConstantReference kind) 
	{

		return Swap(kind);
	}

	Iterand<_Kind_> & operator = (const Iterand<_Kind_> & iterand)
	{

		return Swap(iterand);
	}

	Iterand<_Kind_> & operator = (const Reason::Structure::Iterator<_Kind_> & iterator);

	bool operator == (const Iterand<_Kind_> & iterand) 
	{	
		if (Proxy && iterand.Proxy)
			return Proxy == ((Iterand<_Kind_>&)iterand).Proxy;
		else
			return Union == ((Iterand<_Kind_>&)iterand).Union;
	}

	bool operator == (int value)
	{
		return (int)(bool)(operator void * ()) == value;
	}

	bool operator != (const Iterand<_Kind_> & iterand) {return !operator == (iterand);}	
	bool operator != (int value) {return !operator == (value);}

	friend bool operator == (Iterand<_Kind_> & left, Iterand<_Kind_> & right) {return right.operator == (left);}
	friend bool operator != (Iterand<_Kind_> & left, Iterand<_Kind_> & right) {return !right.operator == (left);}

	friend bool operator == (bool left, Iterand<_Kind_> & right) {return right == left;}
	friend bool operator != (bool left, Iterand<_Kind_> & right) {return right != left;}

	int Index() 
	{
		if (Type == TYPE_INDEX)
			return Union.Index;
		else
			return (Iteration)?Iteration->Index(*this):-1;
	}

	operator void * () const
	{
		if (Bitmask::Is((unsigned short)Option,STATE_INVALID|STATE_UNIQUE|STATE_ERROR))
			return 0;

		if ((Option&MODE) != 0 && Iteration && !Iteration->Valid(*((Iterand<_Kind_>*)this)))
		{
			((Iterand<_Kind_>*)this)->Invalid();
			return 0;
		}

		switch(Type)
		{
		case TYPE_VARIANT: return (void*)Union.Variant;
		case TYPE_KIND: return (void*)Union.Kind;
		case TYPE_INDEX: return (Union.Index > -1)?(void*)&Union.Index:0;
		case TYPE_NULL: return 0;
		}

		return 0;
	}

	void * operator & () const
	{

		switch(Type)
		{
		case TYPE_VARIANT: return (void*)(Variant<_Kind_> *)Union.Variant;
		case TYPE_KIND: return (void*)Union.Kind;

		case TYPE_INDEX: return &Union.Index;
		case TYPE_NULL: return 0;
		}

		return 0;
	}

	void * operator & ()
	{

		switch(Type)
		{
		case TYPE_VARIANT: return (void*)Union.Variant;
		case TYPE_KIND: return (void*)Union.Kind;
		case TYPE_INDEX: return &Union.Index;
		}

		return 0;
	}

	typename Reason::Generic::Type<_Kind_>::Pointer Pointer() 
	{

		OutputAssert(Type==TYPE_KIND||Type==TYPE_VARIANT|Type==TYPE_NULL);
		switch(Type)
		{
		case TYPE_VARIANT: return (Union.Variant)?Union.Variant->Pointer():0;
		case TYPE_KIND: return Reason::Generic::Pointer<_Kind_>::Type(Union.Kind);

		}
		return 0;
	}

	typename Reason::Generic::Type<_Kind_>::Reference Reference() 
	{
		OutputAssert((Type==TYPE_KIND && Union.Kind != 0)||(Type==TYPE_VARIANT && Union.Variant != 0));

		switch(Type)
		{
		case TYPE_VARIANT: return Union.Variant->Reference();
		case TYPE_KIND: return Reason::Generic::Reference<_Kind_>::Type(Union.Kind);

		}
	}

	typename Reason::Generic::Template<_Kind_>::Reference operator () (void) 
	{
		OutputAssert((Type==TYPE_KIND && Union.Kind != 0)||(Type==TYPE_VARIANT && Union.Variant != 0));

		switch(Type)
		{
		case TYPE_VARIANT: return (*Union.Variant)();
		case TYPE_KIND: return Reason::Generic::Reference<_Kind_>::Template(Union.Kind);

		}

		return Reason::Generic::Null<_Kind_>::Value();
	}

	typename Reason::Generic::Template<_Kind_>::Reference operator * () {return (*this)();}
	typename Reason::Generic::Template<_Kind_>::Pointer operator -> () {return & (*this)();}

	Iterand<_Kind_> operator ++ (int);	
	Iterand<_Kind_> & operator ++ ();		
	Iterand<_Kind_> operator -- (int);	
	Iterand<_Kind_> & operator -- ();		

	static void Release(Iterand<_Kind_> & iterand)
	{
		switch(iterand.Type)
		{			
			case Iterand<_Kind_>::TYPE_VARIANT: Reason::Structure::Disposer<Reason::Generic::Variant<_Kind_> *>::Release(iterand.Union.Variant); break;
			case Iterand<_Kind_>::TYPE_KIND: Reason::Structure::Disposer<typename Reason::Generic::Template<_Kind_>::Pointer>::Release(iterand.Union.Kind); break;
		}
	}

	static void Destroy(Iterand<_Kind_> & iterand)
	{
		switch(iterand.Type)
		{
			case Iterand<_Kind_>::TYPE_VARIANT: Reason::Structure::Disposer<Reason::Generic::Variant<_Kind_> *>::Destroy(iterand.Union.Variant); break;
			case Iterand<_Kind_>::TYPE_KIND: Reason::Structure::Disposer<typename Reason::Generic::Template<_Kind_>::Pointer>::Destroy(iterand.Union.Kind); break;
		}	
	}	

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Iterable;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Iteration
{
public:

	Weak<Reason::Structure::Iterable<_Kind_> *,Shared> Iterable;

	Strong< Reason::Structure::Policy::Base<_Kind_> * > Base;

	Iteration(Reason::Structure::Iterable<_Kind_> * iterable) 
	{
		if (iterable)
		{
			if (iterable->Initial.Count && iterable->Initial.Count->Weak < 0)
			{
				OutputBreak();			
			}

			Iterable = iterable->Invalid;
			if (iterable->Initial)
				Base = iterable->Initial->Base;
		}

	}

	Iteration()
	{
	}

	virtual ~Iteration() 
	{

	}

	virtual bool Valid(Iterand<_Kind_> & iterand)
	{

		return (bool)Iterable;
	}

	virtual Iteration<_Kind_> * Clone() 
	{

		return 0;
	}

	virtual void Swap(Iterand<_Kind_> & left, Iterand<_Kind_> & right)
	{

		left.Iteration = right.Iteration;
	}

	virtual int Index(Iterand<_Kind_> & iterand) 
	{

		return -1;
	}

	virtual void Forward(Iterand<_Kind_> & iterand)
	{
		if (Iterable)
		{
			iterand = Iterable->Forward();
		}
	}

	virtual void Reverse(Iterand<_Kind_> & iterand)
	{
		if (Iterable)
		{
			iterand = Iterable->Reverse();
		}
	}

	virtual void Move(Iterand<_Kind_> & iterand) 
	{

		Move(iterand,1);
	}

	virtual void Move(Iterand<_Kind_> & iterand, int amount)
	{

		if (iterand.Move() == 0)
			iterand.Forward();

		if (Iterable)
		{

			Iterator<_Kind_> iterator = Iterable->Iterate();
			iterand.Iteration = iterator.Iterand.Iteration;
			iterand.Iteration->Move(iterand,amount);
		}
		else
		{

			iterand.Option |= Iterand<_Kind_>::STATE_INVALID;
		}
	}

};

template <typename _Kind_>
Iterand<_Kind_> Iterand<_Kind_>::operator ++ (int)
{

	if (!Iteration ) 
	{	
		(*this) = (Reason::Generic::Variant<_Kind_>*)0;
		return (*this);
	}

	Iterand<_Kind_> iterand = (*this);
	Iteration->Move(*this,1);
	return iterand;
}

template <typename _Kind_>
Iterand<_Kind_> & Iterand<_Kind_>::operator ++ ()
{
	if (!Iteration )
	{	
		(*this) = (Reason::Generic::Variant<_Kind_>*)0;
		return (*this);
	}

	Iteration->Move(*this,1);
	return (*this);
}

template <typename _Kind_>
Iterand<_Kind_> Iterand<_Kind_>::operator -- (int)
{
	if (!Iteration )
	{	
		(*this) = (Reason::Generic::Variant<_Kind_>*)0;
		return (*this);
	}

	Iterand<_Kind_> iterand = (*this);
	Iteration->Move(*this,-1);
	return iterand;
}

template <typename _Kind_>
Iterand<_Kind_> & Iterand<_Kind_>::operator -- ()
{
	if (!Iteration )
	{	
		(*this) = (Reason::Generic::Variant<_Kind_>*)0;
		return (*this);
	}

	Iteration->Move(*this,-1);
	return (*this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Iterator
{
public:

	Reason::Structure::Iterand<_Kind_> Iterand;

	Iterator(const Iterator<_Kind_> & iterator):Iterand(iterator.Iterand)
	{
	}

	Iterator(Iterable<_Kind_> & iterable)
	{
		Iterate(iterable);
	}

	Iterator(const Reason::Structure::Iterand<_Kind_> & iterand)
	{
		Iterate(iterand);
	}

	Iterator(const Reason::Structure::Iterand<_Kind_> & from, const Reason::Structure::Iterand<_Kind_> & to)
	{
		Iterate(from,to);
	}

	Iterator(Iteration<_Kind_> * iteration):Iterand(iteration)
	{
	}

	Iterator()
	{
	}

	~Iterator()
	{

	}

	void Iterate(Iterable<_Kind_> & iterable)
	{
		(*this) = iterable.Iterate();
	}

	void Iterate(Iterable<_Kind_> & iterable, int from, int to)
	{
		(*this) = iterable.Iterate(from,to);
	}

	void Iterate(Iterable<_Kind_> & iterable, const Reason::Structure::Iterand<_Kind_> & from, const Reason::Structure::Iterand<_Kind_> & to)
	{
		(*this) = iterable.Iterate(from,to);
	}

	void Iterate(int from, int to)
	{
		if (Iterand.Iteration)
		{
			(*this) = Iterand.Iteration->Iterable->Iterate(from,to);
		}
		else
		{
			Iterand = Reason::Structure::Iterand<_Kind_>();
		}
	}

	void Iterate(const Reason::Structure::Iterand<_Kind_> & iterand)
	{
		Iterand = iterand;
	}

	void Iterate(const Reason::Structure::Iterand<_Kind_> & from, const Reason::Structure::Iterand<_Kind_> & to)
	{
		if (Iterand.Iteration)
		{
			(*this) = Iterand.Iteration->Iterable->Iterate(from,to);
		}
		else
		if (from.Iteration != 0 && to.Iteration != 0 && from.Iteration->Iterable == to.Iteration->Iterable)
		{
			(*this) = from.Iteration->Iterable->Iterate(from,to);
		}
		else
		{
			Iterand = Reason::Structure::Iterand<_Kind_>();
		}
	}

	Iterator<_Kind_> & operator = (const Iterator<_Kind_> & iterator)
	{
		Iterand = iterator.Iterand;

		return *this;
	}

	Iterator<_Kind_> & operator = (const Reason::Structure::Iterand<_Kind_> & iterand)
	{

		Iterand = iterand;
		return *this;
	}

	void Forward(Structure::Iterand<_Kind_> & iterand) 
	{

		if (iterand.Iteration != Iterand.Iteration) 
			iterand.Iteration = Iterand.Iteration;

		if (Iterand.Iteration) 
			Iterand.Iteration->Forward(iterand);
		else
			iterand = Structure::Iterand<_Kind_>();
	}

	void Reverse(Structure::Iterand<_Kind_> & iterand) 
	{
		if (iterand.Iteration != Iterand.Iteration) 
			iterand.Iteration = Iterand.Iteration;

		if (Iterand.Iteration) 
			Iterand.Iteration->Reverse(iterand);
		else
			iterand = Structure::Iterand<_Kind_>();
	}

	void Move(Structure::Iterand<_Kind_> & iterand)
	{			
		Move(iterand,1);
	}

	void Move(Structure::Iterand<_Kind_> & iterand, int amount)
	{	
		if (iterand == 0)
			return;

		if (iterand.Move() == 0)
			iterand.Forward();

		if (iterand.Iteration != Iterand.Iteration)
		{

			Forward(iterand);
		}

		if (Iterand.Iteration )
			Iterand.Iteration->Move(iterand,amount);
		else

			iterand = (Reason::Generic::Variant<_Kind_>*)0;
	}

	int Index(Structure::Iterand<_Kind_> & iterand)
	{
		if (iterand != 0 && iterand.Iteration == this->Iterand.Iteration)
		{
			return (this->Iterand.Iteration)?this->Iterand.Iteration->Index(iterand):-1;
		}

		return -1;
	}

	Structure::Iterand<_Kind_> & Forward()
	{
		Forward(Iterand);
		return Iterand;
	}

	Structure::Iterand<_Kind_> & Reverse()
	{
		Reverse(Iterand);
		return Iterand;
	}

	Structure::Iterand<_Kind_> & Move()
	{
		Move(Iterand);
		return Iterand;
	}

	Structure::Iterand<_Kind_> & Move(int amount)
	{
		Move(Iterand,amount);
		return Iterand;
	}

	bool Has()
	{

		return Iterand != 0 && Iterand.Iteration != 0 && Iterand.Iteration->Iterable;
	}

	int Index()
	{
		return (Iterand != 0 && Iterand.Iteration && Iterand.Iteration->Iterable)?Iterand.Iteration->Index(Iterand):-1;
	}

	void Release() 
	{
		Iterand.Release();
	}

	void Destroy()
	{
		Iterand.Destroy();
	}

	typename Type<_Kind_>::Pointer Pointer() {return Iterand.Pointer();}
	typename Type<_Kind_>::Reference Reference() {return Iterand.Reference();}
	typename Template<_Kind_>::Reference operator () (void) {return Iterand();}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
Iterand<_Kind_>::Iterand(const Iterator<_Kind_> & iterator):
	Iteration(),Proxy(iterator.Iterand.Proxy),Type(iterator.Iterand.Type),Option(iterator.Iterand.Option),Union(iterator.Iterand.Union)
{
	if (Proxy) Increment();

	if (iterator.Iterand.Iteration)
		iterator.Iterand.Iteration->Swap(*this,(Iterand<_Kind_> &)iterator.Iterand);

	if ((*this) == 0)
		((Iterator<_Kind_> &)iterator).Forward((Iterand<_Kind_> &)*this);	
}

template<typename _Kind_>
Iterand<_Kind_> & Iterand<_Kind_>::operator = (const Iterator<_Kind_> & iterator)
{
	operator = (iterator.Iterand);

	if (&(*this) == 0)
		((Iterator<_Kind_> &)iterator).Forward((Iterand<_Kind_> &)*this);

	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Iterable : public Reason::System::Kind
{
friend class Iterator<_Kind_>;
public:

	Weak< Structure::Iterable<_Kind_> *, Shared> Invalid;

	Strong< Structure::Iteration<_Kind_> * > Initial;

	typedef _Kind_ Kind;
	typedef Structure::Iterator<_Kind_> Iterator;

	Iterable(const Iterable & iterable)
	{

		Invalid = Weak<Structure::Iterable<_Kind_> *, Shared>(this);
		Initial = new Structure::Iteration<_Kind_>(this);
		Initial->Base = iterable.Initial->Base;
	}

	Iterable(Structure::Policy::Base<_Kind_> * base)
	{
		Invalid = Weak<Structure::Iterable<_Kind_> *, Shared>(this);
		Initial = new Structure::Iteration<_Kind_>(this);
		Initial->Base = base;
	}

	Iterable()
	{	

		Invalid = Weak<Structure::Iterable<_Kind_> *, Shared>(this);
		Initial = new Structure::Iteration<_Kind_>(this);
		Initial->Base = new Structure::Policy::Default<_Kind_>();
	}

	virtual ~Iterable()
	{

		if (Invalid)
		{

			Invalid = 0;
		}
	}

	Iterable & operator = (const Iterable & iterable)
	{

	}

	Structure::Policy::Base<_Kind_> * Policy()
	{
		return Initial->Base;
	}

	virtual void Invalidate()
	{

		if (Invalid.Count->Weak > 2)
		{
			Invalid = 0;
			Invalid = Weak<Structure::Iterable<_Kind_> *, Shared>(this);
			Initial->Iterable = Invalid;
		}
	}

	virtual Structure::Iterator<_Kind_> Iterate() {return Structure::Iterator<_Kind_>();}
	virtual Structure::Iterator<_Kind_> Iterate(int from, int to) {return Structure::Iterator<_Kind_>();}
	virtual Structure::Iterator<_Kind_> Iterate(Structure::Iterand<_Kind_> & from, Structure::Iterand<_Kind_> & to) {return Structure::Iterator<_Kind_>();}

	Structure::Iterand<_Kind_> Forward() 
	{
		Structure::Iterand<_Kind_> iterand;
		Structure::Iterator<_Kind_> iterator = Iterate();
		iterator.Forward(iterand);
		return iterand;
	}

	Structure::Iterand<_Kind_> Reverse()
	{
		Structure::Iterand<_Kind_> iterand;
		Structure::Iterator<_Kind_> iterator = Iterate();
		iterator.Reverse(iterand);
		return iterand;
	}

	virtual Reason::Generic::Variant<_Kind_> * Get(Iterand<_Kind_> & iterand)
	{
		Reason::Generic::Variant<_Kind_> * variant = 0;
		Get(iterand,variant);
		return variant;
	}	

	virtual bool Get(Iterand<_Kind_> & iterand, Reason::Generic::Variant<_Kind_> *& variant)
	{	
		variant = 0;

		if (iterand.Option&Iterand<_Kind_>::MODE_GET)
		{
			variant = (Reason::Generic::Variant<_Kind_>*) &iterand;
			return true;
		}	

		Iteration<_Kind_> * iteration = (iterand.Iteration!=0)?iterand.Iteration():0;
		if (iteration)
		{			
			if (iteration == this->Initial() || iteration->Iterable->InstanceOf(this))
			{

				variant = (Reason::Generic::Variant<_Kind_>*) &iterand;
				return true;
			}
		}

		return false;

	}

	virtual bool Get(Iterand<_Kind_> & iterand, typename Reason::Generic::Template<_Kind_>::Pointer & kind)
	{
		return true;
	}

	virtual Reason::Generic::Variant<_Kind_> * Put(Iterand<_Kind_> & iterand)
	{

		Reason::Generic::Variant<_Kind_> * variant = 0;
		Put(iterand,variant);
		return variant;
	}

	virtual bool Put(Iterand<_Kind_> & iterand, Reason::Generic::Variant<_Kind_> *& variant)
	{	
		variant = 0;

		if (iterand.Option&Iterand<_Kind_>::MODE_PUT)
		{
			variant = (Reason::Generic::Variant<_Kind_>*) &iterand;
			return true;
		}	

		if (iterand.Proxy)
		{			
			Iteration<_Kind_> * iteration = (iterand.Iteration!=0)?iterand.Iteration():0;
			if (iteration)
			{			
				if (iteration == this->Initial() || iteration->Iterable->InstanceOf(this))
				{

					if (iterand.Proxy->Dispose != Disposable::DISPOSE_NONE)
					{

						variant = (Reason::Generic::Variant<_Kind_>*) &iterand;			
						iterand.Proxy->Dispose = Disposable::DISPOSE_NONE;
						return true;
					}
				}
			}
		}

		return false;	
	}

	virtual bool Put(Iterand<_Kind_> & iterand, typename Reason::Generic::Template<_Kind_>::Pointer & kind)
	{
		return false;
	}	

	virtual void * New(int size)
	{
		return Initial->Base->Allocate.New(size);
	}

	virtual void Del(void * data)
	{
		Initial->Base->Allocate.Del(data);
	}

	virtual int Compare(typename Reason::Generic::Template<_Kind_>::ConstantReference left, typename Reason::Generic::Template<_Kind_>::ConstantReference right, int compare=Comparable::COMPARE_POLICY)
	{
		return Initial->Base->Compare(left,right,compare);
	}

	virtual int Compare(Iterand<_Kind_> & left, Iterand<_Kind_> & right, int compare=Comparable::COMPARE_POLICY)
	{
		return Initial->Base->Compare(left(),right(),compare);
	}

	virtual void Dispose(Iterand<_Kind_> & iterand)
	{
		 Initial->Base->Dispose(iterand());
	}

	virtual void Release(Iterand<_Kind_> & iterand)
	{

		if (iterand.Proxy == 0)
		{
			Reason::Structure::Disposer<_Kind_>::Release(iterand());

			if (iterand.Type&Iterand<_Kind_>::TYPE_VARIANT)
			{
				iterand.Union.Variant->~Variant();
				Initial->Base->Allocate.Del(iterand.Union.Variant);
			}
		}
	}

	virtual void Destroy(Iterand<_Kind_> & iterand)
	{

		if (iterand.Proxy == 0)
		{

			Reason::Structure::Disposer<_Kind_>::Destroy(iterand());

			if (iterand.Type&Iterand<_Kind_>::TYPE_VARIANT)
			{
				iterand.Union.Variant->~Variant();
				Initial->Base->Allocate.Del(iterand.Union.Variant);
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

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef REASON_USING_STL
namespace std
{

template <typename _Kind_>
struct iterator_traits< Reason::Structure::Iterand<_Kind_> >
{
	typedef bidirectional_iterator_tag iterator_category;
	typedef int difference_type;	
	typedef typename Reason::Generic::Type<_Kind_>::Value value_type;
	typedef typename Reason::Generic::Type<_Kind_>::Pointer pointer;
	typedef typename Reason::Generic::Type<_Kind_>::Reference reference;		

};

}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

