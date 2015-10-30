
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
#ifndef GENERIC_GENERIC_H
#define GENERIC_GENERIC_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/output.h"
#include "reason/system/interface.h"

#include "reason/structure/comparable.h"
#include "reason/platform/platform.h"

#pragma warning(disable:4522)

#include <assert.h>
#include <stdlib.h>
#ifdef REASON_PLATFORM_LINUX
#include <new>
#else
#include <new.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;
using namespace Reason::Structure;
using namespace Reason::Platform;

namespace Reason { namespace Generic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Empty
{
};

struct None 
{
};

struct True
{
};

struct False
{
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<bool _Bool_> 
struct Assert
{
	Assert(...) {}
};

template<>
struct Assert<false>
{
	Assert() {}
};

#define StaticAssert(x) {\
	Assert<((bool)(x) != false)> error = Assert<((bool)(x) != false)>(None());\
	(void)sizeof(error);}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
struct Class
{
public:

	static bool Is()
	{
		return IsClass<_Kind_>(0);
	}

private:

	template<typename Parameter>
	static bool IsClass(int Parameter::*)
	{
		return true;
	}

	template<typename Parameter>
	static bool IsClass(...)
	{
		return false;
	}

};

template<typename _Kind_>
struct Primitive
{
	static const bool Is() 
	{
		return !Class<_Kind_>::Is();
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
struct Template
{
	typedef _Kind_   Value;
	typedef _Kind_ * Pointer;
	typedef _Kind_ & Reference;

	typedef const _Kind_   ConstantValue;
	typedef const _Kind_ * ConstantPointer;
	typedef const _Kind_ & ConstantReference;
};

template <typename _Kind_>
struct Template<_Kind_&>
{
	typedef _Kind_ & Value;
	typedef _Kind_ * Pointer;
	typedef _Kind_ & Reference;

	typedef const _Kind_ & ConstantValue;
	typedef const _Kind_ * ConstantPointer;
	typedef const _Kind_ & ConstantReference;
};

template <typename _Kind_>
struct Template<_Kind_*>
{

	typedef _Kind_ *  Value;
	typedef _Kind_ ** Pointer;
	typedef _Kind_ *& Reference;

	typedef const _Kind_ *         ConstantValue;
	typedef const _Kind_ * const * ConstantPointer;
	typedef const _Kind_ * const & ConstantReference;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
struct Type
{

	typedef _Kind_   Value;
	typedef _Kind_ * Pointer;
	typedef _Kind_ & Reference;

	typedef const _Kind_   ConstantValue;
	typedef const _Kind_ * ConstantPointer;
	typedef const _Kind_ & ConstantReference;
};

template <typename _Kind_>
struct Type<_Kind_&>
{

	typedef _Kind_   Value;
	typedef _Kind_ * Pointer;
	typedef _Kind_ & Reference;

	typedef const _Kind_   ConstantValue;
	typedef const _Kind_ * ConstantPointer;
	typedef const _Kind_ & ConstantReference;
};

template <typename _Kind_>
struct Type<_Kind_*>
{

	typedef _Kind_   Value;
	typedef _Kind_ * Pointer;
	typedef _Kind_ & Reference;

	typedef const _Kind_   ConstantValue;
	typedef const _Kind_ * ConstantPointer;
	typedef const _Kind_ & ConstantReference;
};

template<>
struct Type<void*>
{
	typedef void   Value;
	typedef void * Pointer;
	typedef void   Reference;

	typedef const void   ConstantValue;
	typedef const void * ConstantPointer;
	typedef const void   ConstantReference;
};

template<>
struct Type<void>
{
	typedef void   Value;
	typedef void * Pointer;
	typedef void   Reference;

	typedef const void   ConstantValue;
	typedef const void * ConstantPointer;
	typedef const void   ConstantReference;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
struct Value
{
	static typename Generic::Type<_Kind_>::Value Type(typename Generic::Template<_Kind_>::ConstantReference kind) {return (typename Generic::Template<_Kind_>::Reference)kind;};
	static typename Generic::Type<_Kind_>::Value Type(typename Generic::Template<_Kind_>::ConstantPointer kind) {return (typename Generic::Template<_Kind_>::Reference)*kind;};

	static typename Generic::Template<_Kind_>::Value Template(typename Generic::Template<_Kind_>::ConstantReference kind) {return (typename Generic::Template<_Kind_>::Reference)kind;};
	static typename Generic::Template<_Kind_>::Value Template(typename Generic::Template<_Kind_>::ConstantPointer kind) {return (typename Generic::Template<_Kind_>::Reference)*kind;};
};

template <typename _Kind_>
struct Value<_Kind_&>
{
	static typename Generic::Type<_Kind_&>::Value Type(typename Generic::Template<_Kind_&>::ConstantReference kind) {return (typename Generic::Template<_Kind_&>::Reference)kind;}
	static typename Generic::Type<_Kind_&>::Value Type(typename Generic::Template<_Kind_&>::ConstantPointer kind) {return (typename Generic::Template<_Kind_&>::Reference)*kind;}

	static typename Generic::Template<_Kind_&>::Value Template(typename Generic::Template<_Kind_&>::ConstantReference kind) {return (typename Generic::Template<_Kind_&>::Reference)kind;}
	static typename Generic::Template<_Kind_&>::Value Template(typename Generic::Template<_Kind_&>::ConstantPointer kind) {return (typename Generic::Template<_Kind_&>::Reference)*kind;}
};

template <typename _Kind_>
struct Value<_Kind_*>
{
	static typename Generic::Type<_Kind_*>::Value Type(typename Generic::Template<_Kind_*>::ConstantReference kind) {return *(typename Generic::Template<_Kind_*>::Reference)kind;}	
	static typename Generic::Type<_Kind_*>::Value Type(typename Generic::Template<_Kind_*>::ConstantPointer kind) {return *(typename Generic::Template<_Kind_*>::Reference)*kind;}	

	static typename Generic::Template<_Kind_*>::Value Template(typename Generic::Template<_Kind_*>::ConstantReference kind) {return (typename Generic::Template<_Kind_*>::Reference)kind;}	
	static typename Generic::Template<_Kind_*>::Value Template(typename Generic::Template<_Kind_*>::ConstantPointer kind) {return (typename Generic::Template<_Kind_*>::Reference)*kind;}	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
struct Reference
{
	static typename Generic::Type<_Kind_>::Reference Type(typename Generic::Template<_Kind_>::ConstantReference kind) {return (typename Generic::Template<_Kind_>::Reference)kind;}
	static typename Generic::Type<_Kind_>::Reference Type(typename Generic::Template<_Kind_>::ConstantPointer kind) {return (typename Generic::Template<_Kind_>::Reference)*kind;}

	static typename Generic::Template<_Kind_>::Reference Template(typename Generic::Template<_Kind_>::ConstantReference kind) {return (typename Generic::Template<_Kind_>::Reference)kind;}
	static typename Generic::Template<_Kind_>::Reference Template(typename Generic::Template<_Kind_>::ConstantPointer kind) {return (typename Generic::Template<_Kind_>::Reference)*kind;}
};

template <typename _Kind_>
struct Reference<_Kind_&>
{
	static typename Generic::Type<_Kind_&>::Reference Type(typename Generic::Template<_Kind_&>::ConstantReference kind) {return (typename Generic::Template<_Kind_&>::Reference)kind;}
	static typename Generic::Type<_Kind_&>::Reference Type(typename Generic::Template<_Kind_&>::ConstantPointer kind) {return (typename Generic::Template<_Kind_&>::Reference)*kind;}

	static typename Generic::Template<_Kind_&>::Reference Template(typename Generic::Template<_Kind_&>::ConstantReference kind) {return (typename Generic::Template<_Kind_&>::Reference)kind;}
	static typename Generic::Template<_Kind_&>::Reference Template(typename Generic::Template<_Kind_&>::ConstantPointer kind) {return (typename Generic::Template<_Kind_&>::Reference)*kind;}
};

template <typename _Kind_>
struct Reference<_Kind_*>
{
	static typename Generic::Type<_Kind_*>::Reference Type(typename Generic::Template<_Kind_*>::ConstantReference kind) {return *(typename Generic::Template<_Kind_*>::Reference)kind;}	
	static typename Generic::Type<_Kind_*>::Reference Type(typename Generic::Template<_Kind_*>::ConstantPointer kind) {return *(typename Generic::Template<_Kind_*>::Reference)*kind;}	

	static typename Generic::Template<_Kind_*>::Reference Template(typename Generic::Template<_Kind_*>::ConstantReference kind) {return (typename Generic::Template<_Kind_*>::Reference)kind;}
	static typename Generic::Template<_Kind_*>::Reference Template(typename Generic::Template<_Kind_*>::ConstantPointer kind) {return (typename Generic::Template<_Kind_*>::Reference)*kind;}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
struct Pointer
{
	static typename Generic::Type<_Kind_>::Pointer Type(typename Generic::Template<_Kind_>::ConstantReference kind) {return (_Kind_*)&(char&)(typename Generic::Template<_Kind_>::Reference)kind;};
	static typename Generic::Type<_Kind_>::Pointer Type(typename Generic::Template<_Kind_>::ConstantPointer kind) {return (typename Generic::Template<_Kind_>::Pointer)kind;};

	static typename Generic::Template<_Kind_>::Pointer Template(typename Generic::Template<_Kind_>::ConstantReference kind) {return (_Kind_*)&(char&)(typename Generic::Template<_Kind_>::Reference)kind;};
	static typename Generic::Template<_Kind_>::Pointer Template(typename Generic::Template<_Kind_>::ConstantPointer kind) {return (typename Generic::Template<_Kind_>::Pointer)kind;};
};

template <typename _Kind_>
struct Pointer<_Kind_&>
{
	static typename Generic::Type<_Kind_&>::Pointer Type(typename Generic::Template<_Kind_&>::ConstantReference kind) {return (_Kind_*)&(char&)(typename Generic::Template<_Kind_&>::Reference)kind;}
	static typename Generic::Type<_Kind_&>::Pointer Type(typename Generic::Template<_Kind_&>::ConstantPointer kind) {return (typename Generic::Template<_Kind_&>::Pointer)kind;};

	static typename Generic::Template<_Kind_&>::Pointer Template(typename Generic::Template<_Kind_&>::ConstantReference kind) {return (_Kind_*)&(char&)(typename Generic::Template<_Kind_&>::Reference)kind;}
	static typename Generic::Template<_Kind_&>::Pointer Template(typename Generic::Template<_Kind_&>::ConstantPointer kind) {return (typename Generic::Template<_Kind_&>::Pointer)kind;}
};

template <typename _Kind_>
struct Pointer<_Kind_*>
{
	static typename Generic::Type<_Kind_*>::Pointer Type(typename Generic::Template<_Kind_*>::ConstantReference kind) {return (typename Generic::Template<_Kind_*>::Reference)kind;}
	static typename Generic::Type<_Kind_*>::Pointer Type(typename Generic::Template<_Kind_*>::ConstantPointer kind) {return *(typename Generic::Template<_Kind_*>::Pointer)kind;}

	static typename Generic::Template<_Kind_*>::Pointer Template(typename Generic::Template<_Kind_*>::ConstantReference kind) {return &(typename Generic::Template<_Kind_*>::Reference)kind;}
	static typename Generic::Template<_Kind_*>::Pointer Template(typename Generic::Template<_Kind_*>::ConstantPointer kind) {return (typename Generic::Template<_Kind_*>::Pointer)kind;}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Cast
{

	template<typename _Kind_>
	static _Kind_ & Reference(const _Kind_ & kind)
	{
		return (_Kind_&)kind;
	}

	template<typename _Kind_>
	static _Kind_ * Pointer(const _Kind_ & kind)
	{
		return &(_Kind_&)kind;
	}

	template<typename _Kind_>
	static _Kind_ Value(const _Kind_ & kind)
	{
		return (_Kind_&)kind;
	}

};

template <typename _Kind_>
_Kind_ * Dynamic(Object * object)
{
	if (object->InstanceOf(_Kind_::Instance))
		return (_Kind_*)object;
	else
		return 0;
};

template <typename _Kind_>
_Kind_ Reinterpret(_Kind_ kind)
{
	return *(_Kind_*)((void*)&kind);
};

template<typename _Kind_>
_Kind_ Reinterpret(_Kind_ * kind)
{
	return (_Kind_)((void*)kind);
};

template <typename _Kind_>
_Kind_ Static(typename Template<_Kind_>::Reference kind)
{
	return (_Kind_)kind;
};

template <typename _Kind_>
_Kind_ Const(typename Template<_Kind_>::ConstantReference kind)
{
	return (_Kind_)kind;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
struct Null
{

	static typename Generic::Template<_Kind_>::Reference Value() 
	{

		static char mem[sizeof(_Kind_)] = {};

		return *(typename Generic::Template<_Kind_>::Pointer)(void*)&mem;
	}

	static typename Generic::Template<_Kind_>::Pointer Pointer() {return Generic::Pointer<_Kind_>::Template(0);}

	static bool Equals(typename Generic::Template<_Kind_>::ConstantReference kind)
	{

		if (&kind == &Value())
			return true;

		static char mem[sizeof(_Kind_)] = {};
		return memcmp(&kind,&mem,sizeof(_Kind_)) == 0;
	}

	bool operator == (typename Generic::Template<_Kind_>::ConstantReference kind)
	{
		return Null<_Kind_>::Equals(kind);
	}
};

template<>
struct Null<void>
{
	static void Value() {return (void)0;}
	static void * Pointer() {return (void*)0;}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
struct Is
{

	static inline bool IsValue()		{return true;};
	static inline bool IsReference()	{return false;};
	static inline bool IsPointer()		{return false;};
	static inline bool IsClass()		{return Class<_Kind_>::Is();};
	static inline bool IsPrimitive()	{return !IsClass();};
};

template <typename _Kind_>
struct Is<_Kind_&>
{

	static inline bool IsValue()		{return false;};
	static inline bool IsReference()	{return true;};
	static inline bool IsPointer()		{return false;};
	static inline bool IsClass()		{return Class<_Kind_>::Is();};
	static inline bool IsPrimitive()	{return !IsClass();};

};

template <typename _Kind_>
struct Is<_Kind_*>
{

	static inline bool IsValue()		{return false;};
	static inline bool IsReference()	{return false;};
	static inline bool IsPointer()		{return true;};
	static inline bool IsClass()		{return Class<_Kind_>::Is();};
	static inline bool IsPrimitive()	{return !IsClass();};

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Variant 
{
public:

	Variant() {}
	virtual ~Variant() {}

	virtual int Compare(typename Template<_Kind_>::ConstantReference type, int comparitor = Comparer<_Kind_>::COMPARE_GENERAL)
	{
		return Reason::Structure::Comparer<_Kind_>::Compare((*this)(),type,comparitor);
	}

	virtual int Compare(Variant<_Kind_> & value, int comparitor = Comparer<_Kind_>::COMPARE_GENERAL)
	{

		return Reason::Structure::Comparer<_Kind_>::Compare((*this)(),value(),comparitor);
	}

	virtual typename Type<_Kind_>::Pointer Pointer()=0;
	virtual typename Type<_Kind_>::Reference Reference()=0;
	virtual typename Template<_Kind_>::Reference operator () (void)=0;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Variable : public Variant<_Kind_>
{
public:

	Variable():Kind()
	{

	}

	Variable(const Variable & variable):Kind(((Variable &)variable).Kind)
	{
	}

	Variable(typename Template<_Kind_>::ConstantReference kind):Kind((typename Template<_Kind_>::Reference)kind)
	{
	}

	~Variable()
	{
	}

	_Kind_ Kind;
	_Kind_ & operator ()(void)	
	{
		return Kind;
	}

	Variable & operator = (const Variable & variable)
	{
		Kind = ((Variable & )variable).Kind;
		return *this;
	}

	typename Type<_Kind_>::Value Value() {return Kind;}
	typename Type<_Kind_>::Reference Reference() {return Kind;}
	typename Type<_Kind_>::Pointer Pointer() {return (_Kind_*)&(char&)Kind;}

};

template <typename _Kind_>
class Variable<_Kind_&> : public Variant<_Kind_&>
{
public:

	Variable()
	{

	}

	Variable(const Variable & variable):Kind(((Variable &)variable).Kind)
	{
	}

	Variable(typename Template<_Kind_&>::ConstantReference kind):Kind((typename Template<_Kind_&>::Reference)kind)
	{
	}

	~Variable()
	{
	}

	_Kind_ & Kind;
	_Kind_ & operator ()(void)	
	{
		return Kind;
	}

	Variable & operator = (const Variable & variable)
	{
		Kind = variable.Kind;
		return *this;
	}

	typename Type<_Kind_&>::Reference Value()	{return Kind;}
	typename Type<_Kind_&>::Reference Reference() {return Kind;}
	typename Type<_Kind_&>::Pointer Pointer()	{return (_Kind_*)&(char&)Kind;}

};

template <typename _Kind_>
class Variable<_Kind_*> : public Variant<_Kind_*>
{
public:

	Variable():Kind(0)
	{
	}

	Variable(const Variable & variable):Kind(((Variable &)variable).Kind)
	{
	}

	Variable(typename Template<_Kind_*>::ConstantReference kind):Kind((typename Template<_Kind_*>::Reference)kind)
	{
	}

	~Variable()
	{
	}

	_Kind_ * Kind;
	_Kind_ *& operator ()(void)	
	{
		return Kind;
	}

	Variable & operator = (const Variable & variable)
	{
		Kind = variable.Kind;
		return *this;
	}

	typename Type<_Kind_*>::Reference Value() {return *Kind;}
	typename Type<_Kind_*>::Reference Reference() {return *Kind;}
	typename Type<_Kind_*>::Pointer Pointer() {return Kind;}

};

template <>
class Variable<void*> : public Variant<void*>
{
public:

	Variable():Kind(0)
	{
	}

	Variable(const Variable & variable):Kind(((Variable &)variable).Kind)
	{
	}

	Variable(const void * kind):Kind((void*)kind)
	{
	}

	~Variable()
	{
	}

	void * Kind;
	void *& operator ()(void)	
	{
		return Kind;
	}

	Variable & operator = (const Variable & variable)
	{
		Kind = variable.Kind;
		return *this;
	}

	void Value() {}
	void Reference() {}
	void * Pointer() {return Kind;}
};

template <>
class Variable<void> : public Variant<void*>
{
public:

	Variable():Kind(0)
	{
	}

	Variable(const Variable & variable):Kind(((Variable &)variable).Kind)
	{
	}

	Variable(const void * kind):Kind((void*)kind)
	{
	}

	~Variable()
	{
	}

	void * Kind;
	void *& operator ()(void)	
	{
		return Kind;
	}

	Variable & operator = (const Variable & variable)
	{
		Kind = variable.Kind;
		return *this;
	}

	void Value() {}
	void Reference() {}
	void * Pointer() {return Kind;}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Any
{
public:

	template <typename _Kind_>
	Any(_Kind_):
		Var(0)	
	{

	}

	~Any()
	{

	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename TypeOfKind>
bool InstanceOf(typename Template<_Kind_>::ConstantReference left, TypeOfKind type)
{
	return false;
}

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

class Proxied
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
class Intrinsic
{
public:

};

template<typename _Kind_>
class Linked
{
public:

	Linked<_Kind_> * Prev;
	Linked<_Kind_> * Next;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Kind_>
class Proxy
{
public:

	class Counted
	{
	public:

		char Mem[sizeof(_Kind_)];
		_Kind_ * Kind;
		volatile int Weak;
		volatile int Strong;

		Counted(typename Template<_Kind_>::ConstantReference kind):Kind(0),Weak(0),Strong(0) 

		{

			Kind = new ((void*)Mem) _Kind_ (kind);

		}

		Counted():Kind(0),Weak(0),Strong(0)

		{

		}

		~Counted()
		{

			if (Kind == (_Kind_*)&Mem)
			{
				Kind->_Kind_::~_Kind_();
				Kind = 0;
			}
		}

		operator int ()
		{
			return Strong;
		}

		void Set(typename Template<_Kind_>::ConstantReference kind)
		{
			if (!Kind)
				Kind = new ((void*)Mem) _Kind_ (kind);
			else	
				*Kind = kind;
		}

		void Zero()
		{

			if (this->Kind)
			{

				if (Kind == (_Kind_*)&Mem)
				{

					Kind->_Kind_::~_Kind_();
					Kind = 0;
				}
			}
		}
	};

	Counted * Count;	

	Proxy():Count(0)
	{
	}

	virtual ~Proxy()
	{
	}

	_Kind_ & operator () (void) const
	{

		if (!Count)
		{
			((Proxy<_Kind_>*)this)->Count = new Counted();
			((Proxy<_Kind_>*)this)->Increment();
		}

		if (!Count->Kind)
			Count->Kind = new ((void*)Count->Mem) _Kind_ ();

		return *Count->Kind;									
	}

	_Kind_ * operator & () const
	{
		_Kind_ & kind = (*this)();
		return &kind;
	}	

	_Kind_ * operator -> () const
    {

		bool null = (Count==0||Count->Kind==0);
		_Kind_ & kind = (*this)();
		if (null) Count->Set(Null<_Kind_>::Value());
		return &kind;
    }

    _Kind_ & operator * () const
    {
		bool null = (Count==0||Count->Kind==0);
    	_Kind_ & kind = (*this)();
		if (null) Count->Set(Null<_Kind_>::Value());
		return kind;
    }

	operator _Kind_ * () const
	{

		return (Count==0)?0:Count->Kind;
	}

	bool operator != (Proxy<_Kind_> & proxy) const
	{
		return Count != proxy.Count;
	}

    bool operator == (Proxy<_Kind_> & proxy) const
    {
        return Count == proxy.Count;
    }

   	bool operator ()(Proxy<_Kind_> * proxy) const
	{

		return this == proxy;
	} 

    void Swap(Proxy<_Kind_> & proxy)
	{

		Counted * counted = this->Count;
		this->Count = proxy.Count;
		Increment();
		this->Count = counted;
		counted = proxy.Count;
		Decrement();
		this->Count = counted;
	}

	virtual void Increment()=0;
	virtual void Decrement()=0;	

};

template<typename _Kind_>
class Proxy<_Kind_*>
{
public:

	class Counted
	{
	public:

		_Kind_ * Kind;
		volatile int Weak;
		volatile int Strong;

		Counted(typename Template<_Kind_*>::ConstantReference kind):Kind(Reason::Generic::Pointer<_Kind_*>::Type(kind)),Weak(0),Strong(0)
		{

		}

		Counted():Kind(0),Weak(0),Strong(0)
		{

		}

		operator int ()
		{
			return Strong;
		}

		void Set(typename Template<_Kind_*>::ConstantReference kind)
		{
			Kind = Reason::Generic::Pointer<_Kind_>::Type(kind);
		}

		void Zero()
		{
			if (this->Kind)
			{
				delete this->Kind;
				this->Kind = 0;
			}
		}
	};

	Counted * Count;	

	Proxy():Count(0)
	{
	}

	virtual ~Proxy()
	{
	}

	_Kind_ * operator () (void) const
	{
        OutputAssert(Count != 0 && Count->Kind != 0);
		return (Count==0)?0:Count->Kind;	
	}

	_Kind_ ** operator & () const
	{

		if (!Count)
		{
			((Proxy<_Kind_*>*)this)->Count = new Counted();
			((Proxy<_Kind_*>*)this)->Increment();
		}

		return &Count->Kind;

	}

	_Kind_ * operator -> () const
    {
        OutputAssert(Count != 0 && Count->Kind != 0);
		return (Count==0)?0:Count->Kind;
    }

    _Kind_ & operator * () const
    {
		OutputAssert(Count != 0 && Count->Kind != 0);
        return *Count->Kind;
    }

	operator _Kind_ * () const
	{

		return (Count==0)?0:Count->Kind;
	}

	bool operator != (Proxy<_Kind_*> & proxy) const
	{

		return Count != proxy.Count;
	}

    bool operator == (Proxy<_Kind_*> & proxy) const
    {

        return Count == proxy.Count;
    }

	bool operator () (Proxy<_Kind_*> * proxy) const
	{

		return this == proxy;
	}

    void Swap(Proxy<_Kind_*> & proxy)
	{
		Counted * counted = this->Count;
		this->Count = proxy.Count;
		Increment();
		this->Count = counted;
		counted = proxy.Count;
		Decrement();
		this->Count = counted;
	}

	virtual void Increment()=0;
	virtual void Decrement()=0;

};

template<>
class Proxy<void*>
{
public:

    class Counted
    {
    public:

        void * Kind;
        volatile int Weak;
        volatile int Strong;

		Counted(Template<void*>::ConstantReference kind):Kind(Reason::Generic::Pointer<void*>::Type(kind)),Weak(0),Strong(0)
        {

        }

        Counted():Kind(0),Weak(0),Strong(0)
        {

        }

        operator int ()
        {
            return Strong;
        }

		void Set(Template<void*>::ConstantReference kind)
        {
            Kind = Reason::Generic::Pointer<void*>::Type(kind);
        }

        void Zero()
        {
            if (this->Kind)
            {
                this->Kind = 0;
            }
        }
    };

    Counted * Count;	

    Proxy():Count(0)
    {
    }

    virtual ~Proxy()
    {
    }

    void * operator () (void) const
    {
        OutputAssert(Count != 0);
        OutputAssert(Count->Kind);
        return (Count==0)?0:Count->Kind;	
    }

    void ** operator & () const
    {
        if (!Count)
        {
            ((Proxy<void*>*)this)->Count = new Counted();
            ((Proxy<void*>*)this)->Increment();
        }

        return &Count->Kind;
    }

    void * operator -> () const
    {
        OutputAssert(Count != 0);
        OutputAssert(Count->Kind);
        return (Count==0)?0:Count->Kind;
    }

    operator void * () const
    {
        return (Count==0)?0:Count->Kind;
    }

    bool operator != (Proxy<void*> & proxy) const
    {
        return Count != proxy.Count;
    }

    bool operator == (Proxy<void*> & proxy) const
    {
        return Count == proxy.Count;
    }

    bool operator () (Proxy<void*> * proxy) const
    {
        return this == proxy;
    }

    void Swap(Proxy<void*> & proxy)
	{
		Counted * counted = this->Count;
		this->Count = proxy.Count;
		Increment();
		this->Count = counted;
		counted = proxy.Count;
		Decrement();
		this->Count = counted;
	}

	virtual void Increment()=0;
	virtual void Decrement()=0;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_> 
class Owned : public Proxy<_Kind_>
{
public:

    Owned()
	{	
	}

    Owned(typename Template<_Kind_>::ConstantReference kind)
    {
		if (!Null<_Kind_>::Equals(kind)) operator = (kind);
    }

	Owned(const Owned<_Kind_> & owned)
    {
		operator = ((Owned<_Kind_> &)owned);
    }

    ~Owned()
    {
		Decrement();
		this->Count=0;
    }

	void Increment()
	{
		if (this->Count) 
			Atomic::Inc(&this->Count->Strong);

	}

	void Decrement()
	{
		if (this->Count)
		{
			if (this->Count->Strong > 0)
				Atomic::Dec(&this->Count->Strong);

			if (this->Count->Strong == 0)
			{
				this->Count->Zero();

				if (this->Count->Weak == 0)
				{	
					delete this->Count;
					this->Count = 0;
				}
			}
		}
	}

    Owned<_Kind_>& operator = (typename Template<_Kind_>::ConstantReference kind)
    {

		if (!Null<_Kind_>::Equals(kind))
		{
			Counted * counted = new Counted(kind);
			Decrement();
			Count = counted;
			Increment();
		}
		else
		{
			Decrement();
			this->Count = 0;
		}

		return *this;
    }

    Owned<_Kind_>& operator = (const Owned<_Kind_> & owned)
    {
		if (owned.Count == 0 && !owned(this))
		{
			((Owned<_Kind_> &)owned).Count = new typename Proxy<_Kind_>::Counted();
			((Owned<_Kind_> &)owned).Increment();
		}

		Swap(((Owned<_Kind_> &)owned));

		return *this;
    }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_> 
class Shared : public Proxy<_Kind_>
{
public:

    Shared()
	{	
	}

    Shared(typename Template<_Kind_>::ConstantReference kind)
    {
		if (!Null<_Kind_>::Equals(kind)) operator = (kind);
    }

	Shared(const Shared<_Kind_> & shared)
    {
		operator = ((Shared<_Kind_> &)shared);
    }

    ~Shared()
    {
		Decrement();
		this->Count=0;
    }

	void Increment()
	{
		if (this->Count) 
			Atomic::Inc(&this->Count->Strong);
	}

	void Decrement()
	{
		if (this->Count)
		{
			if (this->Count->Strong > 0)
				Atomic::Dec(&this->Count->Strong);

			if (this->Count->Strong == 0)
			{
				this->Count->Zero();

				if (this->Count->Weak == 0)
				{	
					delete this->Count;
					this->Count = 0;
				}
			}
		}
	}

    Shared<_Kind_> & operator = (typename Template<_Kind_>::ConstantReference kind)
    {
		if (this->Count)
		{

			this->Count->Set(kind);
		}
		else
		{
			if (!Null<_Kind_>::Equals(kind))
			{
				this->Count = new typename Proxy<_Kind_>::Counted(kind);
				Increment();
			}
		}

		return *this;
    }

    Shared<_Kind_>& operator = (const Shared<_Kind_> & shared)
    {

		if (shared.Count == 0 && !shared(this))
		{
			((Shared<_Kind_> &)shared).Count = new typename Proxy<_Kind_>::Counted();
			((Shared<_Kind_> &)shared).Increment();
		}

		Swap(((Shared<_Kind_> &)shared));

		return *this;
    }

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, template<typename> class _Proxy_ = Owned> class Weak;

template <typename _Kind_, template<typename> class _Proxy_ = Owned> class Auto;

template <typename _Kind_, template<typename> class _Proxy_ = Owned> class Scoped;

template <typename _Kind_, template<typename> class _Proxy_ = Owned> 
class Strong : public _Proxy_<_Kind_>
{
public:

	Strong() {}
	Strong(typename Template<_Kind_>::ConstantReference kind) {operator = (kind);}
	Strong(const Strong<_Kind_,_Proxy_> & strong) {operator = ((Strong<_Kind_,_Proxy_> &)strong);}
	Strong(const Weak<_Kind_,_Proxy_> & weak) {operator = ((Weak<_Kind_,_Proxy_>&)weak);}
	Strong(const Auto<_Kind_,_Proxy_> & automatic) {operator = ((Auto<_Kind_,_Proxy_> &)automatic);}
	Strong(const Scoped<_Kind_,_Proxy_> & scoped) {operator = ((Scoped<_Kind_,_Proxy_> &)scoped);}

	~Strong()
	{
	}

	using _Proxy_<_Kind_>::operator = ;

	Strong<_Kind_,_Proxy_>& operator = (const Auto<_Kind_,_Proxy_> & automatic)
	{
		Swap((Auto<_Kind_,_Proxy_> &)automatic);
		((Auto<_Kind_,_Proxy_> &)automatic).Count = 0;
		return *this;
	}

	Strong<_Kind_,_Proxy_>& operator = (const Scoped<_Kind_,_Proxy_> & scoped)
	{
		return *this;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, template<typename> class _Proxy_> 
class Weak : public _Proxy_<_Kind_>
{
public:

	Weak() {}
	Weak(typename Template<_Kind_>::ConstantReference kind) {operator = (kind);}
	Weak(const Strong<_Kind_,_Proxy_> & strong) {operator = ((Strong<_Kind_,_Proxy_> &)strong);}
	Weak(const Weak<_Kind_,_Proxy_> & weak) {operator = ((Weak<_Kind_,_Proxy_>&)weak);}
	Weak(const Auto<_Kind_,_Proxy_> & automatic) {operator = ((Auto<_Kind_,_Proxy_> &)automatic);}
	Weak(const Scoped<_Kind_,_Proxy_> & scoped) {operator = ((Scoped<_Kind_,_Proxy_> &)scoped);}

	~Weak()
	{

		Decrement();
		this->Count=0;
	}

	void Increment()
	{
		if (this->Count) 
			Atomic::Inc(&this->Count->Weak);
	}

	void Decrement()
	{
		if (this->Count)
		{
			if (this->Count->Weak > 0)
				Atomic::Dec(&this->Count->Weak);

			if (this->Count->Strong == 0)
			{

				if (this->Count->Weak == 0)
				{	
					delete this->Count;
					this->Count = 0;
				}
			}
		}
	}

	using _Proxy_<_Kind_>::operator = ;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, template<typename> class _Proxy_> 
class Auto : public _Proxy_<_Kind_>
{
public:

	Auto() {}
	Auto(typename Template<_Kind_>::ConstantReference kind) {operator = (kind);}
	Auto(const Strong<_Kind_,_Proxy_> & strong) {operator = ((Strong<_Kind_,_Proxy_> &)strong);}
	Auto(const Weak<_Kind_,_Proxy_> & weak) {operator = ((Weak<_Kind_,_Proxy_>&)weak);}
	Auto(const Auto<_Kind_,_Proxy_> & automatic) {operator = ((Auto<_Kind_,_Proxy_> &)automatic);}
	Auto(const Scoped<_Kind_,_Proxy_> & scoped) {operator = ((Scoped<_Kind_,_Proxy_> &)scoped);}

	~Auto()
	{
	}

	using _Proxy_<_Kind_>::operator = ;

	Auto<_Kind_,_Proxy_>& operator = (const Auto<_Kind_,_Proxy_> & automatic)
	{
		if (!automatic(this))
		{

			OutputAssert(automatic.Count != this->Count);
			OutputAssert(automatic.Count->Strong == 1);

			Swap((Auto<_Kind_,_Proxy_> &)automatic);
			this->Decrement();
			((Auto<_Kind_,_Proxy_> &)automatic).Count = 0;
		}
		return *this;
	}

	Auto<_Kind_,_Proxy_>& operator = (const Scoped<_Kind_,_Proxy_> & scoped)
	{
		return *this;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, template<typename> class _Proxy_> 
class Scoped : public _Proxy_<_Kind_>
{
public:

	Scoped() {}
	Scoped(typename Template<_Kind_>::ConstantReference kind) {operator = (kind);}
	Scoped(const Strong<_Kind_,_Proxy_> & strong) {operator = ((Strong<_Kind_,_Proxy_> &)strong);}
	Scoped(const Weak<_Kind_,_Proxy_> & weak) {operator = ((Weak<_Kind_,_Proxy_>&)weak);}
	Scoped(const Auto<_Kind_,_Proxy_> & automatic) {operator = ((Auto<_Kind_,_Proxy_> &)automatic);}
	Scoped(const Scoped<_Kind_,_Proxy_> & scoped) {operator = ((Scoped<_Kind_,_Proxy_> &)scoped);}

	~Scoped()
	{
	}

	using _Proxy_<_Kind_>::operator = ;

	Scoped<_Kind_,_Proxy_>& operator = (const Strong<_Kind_,_Proxy_> & strong)
	{
		return *this;
	}

	Scoped<_Kind_,_Proxy_>& operator = (const Weak<_Kind_,_Proxy_> & weak)
	{
		return *this;
	}

	Scoped<_Kind_,_Proxy_>& operator = (const Auto<_Kind_,_Proxy_> & automatic)
	{
		if (!automatic(this))
		{
			OutputAssert(automatic.Count != this->Count);
			OutputAssert(automatic.Count->Strong == 1);

			Swap((Auto<_Kind_,_Proxy_> &)automatic);
			this->Decrement();
			((Auto<_Kind_,_Proxy_> &)automatic).Count = 0;
		}
		return *this;
	}

	Scoped<_Kind_,_Proxy_>& operator = (const Scoped<_Kind_,_Proxy_> & scoped)
	{
		return *this;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_> class Nullable : public Variable<_Kind_>
{
public:

	bool Null;

	Nullable(const _Kind_ & variant):Variable<_Kind_>(variant),Null(false) {};
	Nullable():Null(false) {};
	~Nullable() {};

	_Kind_ * operator & ()
	{
		return (Null)?(_Kind_ *)(0):&this->Kind;
	}

	operator _Kind_ ()
	{
		return this->Kind;
	}

	Nullable<_Kind_> & operator = (const _Kind_ & kind)
	{
		this->Kind=kind;
		return *this;
	}

	bool IsNull() {return Null;}

	void Nullify()		{Null=true;};
	void Denullify()	{Null=false;};
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

template <typename _Kind_> class Wrapper : public _Kind_
{
private:

	long WrapperCount;

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

