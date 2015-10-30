
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
#ifndef SYSTEM_NUMBER_H
#define SYSTEM_NUMBER_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/string.h"
#include "reason/system/exception.h"
#include "reason/system/primitive.h"
#include "reason/structure/array.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;
using namespace Reason::Structure;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Prime
{
public:

	static bool IsPrime(int x)
	{
		int i=0;
		int j=NumberCount;

		while (i < j)
		{
			int pivot = (j-i)/2;
			if (Number[pivot] == x)
			{
				return true;
			}
			else
			if (Number[pivot] < x)
			{
				i = pivot;
			}
			else
			if (Number[pivot] > x)
			{
				j = pivot;
			}
		}

		return false;
	}

	static const int NumberCount;
	static const int Number[];

	struct PowerOffset
	{
        const int Offset[10];

		int operator[] (int index)
		{
			return Offset[index];
		}
	};

	static const int PowerCount;
	static const PowerOffset Power[];

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Float
{
public:

	static float Infinity()
	{
		static float inf = 1.0/atof("0.0");
		return inf;
	}

	static float Nan()
	{
		static float nan = 0.0/atof("0.0");
		return nan;
	}

	static const float PositiveZero;
	static const float NegativeZero;
	static const float PositiveInfinity;
	static const float NegativeInfinity;
	static const float NotANumber;
};

class Double
{
public:

	static double Infinity()
	{
		static double inf = 1.0/atof("0.0");
		return inf;
	}

	static double Nan()
	{
		static double nan = 0.0/atof("0.0");
		return nan;
	}

	static const double PositiveZero;
	static const double NegativeZero;
	static const double PositiveInfinity;
	static const double NegativeInfinity;
	static const double NotANumber;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Numbers
{
public:

	static int Max(signed x, signed y)		{return x > y ? x : y;}
	static int Max(unsigned x, unsigned y)	{return x > y ? x : y;}
	static float Max(float x, float y)		{return x > y ? x : y;}
	static double Max(double x, double y)	{return x > y ? x : y;}

	static int Min(signed x, signed y)		{return x < y ? x : y;}
	static int Min(unsigned x, unsigned y)	{return x < y ? x : y;}
	static float Min(float x, float y)		{return x < y ? x : y;}
	static double Min(double x, double y)	{return x < y ? x : y;}

	static int Abs(int x)					{return abs(x);}
	static float Abs(float x)				{return abs(x);}
	static double Abs(double x)				{return abs(x);}

	static float Ceil(float x)				{return ceil(x);}
	static double Ceil(double x)			{return ceil(x);}

	static float Floor(float x)				{return floor(x);}
	static double Floor(double x)			{return floor(x);}

	static float Sign(float x)				{return x > 0.0 ? 1.0 : -1.0;}
	static double Sign(double x)			{return x > 0.0 ? 1.0 : -1.0;}

	static float Log(float x)				{return log10(x);}
	static double Log(double x)				{return log10(x);}

	static float Ln(float x)				{return log(x);}
	static double Ln(double x)				{return log(x);}

	static float Pow(float x, float y)		{return pow(x,y);}
	static double Pow(double x, double y)	{return pow(x,y);}

	static float Round(float x)				{return (x-(int)x>=0.5)?ceil(x):floor(x);}
	static double Round(double x)			{return (x-(int)x>=0.5)?ceil(x):floor(x);}

	static long Random();

	static double Zero()
	{
		return 0.0;
	}

	static double PositiveZero()
	{
		return Double::PositiveZero;
	}

	static double NegativeZero()
	{
		return Double::NegativeZero;
	}

	static double Infinity()
	{

		static const double inf = (double)HUGE_VAL;

		return inf;
	}

	static double PositiveInfinity()
	{
		return Double::PositiveInfinity;
	}

	static double NegativeInfinity()
	{
		return Double::NegativeInfinity;
	}

	static signed short MinimumSignedShort()
	{
		static const short minimum = pow((double)2,(int)(sizeof(short)*8-1));
		return minimum;
	}

	static signed short MaximumSignedShort()
	{
		static const short maximum = pow((double)2,(int)(sizeof(short)*8-1))-1;
		return maximum;
	}

	static unsigned short MaximumUnsignedShort()
	{
		static const unsigned short maximum = pow((double)2,(int)(sizeof(short)*8))-1;
		return maximum;
	}		

	static signed MinimumSigned()
	{
		static const int minimum = pow((double)2,(int)(sizeof(int)*8-1));
		return minimum;
	}

	static signed MaximumSigned()
	{
		static const int maximum = pow((double)2,(int)(sizeof(int)*8-1))-1;

		return maximum;
	}

	static unsigned MaximumUnsigned()
	{
		static const unsigned int maximum = pow((double)2,(int)(sizeof(int)*8))-1;

		return maximum;
	}

	static signed int MinimumSignedInt() {return MinimumSigned();}
	static signed int MaximumSignedInt() {return MaximumSigned();}
	static unsigned int MaximumUnsignedInt() {return MaximumUnsigned();}

	static signed long MinimumSignedLong()
	{
		static const long minimum = pow((double)2,(int)(sizeof(long)*8-1));
		return minimum;
	}

	static signed long MaximumSignedLong()
	{
		static const long maximum = pow((double)2,(int)(sizeof(long)*8-1))-1;
		return maximum;
	}

	static unsigned long MaximumUnsignedLong()
	{
		static const unsigned long maximum = pow((double)2,(int)(sizeof(long)*8))-1;
		return maximum;
	}	

	static signed long long MinimumSignedLongLong()
	{
		static const long long minimum = (1LL<<(sizeof(long long)*8-1));
		return minimum;
	}

	static signed long long MaximumSignedLongLong()
	{

		static const long long maximum = ~(1LL<<(sizeof(long long)*8-1));
		return maximum;
	}

	static unsigned long long MaximumUnsignedLongLong()
	{
		static const unsigned long long maximum = ~0LL;
		return maximum;
	}	

	static float MaximumFloat()
	{

		static const float maximum = (float)3.40282347e+38; 
		return maximum;
	}

	static double MaximumDouble()
	{

		static const double maximum = (double)1.7976931348623157e+308;	
		return maximum;
	}

	static bool IsInfinity(float x)				{return x == Infinity() || x == -Infinity();}
	static bool IsInfinity(double x)			{return x == Infinity() || x == -Infinity();}

	static bool IsZero(float x)					{return IsPositiveZero(x) || IsNegativeZero(x);}
	static bool IsZero(double x)				{return IsPositiveZero(x) || IsNegativeZero(x);}

	static bool IsPositiveInfinity(float x)		{return x == Infinity();}
	static bool IsPositiveInfinity(double x)	{return x == Infinity();}

	static bool IsPositiveZero(float x)			{return x == 0.0;}
	static bool IsPositiveZero(double x)		{return x == 0.0;}

	static bool IsPositive(int x)				{return x > 0;}
	static bool IsPositive(float x)				{return IsPositiveZero(x) || IsPositiveInfinity(x) || x > 0.0;}
	static bool IsPositive(double x)			{return IsPositiveZero(x) || IsPositiveInfinity(x) || x > 0.0;}

	static bool IsNegativeInfinity(float x)		{return x == -Infinity();}
	static bool IsNegativeInfinity(double x)	{return x == -Infinity();}

	static bool IsNegativeZero(float x)			{return x == -0.0;}
	static bool IsNegativeZero(double x)		{return x == -0.0;}

	static bool IsNegative(int x)				{return x < 0;}
	static bool IsNegative(float x)				{return IsNegativeZero(x) || IsNegativeInfinity(x) || x < 0.0;}
	static bool IsNegative(double x)			{return IsNegativeZero(x) || IsNegativeInfinity(x) || x < 0.0;}

	static bool IsFinite(double x)				{return !IsNan(x) && !IsInfinity(x);}

	static double Nan()
	{

		static const double nan = sqrt(-2.01);

		return nan;
	}

	static bool IsNan(float x)
	{
		return x != x;
	}

	static bool IsNan(double x)
	{
		return x != x;
	}

	static int Compare(float left, float right, float epsilon = 0.00001)
	{
		float result = left-right;
		return (Abs(result)<epsilon)?0:(result<0?-1:1);
	}

	static int Compare(double left, double right, double epsilon = 0.00001)
	{
		double result = left-right;
		return (Abs(result)<epsilon)?0:(result<0?-1:1);
	}

	static int Compare(signed int left, signed int right)
	{
		return left-right;
	}

	static int Compare(unsigned int left, unsigned int right)
	{
		return left-right;
	}

	static int Compare(signed long long left, signed long long right)
	{
		return left-right;
	}

	static int Compare(unsigned long long left, unsigned long long right)
	{
		return left-right;
	}

	static bool Equals(float left, float right, float epsilon = 0.00001) {return Compare(left,right,epsilon)==0;}
	static bool Equals(double left, double right, double epsilon = 0.00001) {return Compare(left,right,epsilon)==0;}
	static bool Equals(signed left, signed right) {return Compare(left,right)==0;}
	static bool Equals(unsigned left, unsigned right) {return Compare(left,right)==0;}

	static const char * Suffix(int number)
	{

		if (number > 10 && number < 20) number = 0;
		switch (number%10)
		{
			case 0: return "th";break;
			case 1: return "st";break;
			case 2: return "nd";break;
			case 3: return "rd";break;
			case 4: return "th";break;
			case 5: return "th";break;
			case 6: return "th";break;
			case 7: return "th";break;
			case 8: return "th";break;
			case 9: return "th";break;
		}
		return "";
	}

	static Superstring Binary(int number)
	{
		Superstring string;
		if (number == 0)
			string << "0";

		while (number > 0)
		{
			bool bit = (number&0x00000001);
			number >>= 1;
			string >> ((bit)?'1':'0');
		}

		return string;
	}

	static Superstring Hex(int number)
	{
		Superstring string;

		if (number == 0)
			string << "0";

		while (number > 0)
		{
			char hex = (number&0x0000000F);
			number >>= 4;

			if (hex >=0 && hex <= 9)
				string >> (char)(hex+0x30);
			else
				string >> (char)(hex+0x37);
		}

		return string;
	}

	static int String(int number, int base, char * data, int size, bool padded=false);
	static Superstring String(int number, int base);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Number: public Object, public Numbers
{
public:

	static Identity Instance;
	virtual Identity& Identify(){return Instance;};

public:

	enum NumberTypes
	{
		TYPE_UNSIGNED	=(1)<<0,
		TYPE_SIGNED		=(1)<<1,		

		TYPE_SHORT		=(1)<<2,
		TYPE_INT		=(1)<<3,
		TYPE_LONG		=(1)<<4,
		TYPE_LONGLONG	=(1)<<5,

		TYPE_FLOAT		=(1)<<6,
		TYPE_DOUBLE		=(1)<<7,

		TYPE_INTEGER	= TYPE_SHORT|TYPE_INT|TYPE_LONG|TYPE_LONGLONG,
		TYPE_REAL		= TYPE_FLOAT|TYPE_DOUBLE,
	};

	union NumberStorage
	{

		NumberStorage(){};
		NumberStorage(signed short x):Short(x){};
		NumberStorage(unsigned short x):Short(x){};
		NumberStorage(signed int x):Int(x){};
		NumberStorage(unsigned int x):Int(x){};
		NumberStorage(signed long x):Long(x){};
		NumberStorage(unsigned long x):Long(x){};
		NumberStorage(signed long long x):LongLong(x){};
		NumberStorage(unsigned long long x):LongLong(x){};
		NumberStorage(float x):Float(x){};
		NumberStorage(double x):Double(x){};

		short Short;
		int Int;
		long Long;
		long long LongLong;
		float Float;
		double Double;

	} Data;

	int Size;
	Bitmask8 Type;

	Number():Data(0),Size(sizeof(int)),Type(0)
	{

	}

	Number(const Number &number)
	{
		Data = number.Data;
		Type = number.Type;
	}

	Number(unsigned short value):Data(value),Size(sizeof(short)),Type(TYPE_UNSIGNED|TYPE_SHORT){}
	Number(signed short value):Data(value),Size(sizeof(short)),Type(TYPE_SIGNED|TYPE_SHORT) {}
	Number(unsigned int value):Data(value),Size(sizeof(int)),Type(TYPE_UNSIGNED|TYPE_INT){}
	Number(signed int value):Data(value),Size(sizeof(int)),Type(TYPE_SIGNED|TYPE_INT) {}
	Number(unsigned long value):Data(value),Size(sizeof(long)),Type(TYPE_UNSIGNED|TYPE_LONG){}
	Number(signed long value):Data(value),Size(sizeof(long)),Type(TYPE_SIGNED|TYPE_LONG) {}
	Number(unsigned long long value):Data(value),Size(sizeof(long long)),Type(TYPE_UNSIGNED|TYPE_LONGLONG){}
	Number(signed long long value):Data(value),Size(sizeof(long long)),Type(TYPE_SIGNED|TYPE_LONGLONG) {}
	Number(float value):Data(value),Size(sizeof(float)),Type(TYPE_FLOAT) {}
	Number(double value):Data(value),Size(sizeof(double)),Type(TYPE_DOUBLE) {}

	int Hash()
	{
		return SignedInt();
	}

	void Print(Reason::System::String & string)
	{

		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: string.Append(Data.Short);break;
		case TYPE_INT: string.Append(Data.Int);break;
		case TYPE_LONG: string.Append(Data.Long);break;
		case TYPE_LONGLONG: string.Append(Data.LongLong);break;
		case TYPE_FLOAT: string.Append(Data.Float); break;
		case TYPE_DOUBLE: string.Append(Data.Double); break;
		}	
	}

	signed long long SignedLongLong()
	{
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: return (signed long long)Data.Short;
		case TYPE_INT: return (signed long long)Data.Int;
		case TYPE_LONG: return (signed long long)Data.Long;
		case TYPE_LONGLONG: return (signed long long)Data.LongLong;
		case TYPE_FLOAT: return (signed long long)Data.Float;
		case TYPE_DOUBLE: return (signed long long)Data.Double;
		}	
		return 0;
	}

	unsigned long long UnsignedLongLong()
	{
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: return (unsigned long long)Data.Short;
		case TYPE_INT: return (unsigned long long)Data.Int;
		case TYPE_LONG: return (unsigned long long)Data.Long;
		case TYPE_LONGLONG: return (unsigned long long)Data.LongLong;
		case TYPE_FLOAT: return (unsigned long long)Data.Float;
		case TYPE_DOUBLE: return (unsigned long long)Data.Double;
		}	
		return 0;
	}

	signed long SignedLong() {return (signed long)SignedLongLong();}
	unsigned long UnsignedLong() {return (unsigned long)UnsignedLongLong();}		

	signed int SignedInt() {return (signed int)SignedLongLong();}
	unsigned int UnsignedInt() {return (unsigned int)UnsignedLongLong();}		

	signed short SignedShort() {return (signed short)SignedLongLong();}
	unsigned short UnsignedShort() {return (unsigned short)UnsignedLongLong();}		

	double Double()
	{
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: return (double)Data.Short;
		case TYPE_INT: return (double)Data.Int;
		case TYPE_LONG: return (double)Data.Long;
		case TYPE_LONGLONG: return (double)Data.LongLong;
		case TYPE_FLOAT: return (double)Data.Float;
		case TYPE_DOUBLE: return (double)Data.Double;
		}
		return 0.0;
	}

	float Float()
	{
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: return (float)Data.Short;
		case TYPE_INT: return (float)Data.Int;
		case TYPE_LONG: return (float)Data.Long;
		case TYPE_LONGLONG: return (float)Data.LongLong;
		case TYPE_FLOAT: return (float)Data.Float;
		case TYPE_DOUBLE: return (float)Data.Double;
		}
		return 0.0;
	}

	bool IsInteger()	{return Type.Is(TYPE_INTEGER);}	
	bool IsSigned()		{return Type.Is(TYPE_SIGNED);}
	bool IsUnsigned()	{return Type.Is(TYPE_UNSIGNED);}

	bool IsReal()		{return Type.Is(TYPE_REAL);}
	bool IsFloat()		{return Type.Is(TYPE_FLOAT);}
	bool IsDouble()		{return Type.Is(TYPE_DOUBLE);}

	using Numbers::IsNan;
	using Numbers::IsInfinity;
	using Numbers::IsPositiveInfinity;
	using Numbers::IsNegativeInfinity;
	using Numbers::IsZero;
	using Numbers::IsPositiveZero;
	using Numbers::IsNegativeZero;
	using Numbers::IsFinite;

	bool IsNan()
	{

		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_FLOAT:
			return (Float() != Float());
		case TYPE_DOUBLE:	
			return (Double() != Double());
		default: return false;
		}		
	}

	bool IsInfinity()
	{
		return IsPositiveInfinity() || IsNegativeInfinity();
	}

	bool IsPositiveInfinity()
	{
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_FLOAT:
			return Float() == Infinity();
		case TYPE_DOUBLE:	
			return Double() == Infinity();
		default: return false;
		}		
	}

	bool IsNegativeInfinity()
	{
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_FLOAT:
			return Float() == -Infinity();
		case TYPE_DOUBLE:	
			return Double() == -Infinity();
		default: return false;
		}		
	}

	bool IsZero()
	{
		return IsPositiveZero()||IsNegativeZero();
	}

	bool IsPositiveZero()
	{
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_FLOAT:
			return Float() == 0.0;
		case TYPE_DOUBLE:	
			return Double() == 0.0;
		default: return false;
		}	
	}

	bool IsNegativeZero()
	{
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_FLOAT:
			return Float() == -0.0;
		case TYPE_DOUBLE:	
			return Double() == -0.0;
		default: return false;
		}	
	}

	bool IsFinite()
	{
		return !IsNan() && !IsInfinity();
	}

	using Numbers::Compare;
	int Compare(Object * object,int comparitor)
	{
		if (object->Identify() == Instance)
		{			

			switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
			{
			case TYPE_SHORT:	
				return Type.Is(TYPE_UNSIGNED)?
					((unsigned short)Data.Short - ((Number*)object)->UnsignedShort()):
					((signed short)Data.Short - ((Number*)object)->SignedShort());
			case TYPE_INT:	
				return Type.Is(TYPE_UNSIGNED)?
					((unsigned int)Data.Int - ((Number*)object)->UnsignedInt()):
					((signed int)Data.Int - ((Number*)object)->SignedInt());
			case TYPE_LONG:	
				return Type.Is(TYPE_UNSIGNED)?
						((unsigned long)Data.Long - ((Number*)object)->UnsignedLong()):
						((signed long)Data.Long - ((Number*)object)->SignedLong());
			case TYPE_LONGLONG:	
				return Type.Is(TYPE_UNSIGNED)?
						((unsigned long long)Data.LongLong - ((Number*)object)->UnsignedLongLong()):
						((signed long long)Data.LongLong - ((Number*)object)->SignedLongLong());		

			case TYPE_FLOAT: return Numbers::Compare(Data.Float,((Number*)object)->Float());
			case TYPE_DOUBLE: return Numbers::Compare(Data.Double,((Number*)object)->Double());
			}
		}
		else
		if (object->Identify() == Sequence::Instance)
		{

			switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
			{
			case TYPE_SHORT:	
				return Type.Is(TYPE_UNSIGNED)?
					((unsigned short)Data.Short - ((Sequence*)object)->Integer()):
					((signed short)Data.Short - ((Sequence*)object)->Integer());
			case TYPE_INT:	
				return Type.Is(TYPE_UNSIGNED)?
					((unsigned int)Data.Int - ((Sequence*)object)->Integer()):
					((signed int)Data.Int - ((Sequence*)object)->Integer());
			case TYPE_LONG:	
				return Type.Is(TYPE_UNSIGNED)?
						((unsigned long)Data.Long - ((Sequence*)object)->Integer()):
						((signed long)Data.Long - ((Sequence*)object)->Integer());
			case TYPE_LONGLONG:	
				return Type.Is(TYPE_UNSIGNED)?
						((unsigned long long)Data.LongLong - ((Sequence*)object)->Integer()):
						((signed long long)Data.LongLong - ((Sequence*)object)->Integer());		

			case TYPE_FLOAT: return Numbers::Compare(Data.Float,(float)((Sequence*)object)->Real());
			case TYPE_DOUBLE: return Numbers::Compare(Data.Double,(double)((Sequence*)object)->Real());
			}			
		}

		return Identity::Error;
	}

	using Numbers::Equals;

	Number & operator = (signed long long x) {return operator = (Number(x));}
	Number & operator = (unsigned long long x) {return operator = (Number(x));}
	Number & operator = (signed long x) {return operator = (Number(x));}
	Number & operator = (unsigned long x) {return operator = (Number(x));}	
	Number & operator = (signed int x) {return operator = (Number(x));}
	Number & operator = (unsigned int x) {return operator = (Number(x));}
	Number & operator = (signed short x) {return operator = (Number(x));}
	Number & operator = (unsigned short x) {return operator = (Number(x));}
	Number & operator = (float x) {return operator = (Number(x));}
	Number & operator = (double x) {return operator = (Number(x));}
	Number & operator = (const Number & number) {return operator = ((Number&)number);}
	Number & operator = (Number & number)
	{
		if (!Type)
		{
			Type=number.Type;
			Data=number.Data;
			Size=number.Size;
		}
		else
		{

			Number & n = (Number&)number;

			switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
			{
			case TYPE_SHORT: Data.Short = (n.IsSigned()?n.SignedShort():n.UnsignedShort()); break;
			case TYPE_INT: Data.Int = (n.IsSigned()?n.SignedInt():n.UnsignedInt()); break;
			case TYPE_LONG: Data.Long = (n.IsSigned()?n.SignedLong():n.UnsignedLong()); break;
			case TYPE_LONGLONG: Data.LongLong = (n.IsSigned()?n.SignedLongLong():n.UnsignedLongLong()); break;
			case TYPE_FLOAT: Data.Float = n.Float(); break;
			case TYPE_DOUBLE: Data.Double = n.Double(); break;
			} 
		}

		return *this;
	}

	bool operator == (signed long long x) {return operator == (Number(x));}
	bool operator == (unsigned long long x) {return operator == (Number(x));}
	bool operator == (signed long x) {return operator == (Number(x));}
	bool operator == (unsigned long x) {return operator == (Number(x));}	
	bool operator == (signed int x) {return operator == (Number(x));}
	bool operator == (unsigned int x) {return operator == (Number(x));}
	bool operator == (signed short x) {return operator == (Number(x));}
	bool operator == (unsigned short x) {return operator == (Number(x));}
	bool operator == (float x) {return operator == (Number(x));}
	bool operator == (double x) {return operator == (Number(x));}
	bool operator == (const Number & number) {return operator == (((Number&)number));}
	bool operator == (Number & number) 
	{
		Number & n = (Number&)number;
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: return Data.Short == n.Data.Short; break;
		case TYPE_INT: return Data.Int == n.Data.Int; break;
		case TYPE_LONG: return Data.Long == n.Data.Long; break;
		case TYPE_LONGLONG: return Data.LongLong == n.Data.LongLong; break;
		case TYPE_FLOAT: return Numbers::Equals(Data.Float,n.Float()); break;
		case TYPE_DOUBLE: return Numbers::Equals(Data.Double,n.Double()); break;
		}
		return false;
	}

	bool operator <= (signed long long x) {return operator <= (Number(x));}
	bool operator <= (unsigned long long x) {return operator <= (Number(x));}
	bool operator <= (signed long x) {return operator <= (Number(x));}
	bool operator <= (unsigned long x) {return operator <= (Number(x));}	
	bool operator <= (signed int x) {return operator <= (Number(x));}
	bool operator <= (unsigned int x) {return operator <= (Number(x));}
	bool operator <= (signed short x) {return operator <= (Number(x));}
	bool operator <= (unsigned short x) {return operator <= (Number(x));}
	bool operator <= (float x) {return operator <= (Number(x));}
	bool operator <= (double x) {return operator <= (Number(x));}
	bool operator <= (const Number & number) {return operator <= ((Number&)number);}
	bool operator <= (Number & number)
	{
		Number & n = (Number&)number;
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: return (IsSigned()?SignedShort():UnsignedShort()) <= (n.IsSigned()?n.SignedShort():n.UnsignedShort()); break;
		case TYPE_INT: return (IsSigned()?SignedInt():UnsignedInt()) <= (n.IsSigned()?n.SignedInt():n.UnsignedInt()); break;
		case TYPE_LONG: return (IsSigned()?SignedLong():UnsignedLong()) <= (n.IsSigned()?n.SignedLong():n.UnsignedLong()); break;
		case TYPE_LONGLONG: return (IsSigned()?SignedLongLong():UnsignedLongLong()) <= (n.IsSigned()?n.SignedLongLong():n.UnsignedLongLong()); break;
		case TYPE_FLOAT: return Numbers::Compare(Data.Float,n.Float()) <= 0; break;
		case TYPE_DOUBLE: return Numbers::Compare(Data.Double,n.Double()) <= 0; break;
		}
		return false;
	}

	bool operator < (signed long long x) {return operator < (Number(x));}
	bool operator < (unsigned long long x) {return operator < (Number(x));}
	bool operator < (signed long x) {return operator < (Number(x));}
	bool operator < (unsigned long x) {return operator < (Number(x));}	
	bool operator < (signed int x) {return operator < (Number(x));}
	bool operator < (unsigned int x) {return operator < (Number(x));}
	bool operator < (signed short x) {return operator < (Number(x));}
	bool operator < (unsigned short x) {return operator < (Number(x));}
	bool operator < (float x) {return operator < (Number(x));}
	bool operator < (double x) {return operator < (Number(x));}
	bool operator < (const Number & number) {return operator < ((Number&)number);}
	bool operator < (Number & number)
	{
		Number & n = (Number&)number;
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: return (IsSigned()?SignedShort():UnsignedShort()) < (n.IsSigned()?n.SignedShort():n.UnsignedShort()); break;
		case TYPE_INT: return (IsSigned()?SignedInt():UnsignedInt()) < (n.IsSigned()?n.SignedInt():n.UnsignedInt()); break;
		case TYPE_LONG: return (IsSigned()?SignedLong():UnsignedLong()) < (n.IsSigned()?n.SignedLong():n.UnsignedLong()); break;
		case TYPE_LONGLONG: return (IsSigned()?SignedLongLong():UnsignedLongLong()) < (n.IsSigned()?n.SignedLongLong():n.UnsignedLongLong()); break;
		case TYPE_FLOAT: return Numbers::Compare(Data.Float,n.Float()) < 0; break;
		case TYPE_DOUBLE: return Numbers::Compare(Data.Double,n.Double()) < 0; break;
		}
		return false;
	}

	bool operator >= (signed long long x) {return operator >= (Number(x));}
	bool operator >= (unsigned long long x) {return operator >= (Number(x));}
	bool operator >= (signed long x) {return operator >= (Number(x));}
	bool operator >= (unsigned long x) {return operator >= (Number(x));}	
	bool operator >= (signed int x) {return operator >= (Number(x));}
	bool operator >= (unsigned int x) {return operator >= (Number(x));}
	bool operator >= (signed short x) {return operator >= (Number(x));}
	bool operator >= (unsigned short x) {return operator >= (Number(x));}
	bool operator >= (float x) {return operator >= (Number(x));}
	bool operator >= (double x) {return operator >= (Number(x));}
	bool operator >= (const Number & number) {return operator >= ((Number&)number);}
	bool operator >= (Number & number)
	{
		Number & n = (Number&)number;
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: return (IsSigned()?SignedShort():UnsignedShort()) >= (n.IsSigned()?n.SignedShort():n.UnsignedShort()); break;
		case TYPE_INT: return (IsSigned()?SignedInt():UnsignedInt()) >= (n.IsSigned()?n.SignedInt():n.UnsignedInt()); break;
		case TYPE_LONG: return (IsSigned()?SignedLong():UnsignedLong()) >= (n.IsSigned()?n.SignedLong():n.UnsignedLong()); break;
		case TYPE_LONGLONG: return (IsSigned()?SignedLongLong():UnsignedLongLong()) >= (n.IsSigned()?n.SignedLongLong():n.UnsignedLongLong()); break;
		case TYPE_FLOAT: return Numbers::Compare(Data.Float,n.Float()) >= 0; break;
		case TYPE_DOUBLE: return Numbers::Compare(Data.Double,n.Double()) >= 0; break;
		}
		return false;
	}

	bool operator > (signed long long x) {return operator > (Number(x));}
	bool operator > (unsigned long long x) {return operator > (Number(x));}
	bool operator > (signed long x) {return operator > (Number(x));}
	bool operator > (unsigned long x) {return operator > (Number(x));}	
	bool operator > (signed int x) {return operator > (Number(x));}
	bool operator > (unsigned int x) {return operator > (Number(x));}
	bool operator > (signed short x) {return operator > (Number(x));}
	bool operator > (unsigned short x) {return operator > (Number(x));}
	bool operator > (float x) {return operator > (Number(x));}
	bool operator > (double x) {return operator > (Number(x));}
	bool operator > (const Number & number) {return operator > ((Number&)number);}
	bool operator > (Number & number)
	{
		Number & n = (Number&)number;
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: return (IsSigned()?SignedShort():UnsignedShort()) > (n.IsSigned()?n.SignedShort():n.UnsignedShort()); break;
		case TYPE_INT: return (IsSigned()?SignedInt():UnsignedInt()) > (n.IsSigned()?n.SignedInt():n.UnsignedInt()); break;
		case TYPE_LONG: return (IsSigned()?SignedLong():UnsignedLong()) > (n.IsSigned()?n.SignedLong():n.UnsignedLong()); break;
		case TYPE_LONGLONG: return (IsSigned()?SignedLongLong():UnsignedLongLong()) > (n.IsSigned()?n.SignedLongLong():n.UnsignedLongLong()); break;
		case TYPE_FLOAT: return Numbers::Compare(Data.Float,n.Float()) > 0; break;
		case TYPE_DOUBLE: return Numbers::Compare(Data.Double,n.Double()) > 0; break;
		}
		return false;
	}

	Number operator - (signed long long x) {return operator - (Number(x));}
	Number operator - (unsigned long long x) {return operator - (Number(x));}
	Number operator - (signed long x) {return operator - (Number(x));}
	Number operator - (unsigned long x) {return operator - (Number(x));}	
	Number operator - (signed int x) {return operator - (Number(x));}
	Number operator - (unsigned int x) {return operator - (Number(x));}
	Number operator - (signed short x) {return operator - (Number(x));}
	Number operator - (unsigned short x) {return operator - (Number(x));}
	Number operator - (float x) {return operator - (Number(x));}
	Number operator - (double x) {return operator - (Number(x));}
	Number operator - (const Number & number) {return operator - ((Number&)number);}
	Number operator - (Number & number)
	{
		Number & n = (Number&)number;
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: return (IsSigned()?SignedShort():UnsignedShort()) - (n.IsSigned()?n.SignedShort():n.UnsignedShort()); break;
		case TYPE_INT: return (IsSigned()?SignedInt():UnsignedInt()) - (n.IsSigned()?n.SignedInt():n.UnsignedInt()); break;
		case TYPE_LONG: return (IsSigned()?SignedLong():UnsignedLong()) - (n.IsSigned()?n.SignedLong():n.UnsignedLong()); break;
		case TYPE_LONGLONG: return (IsSigned()?SignedLongLong():UnsignedLongLong()) - (n.IsSigned()?n.SignedLongLong():n.UnsignedLongLong()); break;
		case TYPE_FLOAT: return Data.Float - n.Float(); break;
		case TYPE_DOUBLE: return Data.Double - n.Double(); break;
		}
		return 0;
	}

	Number & operator -= (signed long long x) {return operator -= (Number(x));}
	Number & operator -= (unsigned long long x) {return operator -= (Number(x));}
	Number & operator -= (signed long x) {return operator -= (Number(x));}
	Number & operator -= (unsigned long x) {return operator -= (Number(x));}	
	Number & operator -= (signed int x) {return operator -= (Number(x));}
	Number & operator -= (unsigned int x) {return operator -= (Number(x));}
	Number & operator -= (signed short x) {return operator -= (Number(x));}
	Number & operator -= (unsigned short x) {return operator -= (Number(x));}
	Number & operator -= (float x) {return operator -= (Number(x));}
	Number & operator -= (double x) {return operator -= (Number(x));}
	Number & operator -= (const Number & number) {return operator -= ((Number&)number);}
	Number & operator -= (Number & number)
	{
		Number & n = (Number&)number;
		if (!Type) Type = number.Type;
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: Data.Short = (IsSigned()?SignedShort():UnsignedShort()) - (n.IsSigned()?n.SignedShort():n.UnsignedShort()); break;
		case TYPE_INT: Data.Int = (IsSigned()?SignedInt():UnsignedInt()) - (n.IsSigned()?n.SignedInt():n.UnsignedInt()); break;
		case TYPE_LONG: Data.Long = (IsSigned()?SignedLong():UnsignedLong()) - (n.IsSigned()?n.SignedLong():n.UnsignedLong()); break;
		case TYPE_LONGLONG: Data.LongLong = (IsSigned()?SignedLongLong():UnsignedLongLong()) - (n.IsSigned()?n.SignedLongLong():n.UnsignedLongLong()); break;
		case TYPE_FLOAT: Data.Float -= n.Float(); break;
		case TYPE_DOUBLE: Data.Double -= n.Double(); break;
		}
		return *this;
	}

	Number operator + (signed long long x) {return operator + (Number(x));}
	Number operator + (unsigned long long x) {return operator + (Number(x));}
	Number operator + (signed long x) {return operator + (Number(x));}
	Number operator + (unsigned long x) {return operator + (Number(x));}	
	Number operator + (signed int x) {return operator + (Number(x));}
	Number operator + (unsigned int x) {return operator + (Number(x));}
	Number operator + (signed short x) {return operator + (Number(x));}
	Number operator + (unsigned short x) {return operator + (Number(x));}
	Number operator + (float x) {return operator + (Number(x));}
	Number operator + (double x) {return operator + (Number(x));}
	Number operator + (const Number & number) {return operator + ((Number&)number);}
	Number operator + (Number & number)
	{
		Number & n = (Number&)number;
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: return (IsSigned()?SignedShort():UnsignedShort()) + (n.IsSigned()?n.SignedShort():n.UnsignedShort()); break;
		case TYPE_INT: return (IsSigned()?SignedInt():UnsignedInt()) + (n.IsSigned()?n.SignedInt():n.UnsignedInt()); break;
		case TYPE_LONG: return (IsSigned()?SignedLong():UnsignedLong()) + (n.IsSigned()?n.SignedLong():n.UnsignedLong()); break;
		case TYPE_LONGLONG: return (IsSigned()?SignedLongLong():UnsignedLongLong()) + (n.IsSigned()?n.SignedLongLong():n.UnsignedLongLong()); break;
		case TYPE_FLOAT: return Data.Float + n.Float(); break;
		case TYPE_DOUBLE: return Data.Double + n.Double(); break;
		}
		return 0;
	}

	Number & operator += (signed long long x) {return operator += (Number(x));}
	Number & operator += (unsigned long long x) {return operator += (Number(x));}
	Number & operator += (signed long x) {return operator += (Number(x));}
	Number & operator += (unsigned long x) {return operator += (Number(x));}	
	Number & operator += (signed int x) {return operator += (Number(x));}
	Number & operator += (unsigned int x) {return operator += (Number(x));}
	Number & operator += (signed short x) {return operator += (Number(x));}
	Number & operator += (unsigned short x) {return operator += (Number(x));}
	Number & operator += (float x) {return operator += (Number(x));}
	Number & operator += (double x) {return operator += (Number(x));}
	Number & operator += (const Number & number) {return operator += ((Number&)number);}
	Number & operator += (Number & number)
	{
		Number & n = (Number&)number;
		if (!Type) Type = n.Type;
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: Data.Short = (IsSigned()?SignedShort():UnsignedShort()) + (n.IsSigned()?n.SignedShort():n.UnsignedShort()); break;
		case TYPE_INT: Data.Int = (IsSigned()?SignedInt():UnsignedInt()) + (n.IsSigned()?n.SignedInt():n.UnsignedInt()); break;
		case TYPE_LONG: Data.Long = (IsSigned()?SignedLong():UnsignedLong()) + (n.IsSigned()?n.SignedLong():n.UnsignedLong()); break;
		case TYPE_LONGLONG: Data.LongLong = (IsSigned()?SignedLongLong():UnsignedLongLong()) + (n.IsSigned()?n.SignedLongLong():n.UnsignedLongLong()); break;
		case TYPE_FLOAT: Data.Float += n.Float(); break;
		case TYPE_DOUBLE: Data.Double += n.Double(); break;
		}
		return *this;
	}

	Number operator / (signed long long x) {return operator / (Number(x));}
	Number operator / (unsigned long long x) {return operator / (Number(x));}
	Number operator / (signed long x) {return operator / (Number(x));}
	Number operator / (unsigned long x) {return operator / (Number(x));}	
	Number operator / (signed int x) {return operator / (Number(x));}
	Number operator / (unsigned int x) {return operator / (Number(x));}
	Number operator / (signed short x) {return operator / (Number(x));}
	Number operator / (unsigned short x) {return operator / (Number(x));}
	Number operator / (float x) {return operator / (Number(x));}
	Number operator / (double x) {return operator / (Number(x));}
	Number operator / (const Number & number) {return operator / ((Number&)number);}
	Number operator / (Number & number)
	{
		Number & n = (Number&)number;
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: return (IsSigned()?SignedShort():UnsignedShort()) / (n.IsSigned()?n.SignedShort():n.UnsignedShort()); break;
		case TYPE_INT: return (IsSigned()?SignedInt():UnsignedInt()) / (n.IsSigned()?n.SignedInt():n.UnsignedInt()); break;
		case TYPE_LONG: return (IsSigned()?SignedLong():UnsignedLong()) / (n.IsSigned()?n.SignedLong():n.UnsignedLong()); break;
		case TYPE_LONGLONG: return (IsSigned()?SignedLongLong():UnsignedLongLong()) / (n.IsSigned()?n.SignedLongLong():n.UnsignedLongLong()); break;
		case TYPE_FLOAT: return Data.Float / n.Float(); break;
		case TYPE_DOUBLE: return Data.Double / n.Double(); break;
		}
		return 0;
	}

	Number & operator /= (signed long long x) {return operator /= (Number(x));}
	Number & operator /= (unsigned long long x) {return operator /= (Number(x));}
	Number & operator /= (signed long x) {return operator /= (Number(x));}
	Number & operator /= (unsigned long x) {return operator /= (Number(x));}	
	Number & operator /= (signed int x) {return operator /= (Number(x));}
	Number & operator /= (unsigned int x) {return operator /= (Number(x));}
	Number & operator /= (signed short x) {return operator /= (Number(x));}
	Number & operator /= (unsigned short x) {return operator /= (Number(x));}
	Number & operator /= (float x) {return operator /= (Number(x));}
	Number & operator /= (double x) {return operator /= (Number(x));}
	Number & operator /= (const Number & number) {return operator /= ((Number&)number);}
	Number & operator /= (Number & number)
	{
		Number & n = (Number&)number;
		if (!Type) Type = n.Type;
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: Data.Short = (IsSigned()?SignedShort():UnsignedShort()) / (n.IsSigned()?n.SignedShort():n.UnsignedShort()); break;
		case TYPE_INT: Data.Int = (IsSigned()?SignedInt():UnsignedInt()) / (n.IsSigned()?n.SignedInt():n.UnsignedInt()); break;
		case TYPE_LONG: Data.Long = (IsSigned()?SignedLong():UnsignedLong()) / (n.IsSigned()?n.SignedLong():n.UnsignedLong()); break;
		case TYPE_LONGLONG: Data.LongLong = (IsSigned()?SignedLongLong():UnsignedLongLong()) / (n.IsSigned()?n.SignedLongLong():n.UnsignedLongLong()); break;
		case TYPE_FLOAT: Data.Float /= n.Float(); break;
		case TYPE_DOUBLE: Data.Double /= n.Double(); break;
		}
		return *this;
	}

	Number operator * (signed long long x) {return operator * (Number(x));}
	Number operator * (unsigned long long x) {return operator * (Number(x));}
	Number operator * (signed long x) {return operator * (Number(x));}
	Number operator * (unsigned long x) {return operator * (Number(x));}	
	Number operator * (signed int x) {return operator * (Number(x));}
	Number operator * (unsigned int x) {return operator * (Number(x));}
	Number operator * (signed short x) {return operator * (Number(x));}
	Number operator * (unsigned short x) {return operator * (Number(x));}
	Number operator * (float x) {return operator * (Number(x));}
	Number operator * (double x) {return operator * (Number(x));}
	Number operator * (const Number & number) {return operator * ((Number&)number);}
	Number operator * (Number & number)
	{
		Number & n = (Number&)number;
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: return (IsSigned()?SignedShort():UnsignedShort()) * (n.IsSigned()?n.SignedShort():n.UnsignedShort()); break;
		case TYPE_INT: return (IsSigned()?SignedInt():UnsignedInt()) * (n.IsSigned()?n.SignedInt():n.UnsignedInt()); break;
		case TYPE_LONG: return (IsSigned()?SignedLong():UnsignedLong()) * (n.IsSigned()?n.SignedLong():n.UnsignedLong()); break;
		case TYPE_LONGLONG: return (IsSigned()?SignedLongLong():UnsignedLongLong()) * (n.IsSigned()?n.SignedLongLong():n.UnsignedLongLong()); break;
		case TYPE_FLOAT: return Data.Float * n.Float(); break;
		case TYPE_DOUBLE: return Data.Double * n.Double(); break;
		}

		return 0;
	}

	Number & operator *= (signed long long x) {return operator *= (Number(x));}
	Number & operator *= (unsigned long long x) {return operator *= (Number(x));}
	Number & operator *= (signed long x) {return operator *= (Number(x));}
	Number & operator *= (unsigned long x) {return operator *= (Number(x));}	
	Number & operator *= (signed int x) {return operator *= (Number(x));}
	Number & operator *= (unsigned int x) {return operator *= (Number(x));}
	Number & operator *= (signed short x) {return operator *= (Number(x));}
	Number & operator *= (unsigned short x) {return operator *= (Number(x));}
	Number & operator *= (float x) {return operator *= (Number(x));}
	Number & operator *= (double x) {return operator *= (Number(x));}
	Number & operator *= (const Number & number) {return operator *= ((Number&)number);}
	Number & operator *= (Number & number)
	{
		Number & n = (Number&)number;
		if (!Type) Type = n.Type;
		switch ((Type.Bits&(TYPE_INTEGER|TYPE_REAL)))
		{
		case TYPE_SHORT: Data.Short = (IsSigned()?SignedShort():UnsignedShort()) * (n.IsSigned()?n.SignedShort():n.UnsignedShort()); break;
		case TYPE_INT: Data.Int = (IsSigned()?SignedInt():UnsignedInt()) * (n.IsSigned()?n.SignedInt():n.UnsignedInt()); break;
		case TYPE_LONG: Data.Long = (IsSigned()?SignedLong():UnsignedLong()) * (n.IsSigned()?n.SignedLong():n.UnsignedLong()); break;
		case TYPE_LONGLONG: Data.LongLong = (IsSigned()?SignedLongLong():UnsignedLongLong()) * (n.IsSigned()?n.SignedLongLong():n.UnsignedLongLong()); break;
		case TYPE_FLOAT: Data.Float *= n.Float(); break;
		case TYPE_DOUBLE: Data.Double *= n.Double(); break;
		}
		return *this;
	}

	operator signed int () {return SignedInt();}

	operator double ()			{return Double();}

	friend bool operator  == (signed int x, Number & number) {return x == number.SignedInt();}

	Superstring Suffixed()
	{
		return Suffixed(*this);
	}

	static Superstring Suffixed(Number & number)
	{
		Superstring string;
		if (number.IsSigned())
			string << number.SignedInt() << Suffix(number.SignedInt());
		else
		if (number.IsUnsigned())
			string << (int)number.UnsignedInt() << Suffix(number.UnsignedInt());
		else
			string << "";
		return string;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Magnitude
{
public:

	static int Abs(int x)				{return (x >= 0) ? x : -x;}
	static int Max(int x, int y)		{return Abs(x) > Abs(y) ? x : y;}
	static int Min(int x, int y)		{return Abs(x) < Abs(y) ? x : y;}
	static int Compare(int x, int y)	{return Abs(x) - Abs(y);}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Frequency : public Reason::Structure::Array<Number>
{
public:

	double Sum();
	double Min();
	double Max();
	double Mean();
	double Mode();
	double Variance();
	double Deviation();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Statistics
{
public:

	Number Min;
	Number Total;
	int Count;
	Number Max;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Composite
{
public:

	Number Numerator;
	Number Denominator;
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

