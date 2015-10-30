
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
#ifndef GENERIC_TRAITS_H
#define GENERIC_TRAITS_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/object.h"
#include "reason/generic/generic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;
using namespace Reason::Structure;

namespace Reason { namespace Generic {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Bitset
{

	template<unsigned int _Data_, unsigned int _Size_, unsigned int _Idx_, unsigned int _Off_>
	struct Left
	{	

		enum Enum { Value = Left<_Data_,_Size_,(_Data_&((1<<(_Size_-1))>>_Off_))?(_Idx_-1):((_Off_>_Size_)?-1:_Idx_),_Off_+1>::Value };

	};

	template<unsigned int _Data_, unsigned int _Size_, unsigned int _Off_>
	struct Left<_Data_,_Size_,0,_Off_>
	{ 
		enum Enum { Value = _Off_-1 }; 
	};

	template<unsigned int _Data_, unsigned int _Size_, unsigned int _Off_>
	struct Left<_Data_,_Size_,-1,_Off_>
	{ 
		enum Enum { Value = -1 };	
	};

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Conditional
{
	template<bool _Bool_, typename _True_, typename _False_>
	struct If 
	{ typedef None Type; };

	template<typename _True_, typename _False_>
	struct If<true,_True_,_False_> 
	{ typedef _True_ Type; };

	template<typename _True_, typename _False_>
	struct If<false,_True_,_False_>
	{ typedef _False_ Type; };

	template<int _Int_, typename _Case1_, typename _Case2_=None, typename _Case3_=None, typename _Case4_=None, typename _Case5_=None>
	struct Switch 
	{ typedef None Type; };

	template<typename _Case1_>
	struct Switch<1,_Case1_> 
	{ typedef _Case1_ Type; };

	template<typename _Case1_, typename _Case2_>
	struct Switch<2,_Case1_,_Case2_> 
	{ typedef _Case2_ Type; };

	template<bool _Bool_>
	struct Choose 
	{ 
		template <typename _True_, typename _False_>
		static None Value(_True_ t, _False_ f) {return None();}
	};

	template<>
	struct Choose<true> 
	{ 
		template <typename _True_, typename _False_>
		static _True_ Value(_True_ t, _False_ f) {return t;}
	};

	template<>
	struct Choose<false>
	{ 
		template<typename _True_, typename _False_>
		static _False_ Value(_True_ t, _False_ f) {return f;}
	};

		template<int _Tag_, int _Count_>
		struct Simple
		{

			enum Enum {Val = Simple<(_Count_==10)?1:0,(_Count_>=2)?_Count_+1:_Count_-1>::Val};

		};

		template<int _Count_>
		struct Simple<-1,_Count_>
		{

			enum Enum {Val = 0};
		};

		template<int _Count_>
		struct Simple<1,_Count_>
		{

			enum Enum {Val = 10};
		};

	template<int _Size_>
	struct Mask
	{

		typedef int Bits;

		const static int Bit = (int)(1<<(_Size_-1));

		template<int _Debug_, int _Id_ = 0>
		struct Debugger
		{

			enum EnumDebug {Debug = false};

		};

		template<int _Data_, int _Idx_, int _Off_>
		struct Left
		{	
			struct And
			{
				enum Enum { Value = (1<<(_Size_-1))>>_Off_ };
			};

			enum Enum { Value = Left<_Data_,(_Data_&And::Value)?(_Idx_-1):((_Off_>_Size_)?-1:_Idx_),_Off_+1>::Value };

		};

		template<int _Data_, int _Off_>
		struct Left<_Data_,0,_Off_>
		{ 
			enum Enum { Value = _Off_-1 }; 
		};

		template<int _Data_, int _Off_>
		struct Left<_Data_,-1,_Off_>

		{ 
			enum Enum { Value = -1 };	
		};

		template<int _Data_, int _Idx_, int _Off_>
		struct Right
		{	
			struct And
			{
				enum Enum { Value = (1<<_Off_) };
			};

			enum Enum { Value = Right<_Data_,(_Data_&And::Value)?(_Idx_-1):((_Off_>_Size_)?-1:_Idx_),_Off_+1>::Value };

		};

		template<int _Data_, int _Off_>
		struct Right<_Data_,0,_Off_>
		{ 
			enum Enum { Value = _Size_-_Off_ }; 
		};

		template<int _Data_, int _Off_>
		struct Right<_Data_,-1,_Off_>

		{ 
			enum Enum { Value = -1 };	
		};

		template<int _Data_, int _Bit_, int _Idx_, int _Off_>
		struct IndexOf
		{	

			struct Size
			{
				enum Enum { Value = Left<_Bit_,1,0>::Value };
			};

			struct And
			{

				enum Enum { Value = (_Bit_<<Left<_Bit_,1,0>::Value)>>_Off_ };
			};

			enum Enum { Value = IndexOf<_Data_,_Bit_,(_Off_>Size::Value)?-1:(((_Data_&And::Value) == And::Value)?_Idx_-1:_Idx_),_Off_+1>::Value };

		};

		template<int _Data_,int _Bit_, int _Off_>
		struct IndexOf<_Data_,_Bit_,0,_Off_>
		{
			enum Enum { Value = _Off_-1 };
		};

		template<int _Data_,int _Bit_, int _Off_>
		struct IndexOf<_Data_,_Bit_,-1,_Off_>
		{
			enum Enum { Value = -1 };
		};

		template<int _Data_,int _Idx_, int _Off_>
		struct IndexOf<_Data_,0,_Idx_,_Off_>
		{
			enum Enum { Value = -1 };
		};

		template<int _Data_, int _Bit_, int _Idx_, int _Off_>
		struct LastIndexOf
		{	
			struct Size
			{
				enum Enum { Value = Left<_Bit_,1,0>::Value };
			};

			struct And
			{
				enum Enum { Value = (_Bit_<<_Off_) };
			};

			enum Enum { Value = LastIndexOf<_Data_,_Bit_,(_Off_>Size::Value)?-1:(((_Data_&And::Value) == And::Value)?_Idx_-1:_Idx_),_Off_+1>::Value };
		};

		template<int _Data_,int _Bit_, int _Off_>
		struct LastIndexOf<_Data_,_Bit_,0,_Off_>
		{
			enum Enum { Value = _Off_-1 };
		};

		template<int _Data_,int _Bit_, int _Off_>
		struct LastIndexOf<_Data_,_Bit_,-1,_Off_>
		{
			enum Enum { Value = -1 };
		};

		template<int _Data_,int _Idx_, int _Off_>
		struct LastIndexOf<_Data_,0,_Idx_,_Off_>
		{
			enum Enum { Value = -1 };
		};

	};

}

namespace Support
{

};

namespace Logical
{
	template<typename _Left_, typename _Right_>
	struct Equals { enum Enum { Value = false};};

	template<typename _Left_>
	struct Equals<_Left_,_Left_> { enum Enum { Value = true};};

	template<typename _Kind_>
	struct Empty { enum Enum { Value = false}; };

	template<>
	struct Empty<Generic::Empty> { enum Enum { Value = true}; };

	template<typename _Kind_>
	struct None { enum Enum { Value = false}; };

	template<>
	struct None<Generic::None> { enum Enum { Value = true}; };

	template<bool _Bool1_, bool _Bool2_, bool _Bool3_=true, bool _Bool4_=true, bool _Bool5_=true>
	struct And { enum Enum {Value = false}; };

	template<>
	struct And<true, true, true, true, true>
	{ enum Enum {Value = true}; };

	template<bool _Bool1_, bool _Bool2_, bool _Bool3_=false, bool _Bool4_=false, bool _Bool5_=false>
	struct Or { enum Enum {Value = true}; };

	template<>
	struct Or<false, false, false, false, false>
	{ enum Enum {Value = false}; };

	template<bool _Bool_>
	struct Not { enum Enum {Value = false}; };

	template <>
	struct Not<false> { enum Enum {Value = true}; };

	template<typename _Kind_, typename _To_>
	struct Convertible
	{

		static typename Template<_Kind_>::Reference Kind();
		static char Conversion(typename Template<_To_>::ConstantReference to);
		static int Conversion(...);
		enum Enum {Value = (sizeof(Conversion(Kind())) != sizeof(int))};

	};

	template<typename _Kind_, typename _To_>
	struct Convertible<_Kind_,_To_*>
	{
		static typename Template<_Kind_>::Reference Kind();
		static char Conversion(_To_ * to);
		static int Conversion(...);
		enum Enum {Value = (sizeof(Conversion(Kind())) != sizeof(int))};
	};

	template<typename _Kind_>
	struct Convertible<_Kind_,void>
	{
		enum Enum {Value = false};
	};

	template<typename _To_>
	struct Convertible<void,_To_>
	{
		enum Enum {Value = false};
	};

	template<typename _Super_, typename _Sub_>
	struct Superclass
	{
		enum Enum {Value = Convertible<typename Type<_Super_>::ConstantPointer,typename Type<_Sub_>::ConstantPointer>::Value};
	};

	template<typename _Sub_, typename _Super_>
	struct Subclass
	{
		enum Enum {Value = Convertible<typename Type<_Super_>::ConstantPointer,typename Type<_Sub_>::ConstantPointer>::Value};
	};

 	template<typename _Kind_, typename _To_>
	struct Castable
	{
		static typename Type<_Kind_>::Pointer Kind();
		static char Cast(typename Type<_To_>::Pointer to);
		static int Cast(...);
		enum Enum {Value = (sizeof(Cast(Kind())) != sizeof(int))};
	};

	template<typename _Kind_>
	struct Castable<_Kind_,void> 
	{
		enum Enum {Value = false};
	};

	template<typename _To_>
	struct Castable<void,_To_> 
	{
		enum Enum {Value = false};
	};

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Functional
{
	template <typename Head, typename Tail>
	struct TypeList
	{
		typedef Head THead;
		typedef Tail TTail;
		typedef TypeList<Head, Tail> Type;
	};

	template 
	<
		typename T1 = None, typename T2 = None, typename T3 = None, typename T4 = None, 
		typename T5 = None, typename T6 = None, typename T7 = None, typename T8 = None,
		typename T9 = None, typename T10 = None, typename T11 = None, typename T12 = None,
		typename T13 = None, typename T14 = None, typename T15 = None, typename T16 = None
	>
	struct Make
	{
		typedef TypeList
		<
			T1, 
			typename Make<T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16>::Type
		> Type;
	};

	template<>
	struct Make<>
	{
		typedef None Type;
	};

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Traits
{

	template<typename _Kind_>
	struct Class
	{

		template<typename _Arg_>
		static char Member(void (_Arg_::*)());
		template<typename _Arg_>
		static int Member(...);

		#if defined(REASON_PLATFORM_APPLE) || defined(REASON_PLATFORM_LINUX)
		static const int Value = (sizeof(Member<_Kind_>(0)) != sizeof(int));
		#else

		#ifdef REASON_PLATFORM_WINDOWS
		enum Enum {Value = (sizeof(Member<_Kind_>(0)) != sizeof(int))};		
		#else
		static const int Value;
		#endif		
		#endif

	};

	#if !defined(REASON_PLATFORM_APPLE) && !defined(REASON_PLATFORM_LINUX) && !defined(REASON_PLATFORM_WINDOWS)
	template<typename _Kind_>
	const int Class<_Kind_>::Value = (sizeof(Class<_Kind_>::Member<_Kind_>(0)) != sizeof(int));
	#endif

	template<typename _Kind_>
	struct Pointer
	{
		enum Enum{Value = false};
	};

	template<typename _Kind_>
	struct Pointer<_Kind_*>
	{
		enum Enum{Value = true};
	};

	template<typename _Kind_>
	struct Reference
	{
		enum Enum{Value = false};
	};

	template<typename _Kind_>
	struct Reference<_Kind_&>
	{
		enum Enum{Value = true};
	};

	template<typename _Kind_>
	struct Primitive
	{
		enum Enum {Value = !Class<_Kind_>::Value};
	};

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Comparison
{

	template<typename _Kind_>
	struct Signature
	{
		enum Mask
		{

			Pointer		= 1,

			Object		= 4,
			String		= 8,
			Double		= 16,
		};

		enum Enum
		{
			Value = (Logical::Equals<_Kind_,double>::Value<<4)|
					(Logical::Equals<_Kind_,char*>::Value<<3)|
					(Logical::Castable<_Kind_,Reason::System::Object>::Value<<2)|

					(Traits::Pointer<_Kind_>::Value<<0)
		};

	};

	template<typename _Kind_, int _Mask_=Signature<_Kind_>::Value>
	struct Comparing
	{
		static int Compare(_Kind_ & left, _Kind_ & right, int comparitor=Comparable::COMPARE_GENERAL)
		{
			int mask = _Mask_;

			return left - right;
		}

	};

	template<typename _Kind_>
	struct Comparing<_Kind_,0>
	{
		static int Compare(const _Kind_ & left, const _Kind_ & right, int comparitor=Comparable::COMPARE_GENERAL)
		{
			if (comparitor == Comparable::COMPARE_INSTANCE) return &left-&right;		
			return left - right;
		}
	};

	template<typename _Kind_>
	struct Comparing<_Kind_,1>
	{
		static int Compare(_Kind_ left, _Kind_ right, int comparitor=Comparable::COMPARE_GENERAL)
		{
			return left - right;
		}
	};

	template<typename _Kind_>
	struct Comparing<_Kind_,4>
	{
		static int Compare(_Kind_ & left, _Kind_ & right, int comparitor=Comparable::COMPARE_GENERAL)
		{
			return left.Compare(&right,comparitor);
		}
	};

	template<typename _Kind_>
	struct Comparing<_Kind_,5>
	{
		static int Compare(_Kind_ left, _Kind_ right, int comparitor=Comparable::COMPARE_GENERAL)
		{
			return left->Compare(right,comparitor);
		}
	};

	template<typename _Kind_>
	struct Comparing<_Kind_,9>
	{
		static int Compare(_Kind_ left, _Kind_ right, int comparitor=Comparable::COMPARE_GENERAL)
		{
			int leftSize = strlen(left);
			int rightSize = strlen(right);
			return Sequences::Compare(left,right,(leftSize<rightSize)?leftSize:rightSize,comparitor);
		}
	};

	template<typename _Kind_>
	struct Comparing<_Kind_,16>
	{
		static int Compare(_Kind_ & left, _Kind_ & right, int comparitor=Comparable::COMPARE_GENERAL)
		{
			if (comparitor == Comparable::COMPARE_INSTANCE) return &left-&right;
			float compare = (float)(left-right);
			return *((int*)&compare);
		}
	};

}

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

