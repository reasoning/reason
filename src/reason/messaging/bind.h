
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
#ifndef MESSAGING_BIND_H
#define MESSAGING_BIND_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/output.h"
#include "reason/generic/generic.h"
#include "reason/generic/traits.h"
#include "reason/structure/array.h"
#include "reason/messaging/callback.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::Generic;
using namespace Reason::System;
using namespace Reason::Structure;

namespace Reason { namespace Messaging {

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

template<typename _Head_,typename _Tail_>
struct Binder
{
	typedef _Head_ Head;
	typedef _Tail_ Tail;
	typedef Binder<_Head_,_Tail_> Type;
};

template<typename _Binder_, int _Idx_>
struct At
{
	typedef None Type;
};

template<typename _Head_,typename _Tail_, int _Idx_>
struct At<Binder<_Head_,_Tail_>, _Idx_>
{
	typedef typename Conditional::If<Logical::None<_Tail_>::Value,None,
		typename At<_Tail_,_Idx_-1>::Type>::Type Type;
};

template<typename _Head_,typename _Tail_>
struct At<Binder<_Head_,_Tail_>, 0>
{
	typedef _Head_ Type;
};

template<typename _Callback_,typename _Return_,typename _Arg1_=None,typename _Arg2_=None,typename _Arg3_=None,typename _Arg4_=None,typename _Arg5_=None>
class Bind
{
public:

	static const short Bound = (((int)(!Reason::Generic::Logical::None<_Arg1_>::Value)<<4)|
								((int)(!Reason::Generic::Logical::None<_Arg2_>::Value)<<3)|
								((int)(!Reason::Generic::Logical::None<_Arg3_>::Value)<<2)|
								((int)(!Reason::Generic::Logical::None<_Arg4_>::Value)<<1)|
								((int)(!Reason::Generic::Logical::None<_Arg5_>::Value)<<0));

	static const short Unbound = (((int)Reason::Generic::Logical::None<_Arg1_>::Value<<4)|
								  ((int)Reason::Generic::Logical::None<_Arg2_>::Value<<3)|
								  ((int)Reason::Generic::Logical::None<_Arg3_>::Value<<2)|
								  ((int)Reason::Generic::Logical::None<_Arg4_>::Value<<1)|
								  ((int)Reason::Generic::Logical::None<_Arg5_>::Value<<0));

	template<typename _Bind1_, typename _Bind2_, typename _Bind3_, typename _Bind4_, typename _Bind5_>
	struct Binding
	{
		_Bind1_ First;
		_Bind2_ Second;
		_Bind3_ Third;
		_Bind4_ Fourth;
		_Bind5_ Fifth;
	};	

	Binding<_Arg1_,_Arg2_,_Arg3_,_Arg4_,_Arg5_> Args;

	_Callback_ Callback;

	template<typename _Binding_, int _Idx_>
	struct Unbind
	{
		static _Binding_ & Value(_Binding_ & binding) {return binding;}
	};

	template<typename _Binding_>
	struct Unbind<_Binding_,0>
	{ static _Arg1_ & Call(_Binding_ & binding) {return binding.First;} };

	template<typename _Binding_>
	struct Unbind<_Binding_,1>
	{ static _Arg2_ & Call(_Binding_ & binding) {return binding.Second;} };

	template<typename _Binding_>
	struct Unbind<_Binding_,2>
	{ static _Arg2_ & Call(_Binding_ & binding) {return binding.Second;} };

	template<typename _Binding_>
	struct Unbind<_Binding_,3>
	{ static _Arg2_ & Call(_Binding_ & binding) {return binding.Second;} };

	template<typename _Binding_>
	struct Unbind<_Binding_,4>
	{ static _Arg2_ & Call(_Binding_ & binding) {return binding.Second;} };

	template<typename _Binding_,typename _Bound_, int _Idx_, bool _Status_>
	struct Write
	{
		static void Call(){}
	};

	template<typename _Binding_,typename _Bound_>
	struct Write<_Binding_,_Bound_,0,true>
	{ 
		static void Call(_Binding_ & binding, _Bound_ & bound) 
		{
			binding.First = bound;
		}
	};

	template<typename _Binding_,typename _Bound_>
	struct Write<_Binding_,_Bound_,1,true>
	{ 
		static void Call(_Binding_ & binding, _Bound_ & bound) 
		{
			binding.Second = bound;
		}
	};

	template<typename _Binding_,typename _Bound_>
	struct Write<_Binding_,_Bound_,2,true>
	{ 
		static void Call(_Binding_ & binding, _Bound_ & bound) 
		{
			binding.Third = bound;
		}
	};

	template<typename _Binding_,typename _Bound_>
	struct Write<_Binding_,_Bound_,3,true>
	{ 
		static void Call(_Binding_ & binding, _Bound_ & bound) 
		{
			binding.Fourth = bound;
		}
	};

	template<typename _Binding_,typename _Bound_>
	struct Write<_Binding_,_Bound_,4,true>
	{ 
		static void Call(_Binding_ & binding, _Bound_ & bound) 
		{
			binding.Fifth = bound;
		}
	};

	template<typename _Binding_,typename _Bound_,int _Idx_>
	struct Write<_Binding_,_Bound_,_Idx_,false>
	{
		static void Call(...){};	
	};

	template<typename _Debug_, int _Id_ = 0>
	struct DebugType
	{

		enum EnumDebug {Debug = false};

	};

	template<int _Debug_, int _Id_ = 0>
	struct DebugValue
	{

		enum EnumDebug {Debug = false};

	};

	template<int _Idx_,
		int _Bound_, int _BoundIdx_, typename _BoundList_,
		int _Unbound_, int _UnboundIdx_, typename _UnboundList_>
	struct Rebind
	{

		typedef bool Type;

	};

	template<int _Idx_,
		int _Bound_, int _BoundIdx_, typename _BoundHead_, typename _BoundTail_,
		int _Unbound_, int _UnboundIdx_, typename _UnboundHead_, typename _UnboundTail_>
	struct Rebind<_Idx_,
		_Bound_,_BoundIdx_,Binder<_BoundHead_,_BoundTail_>,
		_Unbound_,_UnboundIdx_,Binder<_UnboundHead_,_UnboundTail_> >
	{
		enum Enum 
		{

			Unbinding = (bool)(_Unbound_&((1<<4)>>_Idx_)),
		};

		typedef typename Binder<			
			typename Conditional::If<(Unbinding==1),
				_UnboundHead_,
				_BoundHead_>::Type,
			typename Rebind<_Idx_+1,

				_Bound_,_BoundIdx_+1,

				typename Conditional::If<Logical::None<_BoundTail_>::Value,Binder<None,None>,_BoundTail_>::Type,
				_Unbound_,(Unbinding)?_UnboundIdx_+1:_UnboundIdx_,

				typename Conditional::If<(Unbinding==1),
					typename Conditional::If<Logical::None<_UnboundTail_>::Value,Binder<None,None>,_UnboundTail_>::Type,
					Binder<_UnboundHead_,_UnboundTail_> 
					>::Type
				>::Type
			>::Type Type;

	};

	template<
		int _Bound_, int _BoundIdx_, typename _BoundHead_, typename _BoundTail_,
		int _Unbound_, int _UnboundIdx_, typename _UnboundHead_, typename _UnboundTail_>
	struct Rebind<5,
		_Bound_,_BoundIdx_,Binder<_BoundHead_,_BoundTail_>,
		_Unbound_,_UnboundIdx_,Binder<_UnboundHead_,_UnboundTail_> >
	{
		typedef None Type;

		DebugValue<_BoundIdx_,9001> BoundIdxDebug;
		DebugValue<_UnboundIdx_,8001> UnboundIdxDebug;

		DebugType<_BoundHead_,2001> BoundHeadDebug;
		DebugType<_BoundTail_,2002> BoundTailDebug;

		DebugType<_UnboundHead_,1001> UnboundHeadDebug;
		DebugType<_UnboundTail_,1002> UnboundTailDebug;
	};

	Bind()
	{
	}

	template<typename _Bound1_,typename _Bound2_>
	Bind(_Bound1_ bound1, _Bound2_ bound2)
	{

	}

	template<typename _Bound1_>
	Bind(_Bound1_ bound1)
	{
		enum Enum 
		{ 
			First = Reason::Generic::Conditional::Mask<2>::IndexOf<Bound,0x1,1,0>::Value 
		};

		OutputConsole("%d\n",First);

	}

	template<typename _Unbound1_,typename _Unbound2_>
	_Return_ operator() (_Unbound1_ unbound1,_Unbound2_ unbound2)
	{

		typedef Binder<_Arg1_,Binder<_Arg2_,Binder<_Arg3_,Binder<_Arg4_,Binder<_Arg5_,None> > > > > BoundList;

		typedef Binder<_Unbound1_,Binder<_Unbound2_,None> > UnboundList;

		StaticAssert((Logical::Equals<typename At<BoundList,0>::Type,None>::Value==true));
		StaticAssert((Logical::Equals<typename At<BoundList,1>::Type,double>::Value==true));

		StaticAssert((Logical::Equals<typename At<UnboundList,0>::Type,_Unbound1_>::Value==true));
		StaticAssert((Logical::Equals<typename At<UnboundList,1>::Type,_Unbound2_>::Value==true));
		StaticAssert((Logical::Equals<typename At<UnboundList,2>::Type,None>::Value==true));

		typedef typename Rebind<0,Bound,0,BoundList,Unbound,0,UnboundList >::Type ArgList;

        typedef Binding<typename At<ArgList,0>::Type,typename At<ArgList,1>::Type,typename At<ArgList,2>::Type,typename At<ArgList,3>::Type,typename At<ArgList,4>::Type > Unbinding;

		Unbinding unbinding;
		StaticAssert((Logical::Equals<typename At<ArgList,0>::Type,bool>::Value==false));
		StaticAssert((Logical::Equals<typename At<ArgList,0>::Type,_Unbound1_>::Value==true));
		StaticAssert((Logical::Equals<typename At<ArgList,0>::Type,int>::Value==true));

		StaticAssert((Logical::Equals<typename At<ArgList,1>::Type,None>::Value==false));
		StaticAssert((Logical::Equals<typename At<ArgList,1>::Type,_Unbound2_>::Value==true));
		StaticAssert((Logical::Equals<typename At<ArgList,1>::Type,double>::Value==true));

		return Null<_Return_>::Value();
	}

	template<typename _Unbound1_>
	_Return_ operator() (_Unbound1_ unbound1)
	{
		enum Enum 
		{
			First = Reason::Generic::Conditional::Mask<2>::IndexOf<Unbound,0x1,1,0>::Value
		};

		OutputConsole("%d\n",First);

		switch(Unbound)
		{
		case 0x00000010: return Callback(unbound1,Args.Second);	
		case 0x00000008: 
			{
				typedef Reason::Messaging::Callback<_Return_,_Arg1_,_Unbound1_> Caller; 
				Caller caller;

				return ((Caller&)Callback)(Args.First,unbound1);	

			}
		}

	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename _Callback_,typename _Return_,typename _Arg1_=None,typename _Arg2_=None,typename _Arg3_=None,typename _Arg4_=None,typename _Arg5_=None>
class Delayed
{
public:

	template<typename _Arg_>
	struct Param
	{
		_Arg_ Arg;
		Param(_Arg_ arg):Arg(arg) {}		
	};

	template<typename _Arg_>
	struct Param<_Arg_&>
	{
		_Arg_ Arg;
		Param(_Arg_ arg):Arg(arg) {}
	};

	Param<_Arg1_> First;
	Param<_Arg2_> Second;
	Param<_Arg3_> Third;

	_Callback_ Callback;

	Delayed(const Param<_Arg1_> & param1, const Param<_Arg2_> & param2, const Param<_Arg3_> & param3):
		First(param1),Second(param2),Third(param3)		
	{

	}

	_Return_ operator() ()
	{
		Callback(First.Arg,Second.Arg,Third.Arg);
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

