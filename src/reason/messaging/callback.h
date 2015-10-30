
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
#ifndef MESSAGING_CALLBACK_H
#define MESSAGING_CALLBACK_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/generic/generic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::Generic;
using namespace Reason::System;

namespace Reason { namespace Messaging {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Call
{
public:

	class Function
	{
	public:

	};

	class Base
	{
	public:
	};

	class Derived : public Base, public Function
	{
	public:
	};

	class Virtual : virtual Derived
	{
	public:		
	};

	class Unknown;

	typedef int (*CallFunction)(int);
	typedef int (Base::*CallBase)(int);
	typedef int (Derived::*CallDerived)(int);
	typedef int (Virtual::*CallVirtual)(int);
	typedef int (Unknown::*CallUnknown)(int);

	static const int SizeofFunction = sizeof(CallFunction);
	static const int SizeofBase = sizeof(CallBase);
	static const int SizeofDerived = sizeof(CallDerived);
	static const int SizeofVirtual = sizeof(CallVirtual);
	static const int SizeofUnknown = sizeof(CallUnknown);

	enum CallType
	{
		CALL_TYPE_FUNCTION	= 1,
		CALL_TYPE_BASE		= 2,
		CALL_TYPE_DERIVED	= 3,
		CALL_TYPE_VIRTUAL	= 4,
		CALL_TYPE_UNKNOWN	= 5,
	};

	union Calls
	{
		CallFunction Function;
		CallBase Base;
		CallDerived Derived;
		CallVirtual Virtual;
		CallUnknown Unknown;

		Calls():Unknown(0) {}
	};

public:

	char Type;
	char Args;
	short Crc;
	void * That;
	Calls Func;

	Call():Type(0),Args(0),That(0)
	{
	}

	Call(short type, short args, void * that):Type(type),Args(args),That(that)
	{
	}

	operator bool ()
	{
		return Type != 0 && Func.Unknown != 0;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Return_, typename _Arg1_=None, typename _Arg2_=None, typename _Arg3_=None, typename _Arg4_=None, typename _Arg5_=None>
class Callback : public Call
{
public:

	#define Sizeofcall(call) (sizeof(call)<=SizeofBase?2:(sizeof(call)<=SizeofDerived?3:(sizeof(call)<=SizeofVirtual?4:(sizeof(call)<=SizeofUnknown?5:0))))

	Callback() 
	{
	}

	template<typename _Class_>
	Callback(_Return_ (_Class_::*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_)):Call(Sizeofcall(call),5,0)
	{
		struct Info {_Return_ (_Class_::*Func)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	template<typename _Class_>
	Callback(_Class_ * that, _Return_ (_Class_::*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_)):Call(Sizeofcall(call),5,that)
	{
		struct Info {_Return_ (_Class_::*Func)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	Callback(_Return_ (*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_)):Call(1,5,0)
	{
		struct Info {_Return_ (*Func)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	_Return_ operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4, _Arg5_ arg5) const
	{
		OutputAssert(Type != 0);
		OutputAssert(Type == CALL_TYPE_FUNCTION || That != 0);

		switch(Type)
		{
		case CALL_TYPE_FUNCTION: typedef _Return_ (*FunctionPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_); return ((FunctionPrototype)Func.Function)(arg1,arg2,arg3,arg4,arg5);
		case CALL_TYPE_BASE: typedef _Return_ (Base::*BasePrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_); return (((Base*)That)->*((BasePrototype)Func.Base))(arg1,arg2,arg3,arg4,arg5);
		case CALL_TYPE_DERIVED: typedef _Return_ (Derived::*DerivedPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_); return (((Derived*)That)->*((DerivedPrototype)Func.Derived))(arg1,arg2,arg3,arg4,arg5);
		case CALL_TYPE_VIRTUAL: typedef _Return_ (Virtual::*VirtualPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_); return (((Virtual*)That)->*((VirtualPrototype)Func.Virtual))(arg1,arg2,arg3,arg4,arg5);
		default: typedef _Return_ (Unknown::*UnknownPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_); return (((Unknown*)That)->*((UnknownPrototype)Func.Unknown))(arg1,arg2,arg3,arg4,arg5);
		}

		return Null<_Return_>::Value();
	}	

	template<typename _Class_>
	_Return_ operator() (_Class_ * that, _Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4, _Arg5_ arg5) const
	{
		OutputAssert(Type != 0);
		OutputAssert(Type != CALL_TYPE_FUNCTION && that != 0);

		switch(Type)
		{
		case CALL_TYPE_BASE: typedef _Return_ (Base::*BasePrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_); return (((Base*)that)->*((BasePrototype)Func.Base))(arg1,arg2,arg3,arg4,arg5);
		case CALL_TYPE_DERIVED: typedef _Return_ (Derived::*DerivedPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_); return (((Derived*)that)->*((DerivedPrototype)Func.Derived))(arg1,arg2,arg3,arg4,arg5);
		case CALL_TYPE_VIRTUAL: typedef _Return_ (Virtual::*VirtualPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_); return (((Virtual*)that)->*((VirtualPrototype)Func.Virtual))(arg1,arg2,arg3,arg4,arg5);
		default: typedef _Return_ (Unknown::*UnknownPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_); return (((Unknown*)that)->*((UnknownPrototype)Func.Unknown))(arg1,arg2,arg3,arg4,arg5);
		}

		return Null<_Return_>::Value();
	}

};

template <typename _Return_>
class Callback<_Return_> : public Call
{
public:

	Callback() 
	{
	}

	template<typename _Class_>
	Callback(_Return_ (_Class_::*call)()):Call(Sizeofcall(call),0,0)
	{
		struct Info {_Return_ (_Class_::*Func)();} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	template<typename _Class_>
	Callback(_Class_ * that, _Return_ (_Class_::*call)()):Call(Sizeofcall(call),0,that)
	{
		struct Info {_Return_ (_Class_::*Func)();} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	Callback(_Return_ (*call)()):Call(1,0,0)
	{
		struct Info {_Return_ (*Func)();} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	_Return_ operator() () const
	{
		OutputAssert(Type != 0);
		OutputAssert(Type == CALL_TYPE_FUNCTION || That != 0);

		switch(Type)
		{
		case CALL_TYPE_FUNCTION: typedef _Return_ (*FunctionPrototype)(); return ((FunctionPrototype)Func.Function)();
		case CALL_TYPE_BASE: typedef _Return_ (Base::*BasePrototype)(); return (((Base*)That)->*((BasePrototype)Func.Base))();
		case CALL_TYPE_DERIVED: typedef _Return_ (Derived::*DerivedPrototype)(); return (((Derived*)That)->*((DerivedPrototype)Func.Derived))();
		case CALL_TYPE_VIRTUAL: typedef _Return_ (Virtual::*VirtualPrototype)(); return (((Virtual*)That)->*((VirtualPrototype)Func.Virtual))();
		default: typedef _Return_ (Unknown::*UnknownPrototype)(); return (((Unknown*)That)->*((UnknownPrototype)Func.Unknown))();
		}

		return Null<_Return_>::Value();
	}

	template<typename _Class_>
	_Return_ operator() (_Class_ * that) const
	{
		OutputAssert(Type != 0);
		OutputAssert(Type != CALL_TYPE_FUNCTION && that != 0);

		switch(Type)
		{
		case CALL_TYPE_BASE: typedef _Return_ (Base::*BasePrototype)(); return (((Base*)that)->*((BasePrototype)Func.Base))();
		case CALL_TYPE_DERIVED: typedef _Return_ (Derived::*DerivedPrototype)(); return (((Derived*)that)->*((DerivedPrototype)Func.Derived))();
		case CALL_TYPE_VIRTUAL: typedef _Return_ (Virtual::*VirtualPrototype)(); return (((Virtual*)that)->*((VirtualPrototype)Func.Virtual))();
		default: typedef _Return_ (Unknown::*UnknownPrototype)(); return (((Unknown*)that)->*((UnknownPrototype)Func.Unknown))();
		}

		return Null<_Return_>::Value();
	}
};

template <typename _Return_, typename _Arg1_>
class Callback<_Return_,_Arg1_> : public Call
{
public:

	Callback() 
	{
	}

	template<typename _Class_>
	Callback(_Return_ (_Class_::*call)(_Arg1_)):Call(Sizeofcall(call),1,0)
	{
		struct Info {_Return_ (_Class_::*Func)(_Arg1_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	template<typename _Class_>
	Callback(_Class_ * that, _Return_ (_Class_::*call)(_Arg1_)):Call(Sizeofcall(call),1,that)
	{
		struct Info {_Return_ (_Class_::*Func)(_Arg1_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	Callback(_Return_ (*call)(_Arg1_)):Call(1,1,0)
	{
		struct Info {_Return_ (*Func)(_Arg1_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	_Return_ operator() (_Arg1_ arg1) const
	{
		OutputAssert(Type != 0);
		OutputAssert(Type == CALL_TYPE_FUNCTION || That != 0);

		switch(Type)
		{
		case CALL_TYPE_FUNCTION: typedef _Return_ (*FunctionPrototype)(_Arg1_); return ((FunctionPrototype)Func.Function)(arg1);
		case CALL_TYPE_BASE: typedef _Return_ (Base::*BasePrototype)(_Arg1_); return (((Base*)That)->*((BasePrototype)Func.Base))(arg1);
		case CALL_TYPE_DERIVED: typedef _Return_ (Derived::*DerivedPrototype)(_Arg1_); return (((Derived*)That)->*((DerivedPrototype)Func.Derived))(arg1);
		case CALL_TYPE_VIRTUAL: typedef _Return_ (Virtual::*VirtualPrototype)(_Arg1_); return (((Virtual*)That)->*((VirtualPrototype)Func.Virtual))(arg1);
		default: typedef _Return_ (Unknown::*UnknownPrototype)(_Arg1_); return (((Unknown*)That)->*((UnknownPrototype)Func.Unknown))(arg1);
		}

		return Null<_Return_>::Value();
	}

	template<typename _Class_>
	_Return_ operator() (_Class_ * that, _Arg1_ arg1) const
	{
		OutputAssert(Type != 0);
		OutputAssert(Type != CALL_TYPE_FUNCTION && that != 0);

		switch(Type)
		{
		case CALL_TYPE_BASE: typedef _Return_ (Base::*BasePrototype)(_Arg1_); return (((Base*)that)->*((BasePrototype)Func.Base))(arg1);
		case CALL_TYPE_DERIVED: typedef _Return_ (Derived::*DerivedPrototype)(_Arg1_); return (((Derived*)that)->*((DerivedPrototype)Func.Derived))(arg1);
		case CALL_TYPE_VIRTUAL: typedef _Return_ (Virtual::*VirtualPrototype)(_Arg1_); return (((Virtual*)that)->*((VirtualPrototype)Func.Virtual))(arg1);
		default: typedef _Return_ (Unknown::*UnknownPrototype)(_Arg1_); return (((Unknown*)that)->*((UnknownPrototype)Func.Unknown))(arg1);
		}

		return Null<_Return_>::Value();
	}
};

template <typename _Return_, typename _Arg1_, typename _Arg2_>
class Callback<_Return_,_Arg1_,_Arg2_> : public Call
{
public:

	Callback() 
	{
	}

	template<typename _Class_>
	Callback(_Return_ (_Class_::*call)(_Arg1_,_Arg2_)):Call(Sizeofcall(call),2,0)
	{
		struct Info {_Return_ (_Class_::*Func)(_Arg1_,_Arg2_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	template<typename _Class_>
	Callback(_Class_ * that, _Return_ (_Class_::*call)(_Arg1_,_Arg2_)):Call(Sizeofcall(call),2,that)
	{
		struct Info {_Return_ (_Class_::*Func)(_Arg1_,_Arg2_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	Callback(_Return_ (*call)(_Arg1_,_Arg2_)):Call(1,2,0)
	{
		struct Info {_Return_ (*Func)(_Arg1_,_Arg2_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	_Return_ operator() (_Arg1_ arg1, _Arg2_ arg2) const
	{
		OutputAssert(Type != 0);
		OutputAssert(Type == CALL_TYPE_FUNCTION || That != 0);

		switch(Type)
		{
		case CALL_TYPE_FUNCTION: typedef _Return_ (*FunctionPrototype)(_Arg1_,_Arg2_); return ((FunctionPrototype)Func.Function)(arg1,arg2);
		case CALL_TYPE_BASE: typedef _Return_ (Base::*BasePrototype)(_Arg1_,_Arg2_); return (((Base*)That)->*((BasePrototype)Func.Base))(arg1,arg2);
		case CALL_TYPE_DERIVED: typedef _Return_ (Derived::*DerivedPrototype)(_Arg1_,_Arg2_); return (((Derived*)That)->*((DerivedPrototype)Func.Derived))(arg1,arg2);
		case CALL_TYPE_VIRTUAL: typedef _Return_ (Virtual::*VirtualPrototype)(_Arg1_,_Arg2_); return (((Virtual*)That)->*((VirtualPrototype)Func.Virtual))(arg1,arg2);
		default: typedef _Return_ (Unknown::*UnknownPrototype)(_Arg1_,_Arg2_); return (((Unknown*)That)->*((UnknownPrototype)Func.Unknown))(arg1,arg2);
		}

		return Null<_Return_>::Value();
	}

	template<typename _Class_>
	_Return_ operator() (_Class_ * that, _Arg1_ arg1, _Arg2_ arg2) const
	{
		OutputAssert(Type != 0);
		OutputAssert(Type != CALL_TYPE_FUNCTION && that != 0);

		switch(Type)
		{
		case CALL_TYPE_BASE: typedef _Return_ (Base::*BasePrototype)(_Arg1_,_Arg2_); return (((Base*)that)->*((BasePrototype)Func.Base))(arg1,arg2);
		case CALL_TYPE_DERIVED: typedef _Return_ (Derived::*DerivedPrototype)(_Arg1_,_Arg2_); return (((Derived*)that)->*((DerivedPrototype)Func.Derived))(arg1,arg2);
		case CALL_TYPE_VIRTUAL: typedef _Return_ (Virtual::*VirtualPrototype)(_Arg1_,_Arg2_); return (((Virtual*)that)->*((VirtualPrototype)Func.Virtual))(arg1,arg2);
		default: typedef _Return_ (Unknown::*UnknownPrototype)(_Arg1_,_Arg2_); return (((Unknown*)that)->*((UnknownPrototype)Func.Unknown))(arg1,arg2);
		}

		return Null<_Return_>::Value();
	}
};

template <typename _Return_, typename _Arg1_, typename _Arg2_, typename _Arg3_>
class Callback<_Return_,_Arg1_,_Arg2_,_Arg3_> : public Call
{
public:

	Callback() 
	{
	}

	template<typename _Class_>
	Callback(_Return_ (_Class_::*call)(_Arg1_,_Arg2_,_Arg3_)):Call(Sizeofcall(call),3,0)
	{
		struct Info {_Return_ (_Class_::*Func)(_Arg1_,_Arg2_,_Arg3_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	template<typename _Class_>
	Callback(_Class_ * that, _Return_ (_Class_::*call)(_Arg1_,_Arg2_,_Arg3_)):Call(Sizeofcall(call),3,that)
	{
		struct Info {_Return_ (_Class_::*Func)(_Arg1_,_Arg2_,_Arg3_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	Callback(_Return_ (*call)(_Arg1_,_Arg2_,_Arg3_)):Call(1,3,0)
	{
		struct Info {_Return_ (*Func)(_Arg1_,_Arg2_,_Arg3_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	_Return_ operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3) const
	{
		OutputAssert(Type != 0);
		OutputAssert(Type == CALL_TYPE_FUNCTION || That != 0);

		switch(Type)
		{
		case CALL_TYPE_FUNCTION: typedef _Return_ (*FunctionPrototype)(_Arg1_,_Arg2_,_Arg3_); return ((FunctionPrototype)Func.Function)(arg1,arg2,arg3);
		case CALL_TYPE_BASE: typedef _Return_ (Base::*BasePrototype)(_Arg1_,_Arg2_,_Arg3_); return (((Base*)That)->*((BasePrototype)Func.Base))(arg1,arg2,arg3);
		case CALL_TYPE_DERIVED: typedef _Return_ (Derived::*DerivedPrototype)(_Arg1_,_Arg2_,_Arg3_); return (((Derived*)That)->*((DerivedPrototype)Func.Derived))(arg1,arg2,arg3);
		case CALL_TYPE_VIRTUAL: typedef _Return_ (Virtual::*VirtualPrototype)(_Arg1_,_Arg2_,_Arg3_); return (((Virtual*)That)->*((VirtualPrototype)Func.Virtual))(arg1,arg2,arg3);
		default: typedef _Return_ (Unknown::*UnknownPrototype)(_Arg1_,_Arg2_,_Arg3_); return (((Unknown*)That)->*((UnknownPrototype)Func.Unknown))(arg1,arg2,arg3);
		}

		return Null<_Return_>::Value();
	}	

	template<typename _Class_>
	_Return_ operator() (_Class_ * that, _Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3) const
	{
		OutputAssert(Type != 0);
		OutputAssert(Type != CALL_TYPE_FUNCTION && that != 0);

		switch(Type)
		{
		case CALL_TYPE_BASE: typedef _Return_ (Base::*BasePrototype)(_Arg1_,_Arg2_,_Arg3_); return (((Base*)that)->*((BasePrototype)Func.Base))(arg1,arg2,arg3);
		case CALL_TYPE_DERIVED: typedef _Return_ (Derived::*DerivedPrototype)(_Arg1_,_Arg2_,_Arg3_); return (((Derived*)that)->*((DerivedPrototype)Func.Derived))(arg1,arg2,arg3);
		case CALL_TYPE_VIRTUAL: typedef _Return_ (Virtual::*VirtualPrototype)(_Arg1_,_Arg2_,_Arg3_); return (((Virtual*)that)->*((VirtualPrototype)Func.Virtual))(arg1,arg2,arg3);
		default: typedef _Return_ (Unknown::*UnknownPrototype)(_Arg1_,_Arg2_,_Arg3_); return (((Unknown*)that)->*((UnknownPrototype)Func.Unknown))(arg1,arg2,arg3);
		}

		return Null<_Return_>::Value();
	}
};

template <typename _Return_, typename _Arg1_, typename _Arg2_, typename _Arg3_, typename _Arg4_>
class Callback<_Return_,_Arg1_,_Arg2_,_Arg3_,_Arg4_> : public Call
{
public:

	Callback() 
	{
	}

	template<typename _Class_>
	Callback(_Return_ (_Class_::*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_)):Call(Sizeofcall(call),4,0)
	{
		struct Info {_Return_ (_Class_::*Func)(_Arg1_,_Arg2_,_Arg3_,_Arg4_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	template<typename _Class_>
	Callback(_Class_ * that, _Return_ (_Class_::*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_)):Call(Sizeofcall(call),4,that)
	{
		struct Info {_Return_ (_Class_::*Func)(_Arg1_,_Arg2_,_Arg3_,_Arg4_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	Callback(_Return_ (*call)(_Arg1_,_Arg2_,_Arg3_,_Arg4_)):Call(1,4,0)
	{
		struct Info {_Return_ (*Func)(_Arg1_,_Arg2_,_Arg3_,_Arg4_);} info = {call};
		memcpy(&Func,&info,sizeof(info));
	}

	_Return_ operator() (_Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4) const
	{
		OutputAssert(Type != 0);
		OutputAssert(Type == CALL_TYPE_FUNCTION || That != 0);

		switch(Type)
		{
		case CALL_TYPE_FUNCTION: typedef _Return_ (*FunctionPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_); return ((FunctionPrototype)Func.Function)(arg1,arg2,arg3,arg4);
		case CALL_TYPE_BASE: typedef _Return_ (Base::*BasePrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_); return (((Base*)That)->*((BasePrototype)Func.Base))(arg1,arg2,arg3,arg4);
		case CALL_TYPE_DERIVED: typedef _Return_ (Derived::*DerivedPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_); return (((Derived*)That)->*((DerivedPrototype)Func.Derived))(arg1,arg2,arg3,arg4);
		case CALL_TYPE_VIRTUAL: typedef _Return_ (Virtual::*VirtualPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_); return (((Virtual*)That)->*((VirtualPrototype)Func.Virtual))(arg1,arg2,arg3,arg4);
		default: typedef _Return_ (Unknown::*UnknownPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_); return (((Unknown*)That)->*((UnknownPrototype)Func.Unknown))(arg1,arg2,arg3,arg4);
		}

		return Null<_Return_>::Value();
	}	

	template<typename _Class_>
	_Return_ operator() (_Class_ * that, _Arg1_ arg1, _Arg2_ arg2, _Arg3_ arg3, _Arg4_ arg4) const
	{
		OutputAssert(Type != 0);
		OutputAssert(Type != CALL_TYPE_FUNCTION && that != 0);

		switch(Type)
		{
		case CALL_TYPE_BASE: typedef _Return_ (Base::*BasePrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_); return (((Base*)that)->*((BasePrototype)Func.Base))(arg1,arg2,arg3,arg4);
		case CALL_TYPE_DERIVED: typedef _Return_ (Derived::*DerivedPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_); return (((Derived*)that)->*((DerivedPrototype)Func.Derived))(arg1,arg2,arg3,arg4);
		case CALL_TYPE_VIRTUAL: typedef _Return_ (Virtual::*VirtualPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_); return (((Virtual*)that)->*((VirtualPrototype)Func.Virtual))(arg1,arg2,arg3,arg4);
		default: typedef _Return_ (Unknown::*UnknownPrototype)(_Arg1_,_Arg2_,_Arg3_,_Arg4_); return (((Unknown*)that)->*((UnknownPrototype)Func.Unknown))(arg1,arg2,arg3,arg4);
		}

		return Null<_Return_>::Value();
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

