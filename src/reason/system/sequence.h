
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
#ifndef SYSTEM_SEQUENCE_H
#define SYSTEM_SEQUENCE_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/object.h"
#include "reason/system/output.h"
#include "reason/system/interface.h"

using namespace Reason::System;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Language { namespace Regex {

	class Regex;

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Sequences
{
public:

	static int Compare(const char * left, const char * right, bool caseless){return (caseless)?CompareCaseless(left,right):Compare(left,right);}
	static int Compare(const char * left, const char * right, const int size, bool caseless){return (caseless)?CompareCaseless(left,right,size):Compare(left,right,size);}
	static int Compare(const char * left, const int leftSize, const char * right, int rightSize, bool caseless){return (caseless)?CompareCaseless(left,leftSize,right,rightSize):Compare(left,leftSize,right,rightSize);}

	static int Compare(const char * left, const char * right);
	static int Compare(const char * left, const char * right, const int size);
	static int Compare(const char * left, const int leftSize, const char * right, int rightSize);

	static int CompareCaseless(const char * left, const char * right);
	static int CompareCaseless(const char * left, const char * right, const int size);
	static int CompareCaseless(const char * left, const int leftSize, const char * right, int rightSize);

	static int CompareReverse(const char * left, const char * right, bool caseless){return (caseless)?CompareReverseCaseless(left,right):CompareReverse(left,right);}
	static int CompareReverse(const char * left, const char * right, const int size, bool caseless){return (caseless)?CompareReverseCaseless(left,right,size):CompareReverse(left,right,size);}
	static int CompareReverse(const char * left, const int leftSize, const char * right, int rightSize, bool caseless){return (caseless)?CompareReverseCaseless(left,leftSize,right,rightSize):CompareReverse(left,leftSize,right,rightSize);}

	static int CompareReverse(const char * left, const char * right);
	static int CompareReverse(const char * left, const char * right, const int size);
	static int CompareReverse(const char * left, const int leftSize, const char * right, int rightSize);

	static int CompareReverseCaseless(const char * left, const char * right);
	static int CompareReverseCaseless(const char * left, const char * right, const int size);
	static int CompareReverseCaseless(const char * left, const int leftSize, const char * right, int rightSize);

public:

	static bool Equals(const char * left, const char * right, bool caseless){return (caseless)?EqualsCaseless(left,right):Equals(left,right);}
	static bool Equals(const char * left, const char * right, const int size, bool caseless){return (caseless)?EqualsCaseless(left,right,size):Equals(left,right,size);}
	static bool Equals(const char * left, const int leftSize, const char * right, const int rightSize, bool caseless){return (caseless)?EqualsCaseless(left,leftSize,right,rightSize):Equals(left,leftSize,right,rightSize);}

	static bool Equals(const char * left, const char * right);
	static bool Equals(const char * left, const char * right, const int size);
	static bool Equals(const char * left, const int leftSize, const char * right, const int rightSize);

	static bool EqualsCaseless(const char * left, const char * right);
	static bool EqualsCaseless(const char * left, const char * right, const int size);
	static bool EqualsCaseless(const char * left, const int leftSize, const char * right, int rightSize);

	static bool EqualsReverse(const char * left, const char * right, bool caseless){return (caseless)?EqualsReverseCaseless(left,right):EqualsReverse(left,right);}
	static bool EqualsReverse(const char * left, const char * right, const int size, bool caseless){return (caseless)?EqualsReverseCaseless(left,right,size):EqualsReverse(left,right,size);}
	static bool EqualsReverse(const char * left, const int leftSize, const char * right, const int rightSize, bool caseless){return (caseless)?EqualsReverseCaseless(left,leftSize,right,rightSize):EqualsReverse(left,leftSize,right,rightSize);}

	static bool EqualsReverse(const char * left, const char * right);
	static bool EqualsReverse(const char * left, const char * right, const int size);
	static bool EqualsReverse(const char * left, const int leftSize, const char * right, int rightSize);

	static bool EqualsReverseCaseless(const char * left, const char * right);
	static bool EqualsReverseCaseless(const char * left, const char * right, const int size);
	static bool EqualsReverseCaseless(const char * left, const int leftSize, const char * right, int rightSize);

public:

	static char * Search(const char * left, const char * right, bool caseless){return (caseless)?SearchCaseless(left,right):Search(left,right);}
	static char * Search(const char * left, const char * right, const int size, bool caseless){return (caseless)?SearchCaseless(left,right,size):Search(left,right,size);}
	static char * Search(const char * left, const int leftSize, const char * right, const int rightSize, bool caseless){return (caseless)?SearchCaseless(left,leftSize,right,rightSize):Search(left,leftSize,right,rightSize);}

	static char * Search(const char * left, const char * right);
	static char * Search(const char * left, const char * right, const int size);
	static char * Search(const char * left, const int leftSize, const char * right, const int rightSize);

	static char * SearchCaseless(const char * left, const char * right);
	static char * SearchCaseless(const char * left, const char * right, const int size);
	static char * SearchCaseless(const char * left, const int leftSize, const char * right, int rightSize);

	static char * SearchReverse(const char * left, const char * right, bool caseless){return (caseless)?SearchReverseCaseless(left,right):SearchReverse(left,right);}
	static char * SearchReverse(const char * left, const char * right, const int size, bool caseless){return (caseless)?SearchReverseCaseless(left,right,size):SearchReverse(left,right,size);}
	static char * SearchReverse(const char * left, const int leftSize, const char * right, int rightSize, bool caseless){return (caseless)?SearchReverseCaseless(left,leftSize,right,rightSize):SearchReverse(left,leftSize,right,rightSize);}
	static char * SearchReverse(const char * left, const char * right);
	static char * SearchReverse(const char * left, const char * right, const int size);
	static char * SearchReverse(const char * left, const int leftSize, const char * right, int rightSize);

	static char * SearchReverseCaseless(const char * left, const char * right);
	static char * SearchReverseCaseless(const char * left, const char * right, const int size);
	static char * SearchReverseCaseless(const char * left, const int leftSize, const char * right, int rightSize);

public:

	static int Length(const char * data);

	static int Hash(const char * data){return Hash((char*)data,Length(data));}
	static int Hash(char * data, int size);

	static int HashCaseless(const char * data){return HashCaseless((char*)data,Length(data));}
	static int HashCaseless(char * data, int size);

	static long long Binary(const char * data){return Binary((char*)data,Length(data));}
	static long long Binary(char * data,int size);

	static long long Integer(int radix, const char * data){return Integer(radix,(char*)data,Length(data));}
	static long long Integer(int radix, char * data, int size);

	static long long Integer(const char * data){return Integer((char*)data,Length(data));}
	static long long Integer(char * data, int size);

	static double Real(const char * data){return Real((char*)data,Length(data));}
	static double Real(char * data, int size);	

	static long long Octal(const char * data){return Octal((char*)data,Length(data));}
	static long long Octal(char * data, int size);

	static long long Hex(const char * data){return Hex((char*)data,Length(data));}
	static long long Hex(char * data, int size);

	static long long Decimal(const char * data){return Decimal((char*)data,Length(data));}
	static long long Decimal(char * data, int size);

	static bool IsHex(const char * data){return IsHex((char*)data,Length(data));}
	static bool IsHex(char * data, int size);

	static bool IsOctal(const char * data){return IsOctal((char*)data,Length(data));}
	static bool IsOctal(char * data, int size);

	static bool IsDecimal(const char * data){return IsNumeric((char*)data,Length(data));}
	static bool IsDecimal(char * data, int size){return IsNumeric(data,size);}

	static bool IsBinary(const char * data){return IsBinary((char*)data,Length(data));}
	static bool IsBinary(char * data, int size);

	static bool IsProppercase(const char * data){return IsProppercase((char*)data,Length(data));}
	static bool IsProppercase(char * data, int size);

	static bool IsUppercase(const char * data){return IsUppercase((char*)data,Length(data));}
	static bool IsUppercase(char * data, int size);

	static bool IsLowercase(const char * data){return IsLowercase((char*)data,Length(data));}
	static bool IsLowercase(char *data, int size);

	static bool IsAlpha(const char * data){return IsAlpha((char*)data,Length(data));}
	static bool IsAlpha(char * data, int size);

	static bool IsNumeric(const char * data){return IsNumeric((char*)data,Length(data));}
	static bool IsNumeric(char * data, int size);

	static bool IsAlphanumeric(const char * data){return IsAlphanumeric((char*)data,Length(data));}
	static bool IsAlphanumeric(char * data, int size);

	static void Lowercase(const char * data){Lowercase((char*)data,Length(data));}
	static void Lowercase(char * data, int size);

	static void Uppercase(const char * data){Uppercase((char*)data,Length(data));}
	static void Uppercase(char *data, int size);

	static void Proppercase(const char * data){Proppercase((char*)data,Length(data));}
	static void Proppercase(char * data, int size);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Sequence : public Reason::System::Object, public Sequences
{
public:

	static Identity Instance;
	virtual Identity& Identify(){return Instance;};

public:

	char	*Data;		
	int		Size;		

	explicit Sequence();
	explicit Sequence(const char *data);
	explicit Sequence(const Sequence & sequence);
	explicit Sequence(char *data, int size);

	Sequence & operator = (const Sequence & sequence);

	virtual ~Sequence()=0;

	using Sequences::Length;
	int Length() {return Size;}

	bool IsEmpty()
	{

		OutputAssert(Size >= 0);

		return (Data)?Size==0:true;
	}

	bool IsNull()
	{
		return (Data == 0);
	}

	char CharAt(int index);

	char & ReferenceAt(int index);
	char * PointerAt(int index);

	int IndexAt(char * data)
	{

		return (data && data >= Data && data < Data+Size)?data-Data:-1;
	}

	bool IsProppercase()	{return Sequences::IsProppercase(Data, Size);}
	bool IsUppercase()		{return Sequences::IsUppercase(Data, Size);}
	bool IsLowercase()		{return Sequences::IsLowercase(Data, Size);}
	bool IsAlpha()			{return Sequences::IsAlpha(Data, Size);}
	bool IsNumeric()		{return Sequences::IsNumeric(Data, Size);}
	bool IsAlphanumeric()	{return Sequences::IsAlphanumeric(Data, Size);}

	long long Octal()		{return Sequences::Octal(Data,Size);}
	long long Hex()			{return Sequences::Hex(Data,Size);}
	long long Decimal()		{return Sequences::Decimal(Data,Size);}
	long long Binary()		{return Sequences::Binary(Data,Size);}

	long long Integer(int radix=10)		{return Sequences::Integer(radix,Data,Size);}
	double Real()						{return Sequences::Real(Data,Size);}

	int IndexOf(int from, const char c, bool caseless = false);
	int IndexOf(const char c, bool caseless = false) {return IndexOf(0,c,caseless);};

	virtual int IndexOf(const int from, char * data, int size, bool caseless = false);
	int IndexOf(int from, const char * data, bool caseless = false) {return IndexOf(from,(char*)data,Length(data),caseless);};
	int IndexOf(int from, const Sequence & sequence, bool caseless = false)  {return IndexOf(from,sequence.Data,sequence.Size,caseless);};;
	int IndexOf(char * data, int size, bool caseless = false) {return IndexOf(0,data,size,caseless);};
	int IndexOf(const char * data, bool caseless = false) {return IndexOf(0,(char*)data,strlen(data),caseless);};
	int IndexOf(const Sequence & sequence, bool caseless = false) {return IndexOf(0,sequence.Data,sequence.Size,caseless);};

	int LastIndexOf(int from, const char c, bool caseless = false);
	int LastIndexOf(const char c, bool caseless = false) {return LastIndexOf(Size-1,c,caseless);};

	virtual int LastIndexOf(int from, char * data, int size, bool caseless = false);
	int LastIndexOf(int from, const char * data, bool caseless = false) {return LastIndexOf(from, (char*)data,Length(data),caseless);};
	int LastIndexOf(int from, const Sequence & sequence, bool caseless = false) {return LastIndexOf(from, sequence.Data,sequence.Size,caseless);};
	int LastIndexOf(char * data, int size, bool caseless = false) {return LastIndexOf(Size-1, data,size,caseless);};
	int LastIndexOf(const char * data, bool caseless = false) {return LastIndexOf(Size-1, (char*)data,Length(data),caseless);};
	int LastIndexOf(const Sequence & sequence, bool caseless = false) {return LastIndexOf(Size-1, sequence.Data,sequence.Size,caseless);};

	bool Contains(char *data, int size, bool caseless = false);
	bool Contains(const char *data, bool caseless = false) {return Contains((char*)data,Length(data),caseless);};
	bool Contains(const Sequence & sequence, bool caseless = false) {return Contains(sequence.Data,sequence.Size,caseless);};
	bool Contains(const char c, bool caseless = false) {return IndexOf(c,caseless)!=-1;};

	int Match(const Sequence & sequence) {return Match(0,sequence.Data,sequence.Size);}
	int Match(const char * data) {return Match(0,(char*)data,Length(data));}
	int Match(char * data, int size) {return Match(0,data,size);}
	int Match(Reason::Language::Regex::Regex & regex) {return Match(0,regex);}
	int Match(int from, const Sequence & sequence) {return Match(from,sequence.Data,sequence.Size);}
	int Match(int from, const char * data) {return Match(from,(char*)data,Length(data));}
	int Match(int from, char * data, int size);
	int Match(int from, Reason::Language::Regex::Regex & regex);

	bool Is(char *data, int size, bool caseless = false);
	bool Is(const char *data, bool caseless = false) {return Is((char*)data,Length(data),caseless);};
	bool Is(const Sequence & sequence, bool caseless = false) {return Is(sequence.Data,sequence.Size,caseless);};
	bool Is(char data, bool caseless=false);

	bool StartsWith(char *prefix, int prefixSize, bool caseless = false);
	bool StartsWith(const char *prefix, bool caseless = false) {return StartsWith((char*)prefix,Length(prefix),caseless);};
	bool StartsWith(const Sequence &prefix, bool caseless = false) {return StartsWith(prefix.Data,prefix.Size,caseless);};
	bool StartsWith(const char prefix, bool caseless = false);

	bool EndsWith(char *suffix, int suffixSize, bool caseless = false);
	bool EndsWith(const char *suffix, bool caseless = false) {return EndsWith((char*)suffix,Length(suffix),caseless);};
	bool EndsWith(const Sequence &suffix, bool caseless = false) {return EndsWith(suffix.Data,suffix.Size,caseless);};
	bool EndsWith(const char suffix, bool caseless = false);

	bool IsSequenceOf(const Sequence & sequence) {return IsSequenceOf(sequence.Data,sequence.Size);}
	bool IsSequenceOf(char * data, int size)
	{
		return Data >= data && Data+Size <= data+size;
	}

	bool operator == (const Sequence & sequence) {return Is(sequence);}
	bool operator == (const char * data) {return Is(data);}

	bool operator < (const Sequence & sequence) {return Compare(sequence) < 0;}
	bool operator < (const char * data) {return Compare(data) < 0;}

	bool operator <= (const Sequence & sequence) {return Compare(sequence) <= 0;}
	bool operator <= (const char * data) {return Compare(data) <= 0;}

	bool operator > (const Sequence & sequence) {return Compare(sequence) > 0;}
	bool operator > (const char * data) {return Compare(data) > 0;}

	bool operator >= (const Sequence & sequence) {return Compare(sequence) >= 0;}
	bool operator >= (const char * data) {return Compare(data) >= 0;}

	int Compare(const Sequence & sequence, bool caseless=false) {return (caseless)?Sequences::CompareCaseless(Data,Size,sequence.Data,sequence.Size):Sequences::Compare(Data,Size,sequence.Data,sequence.Size);}
	int Compare(const char * data, bool caseless=false) {return (caseless)?Sequences::CompareCaseless(Data,Size,(char*)data,Length(data)):Sequences::Compare(Data,Size,(char*)data,Length(data));}
	int Compare(char * data, int size, bool caseless=false) {return (caseless)?Sequences::CompareCaseless(Data,Size,data,size):Sequences::Compare(Data,Size,data,size);}

	bool Equals(const Sequence & sequence, bool caseless=false) {return Is(sequence,caseless);}
	bool Equals(const char * data, bool caseless=false) {return Is(data,caseless);}
	bool Equals(char * data, int size, bool caseless=false) {return Is(data,size,caseless);}

	int Hash();
	int Compare(Reason::System::Object *object, int comparitor=COMPARE_GENERAL);
	bool Equals(Reason::System::Object * object, int comparitor=COMPARE_GENERAL);

	using Object::Print;
	void Print(String & string);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Strange
{

};

class Strand
{
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Point : public Reason::System::Object
{
public:

	int Offset;
	int Size;

	Point(const Sequence & sequence):Offset(0),Size(sequence.Size)
	{
	}

	~Point()
	{
	}

	Point & operator = (const Sequence & sequence)		
	{
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

