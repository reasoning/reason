
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
#ifndef SYSTEM_STRING_H
#define SYSTEM_STRING_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "reason/system/interface.h"
#include "reason/system/object.h"

#include "reason/system/indices.h"
#include "reason/system/output.h"
#include "reason/system/character.h"
#include "reason/system/primitive.h"
#include "reason/system/sequence.h"
#include "reason/platform/thread.h"

#include "reason/system/bit.h"

#ifndef min
#define min(x,y) (((x)<(y))?(x):(y))
#endif

#ifndef max
#define max(x,y) (((x)>(y))?(x):(y))
#endif

#include <stdlib.h>
#include <stdio.h>

using namespace Reason::System;
using namespace Reason::Platform;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Printer
{
private:

	static Reason::Platform::Critical Critical;
	static const int Size=4096;
	static char Data[];
	static char *QueuePointer;
	static int   QueueItemSize;

public:

	operator char * (void) {return QueuePointer;};

	static void Terminate()
	{
		#ifdef REASON_PLATFORM_WINDOWS

		static bool initialised = false;
		if (!initialised)
		{
			if ((Critical.Handle).DebugInfo == 0)
			{
				OutputError("Printer::Initialise - Critical not initialised, applying workaround.\n");
				Critical.Create();
				initialised = true;
			}
		}
		#endif
	}

	static void Finalise()
	{

	}

	static int QueueSpaceAvailable()
	{
		return Size-(QueuePointer-Data);
	}

	static char * Load(Sequence &string)
	{
		return Load(string.Data,string.Size);
	}

	static char * Load(const char *data, const int size);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Substring : public Sequence
{
public:

	Substring()
	{
	}

	Substring(const String & string);

	Substring(const Substring & substring):
		Sequence((Sequence&)substring)
	{
	}

	Substring(const Sequence & sequence):
		Sequence((Sequence&)sequence)
	{
	}

	Substring(const Sequence & sequence, int from, int to):
		Sequence(Substring(sequence).Slice(from,to))	
	{
	}

	Substring(const Sequence & sequence, int from):
		Sequence(Substring(sequence).Slice(from))
	{
	}

	Substring (const char * data):
		Sequence((char*)data, Sequence::Length(data))
	{
	}

	Substring (const char * data, int from, int to):
		Sequence(Substring((char*)data, Sequence::Length(data)).Slice(from,to))
	{
	}

	Substring (const char * data, int from):
		Sequence(Substring((char*)data, Sequence::Length(data)).Slice(from))
	{
	}

	Substring(char *data, int size):
		Sequence(data,size)
	{
	}

	~Substring()
	{
	}

	Substring & operator = (const char *data)				{Assign((char*)data,Length(data));return *this;}
	Substring & operator = (const Sequence & sequence)		{Assign(sequence.Data,sequence.Size);return *this;}
	Substring & operator = (const Substring & substring)	{Assign(substring.Data,substring.Size);return *this;}

	Substring & Assign(const char *data, const int size)	{Data = (char*)data;Size = (int)size;return *this;}
	Substring & Assign(const char *data)					{return Assign(data,Length(data));}
	Substring & Assign(const char *start,const char * end)	{return Assign(start,(end-start+1));}
	Substring & Assign(const Sequence & sequence)			{return Assign(sequence.Data,sequence.Size);}
	Substring & Assign(const Substring & substring)		{return Assign(substring.Data,substring.Size);}

	Substring & Trim(int amount);
	Substring & TrimLeft(int amount);
	Substring & TrimRight(int amount);
	Substring & Trim(const char * matches = " \t\n\r") {return Trim((char*)matches,Length(matches));}
	Substring & TrimLeft(const char * matches = " \t\n\r") {return TrimLeft((char*)matches,Length(matches));}
	Substring & TrimRight(const char * matches = " \t\n\r") {return TrimRight((char*)matches,Length(matches));}
	Substring & Trim(char * matches, int matchesSize);
	Substring & TrimLeft(char * matches, int matchesSize);
	Substring & TrimRight(char * matches, int matchesSize);

	Substring & Reverse();

	Substring & Left(int amount);
	Substring & Right(int amount);

	void Release()	{Data=0;Size=0;};

	virtual bool IsSubstring() {return true;}

	char * At(int index);

	Substring Before(char data) {return Before(0,&data,1);}
	Substring Before(const char * data) {return Before(0,data);}
	Substring Before(const Sequence & sequence) {return Before(0,sequence);}
	Substring Before(char * data, int size) {return Before(0,data,size);}
	Substring Before(int from, const char * data) {return Before(from,(char*)data,Length(data));}
	Substring Before(int from, const Sequence & sequence) {return Before(from,sequence.Data,sequence.Size);}

	Substring Before(int from, char * data, int size)	
	{
		if (Indices::Normalise(from,Size))
		{

			int index = IndexOf(from,data,size);
			if (index >= 0)
				return Slice(0,index);
		}

		return Reason::System::Substring();
	}	

	Substring Before(int index);

	Substring After(char data) {return After(0,&data,1);}
	Substring After(const char * data) {return After(0,data);}
	Substring After(const Sequence & sequence) {return After(0,sequence);}
	Substring After(char * data, int size) {return After(0,data,size);}
	Substring After(int from, const char * data) {return After(from,(char*)data,Length(data));}
	Substring After(int from, const Sequence & sequence) {return After(from,sequence.Data,sequence.Size);}
	Substring After(int from, char * data, int size);

	Substring After(int index);

	Substring Slice(int from, int to)
	{

		Indices::Normalise(from,Size);
		Indices::Normalise(to,Size);

		if (from >= 0 && from < Size && to > from)
			return Reason::System::Substring(Data+from,min(Size,to)-from);

		return Reason::System::Substring();

	}

	Substring Slice(int from);

	Reason::System::Superstring Superstring();		
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Superstring;

class String : public Substring, public virtual Reason::System::Disposable
{
public:

	int		Allocated;

	String & operator = (const Reason::System::Superstring & superstring) {Construct(superstring);return *this;}
	String & operator = (const String & data) {Construct(data.Data,data.Size);return *this;}
	String & operator = (const Sequence & data) {Construct(data.Data,data.Size);return *this;}
	String & operator = (const char * data) {Construct(data);return *this;}
	String & operator = (char c) {Construct(c);return *this;}
	String & operator = (int i) {Construct(i);return *this;}
	String & operator = (long l) {Construct(l);return *this;}
	String & operator = (long long ll) {Construct(ll);return *this;}
	String & operator = (float f) {Construct(f);return *this;}
	String & operator = (double d) {Construct(d);return *this;}

	String(const Reason::System::Superstring & superstring):Allocated(0) {Construct(superstring);}
	String(const String & string):Allocated(0) {Construct(string.Data,string.Size);}
	String(const Sequence & sequence):Allocated(0) {Construct(sequence.Data,sequence.Size);}
	String(const Sequence & sequence, int from, int to);
	String(const Sequence & sequence, int from);

	String(const char * data):Allocated(0) {Construct(data);}
	String(char * data, int size):Allocated(0) {Construct(data,size);}
	String(char c):Allocated(0) {Construct(c);}
	String(int i):Allocated(0) {Construct(i);}
	String(long l):Allocated(0) {Construct(l);}
	String(long long ll):Allocated(0) {Construct(ll);}
	String(float f):Allocated(0) {Construct(f);}
	String(double d):Allocated(0) {Construct(d);}

	String(int times, const Sequence & sequence) {Construct(times,sequence.Data,sequence.Size);}	
	String(int times, char * data, int size) {Construct(times,data,size);}

	String():Allocated(0) {}
	~String();

public:

	virtual String & Assign(char * data, int size);
	String & Assign(const Sequence & sequence) {return Assign(sequence.Data,sequence.Size);}

	String & Acquire(char *& data, int & size);
	String & Acquire(const String & string);
	String & Acquire(const Sequence & sequence);

	String & Construct(const Reason::System::Superstring & superstring);
	String & Construct(const String & string) {return Construct(string.Data,string.Size);}
	String & Construct(const Sequence &sequence) {return Construct(sequence.Data,sequence.Size);}

	String & Construct(const char * data) {return Construct((char*)data,Length(data));}
	String & Construct(char * data, int size);
	String & Construct(int i);
	String & Construct(long l) {return Construct((int)l);}
	String & Construct(long long ll);
	String & Construct(float f);
	String & Construct(double d);
	String & Construct(char c);
	String & Construct();

	String & Construct(int times, const Sequence & sequence) {return Construct(times,sequence.Data,sequence.Size);}	
	String & Construct(int times, char * data, int size);

	void Release();	
	void Destroy();	

	String & Append(char c);
	String & Append(short s);
	String & Append(int i);
	String & Append(long l) {return Append((int)l);}
	String & Append(long long ll);
	String & Append(double d);
	String & Append(float f);
	String & Append(const Sequence &sequence) {return Append(sequence.Data,sequence.Size); }
	String & Append(const char *data) {return Append((char*)data,Length(data)); }
	String & Append(char *data, int size);
	String & Append(const Reason::System::Superstring & superstring);

	String & operator += (const Reason::System::Superstring & superstring) {Append(superstring);return *this;}
	String & operator += (const Sequence & sequence) {Append(sequence);return *this;}
	String & operator += (const char * string) {Append(string);return *this;}
	String & operator += (char c) {Append(c);return *this;}
	String & operator += (short s) {Append(s);return *this;}
	String & operator += (int i) {Append(i);return *this;}
	String & operator += (long l) {Append(l);return *this;}
	String & operator += (long long ll) {Append(ll);return *this;}
	String & operator += (double d)	{Append(d);return *this;}
	String & operator += (float f) {Append(f);return *this;}

	String & operator *= (int times);

	String & operator << (const Reason::System::Superstring & superstring)	{Append(superstring);return *this;}
	String & operator << (const Sequence &sequence)	{Append(sequence);return *this;}
	String & operator << (const char * string) {Append(string);return *this;}
	String & operator << (char c) {Append(c);return *this;}
	String & operator << (short s) {Append(s);return *this;}
	String & operator << (int i) {Append(i);return *this;}
	String & operator << (long l) {Append(l);return *this;}
	String & operator << (long long ll) {Append(ll);return *this;}
	String & operator << (double d)	{Append(d);return *this;}
	String & operator << (float f) {Append(f);return *this;}

	String & Prepend(char c);
	String & Prepend(short s);
	String & Prepend(int i);
	String & Prepend(long l) {return Prepend((int)l);}
	String & Prepend(long long ll);
	String & Prepend(double d);
	String & Prepend(float f);
	String & Prepend(const Sequence &sequence) {return Prepend(sequence.Data,sequence.Size); }
	String & Prepend(const char *data) {return Prepend((char*)data,Length(data)); }
	String & Prepend(char *data, int size);
	String & Prepend(const Reason::System::Superstring & superstring);

	String & operator >> (const Reason::System::Superstring & superstring)	{Prepend(superstring);return *this;}	
	String & operator >> (const Sequence &sequence)	{Prepend(sequence);return *this;}
	String & operator >> (const char * data) {Prepend(data);return *this;}
	String & operator >> (char c) {Prepend(c);return *this;}
	String & operator >> (short s) {Prepend(s);return *this;}
	String & operator >> (int i) {Prepend(i);return *this;}
	String & operator >> (long l) {Prepend(l);return *this;}
	String & operator >> (long long ll) {Prepend(ll);return *this;}
	String & operator >> (double d) {Prepend(d);return *this;}
	String & operator >> (float f) {Prepend(f);return *this;}

	String & Insert(const Sequence & sequence, const int index) {return Insert(sequence.Data,sequence.Size,index);}
	String & Insert(const char *data, int index){if (data) Insert((char*)data,Length(data),index);return *this;}
	String & Insert(char *data, int size, int index);

	String & Insert(char c, int index) {Insert(&c,1,index);return *this;}
	String & Insert(short s, int index);
	String & Insert(int i, int index);
	String & Insert(long l, int index) {return Insert((int)l,index);}
	String & Insert(long long ll, int index);
	String & Insert(double d, int index);
	String & Insert(float f, int index);

	String & Format(const char * format,...);

	String & Replace(int i);
	String & Replace(long l) {Replace((int)l);return *this;}
	String & Replace(long long ll);
	String & Replace(double d);
	String & Replace(float f);
	String & Replace(const Sequence & sequence) {return Replace(sequence.Data,sequence.Size);}
	String & Replace(const char *data) {return Replace((char*)data,Length(data));}
	String & Replace(char * data, int size) {return Construct(data,size);}
	String & Replace(const Reason::System::Superstring & superstring) {return Construct(superstring);}

	String & Replace(int from, int to, const Sequence & sequence) {return Replace(from,to,sequence.Data,sequence.Size);}
	String & Replace(int from, int to, const char * data) {return Replace(from,to,(char*)data,Length(data));}
	String & Replace(int from, int to, char * data, int size);

	String & Replace(char match, char with) {return Replace(0,match,with);}
	String & Replace(const Sequence & match, const Sequence & with) {return Replace(0,match.Data,match.Size,with.Data,with.Size);}
	String & Replace(const Sequence & match, const char * with) {return Replace(0,match.Data,match.Size,(char*)with,Length(with));}
	String & Replace(const char * match, const char * with) {return Replace(0,(char*)match,Length(match),(char*)with,Length(with));}
	String & Replace(const char * match, const Sequence & with) {return Replace(0,(char*)match,Length(match),with.Data,with.Size);}
	String & Replace(char * match, int matchSize, char *with, int withSize) {return Replace(0,match,matchSize,with,withSize);} 

	String & Replace(int limit, char match, char with);	
	String & Replace(int limit, const Sequence & match, const Sequence & with) {return Replace(limit,match.Data,match.Size,with.Data,with.Size);}
	String & Replace(int limit, const Sequence & match, const char * with) {return Replace(limit,match.Data,match.Size,(char*)with,Length(with));}
	String & Replace(int limit, const char * match, const char * with) {return Replace(limit,(char*)match,Length(match),(char*)with,Length(with));}
	String & Replace(int limit, const char * match, const Sequence & with) {return Replace(limit,(char*)match,Length(match),with.Data,with.Size);}
	String & Replace(int limit, char * match, int matchSize, char *with, int withSize);

	String & Replace(Reason::Language::Regex::Regex & regex, const Sequence & sequence) {return Replace(0,regex,sequence.Data,sequence.Size);}
	String & Replace(Reason::Language::Regex::Regex & regex, const char * data) {return Replace(0,regex,(char*)data,String::Length(data));}
	String & Replace(Reason::Language::Regex::Regex & regex, char * data, int size) {return Replace(0,regex,data,size);}

	String & Replace(int limit, Reason::Language::Regex::Regex & regex, const Sequence & sequence) {return Replace(regex,sequence.Data,sequence.Size);}
	String & Replace(int limit, Reason::Language::Regex::Regex & regex, const char * data) {return Replace(regex,(char*)data,String::Length(data));}
	String & Replace(int limit, Reason::Language::Regex::Regex & regex, char * data, int size);

	String & Translate(const Sequence & matches, const Sequence & replacements)	{return Translate(matches.Data,matches.Size,replacements.Data,replacements.Size);}
	String & Translate(const char * matches, const char *replacements) {return Translate((char*)matches,Length(matches),(char*)replacements,Length(replacements));}
	String & Translate(char * matches, int matchesSize, char * replacements, int replacementsSize);

	String & Trim(int amount);
	String & TrimLeft(int amount);
	String & TrimRight(int amount);
	String & Trim(const char * matches = " \t\n\r") {return Trim((char*)matches,Length(matches));}
	String & TrimLeft(const char * matches = " \t\n\r") {return TrimLeft((char*)matches,Length(matches));}
	String & TrimRight(const char * matches = " \t\n\r") {return TrimRight((char*)matches,Length(matches));}
	String & Trim(char * matches, int matchesSize);
	String & TrimLeft(char * matches, int matchesSize);
	String & TrimRight(char * matches, int matchesSize);

	String & Uppercase();
	String & Lowercase();
	String & Proppercase();

	String & Capitalize() {}

	using Sequence::Print;
	char * Print(); 

	bool IsAllocated() {return Allocated!=0;};

	virtual bool IsSubstring() {return Allocated==0;}

public:

	virtual float Increment() {return 0.25;}
	virtual int Remaining();

	virtual String & Allocate(int amount);

	virtual String & Reserve(int amount);

	virtual String & Left(int amount);
	virtual String & Right(int amount);

	virtual String & Resize(int amount);

	virtual String & Terminate();
	virtual String & Clear();

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Superstring : public String
{
public:

	Superstring(const Superstring & left, const Superstring & right)
	{
		Acquire(left);
		Append(right);
	}

	Superstring(const Superstring & left, const Sequence & right)
	{
		Acquire(left);
		Append(right);
	}

	Superstring(const Superstring & left, const char * right)
	{
		Acquire(left);
		Append(right);
	}

	Superstring(const Sequence & left, const Superstring & right)
	{
		Acquire(right);
		Prepend(left);
	}

	Superstring(const Sequence & left, const Sequence & right)
	{
		Allocate(left.Size+right.Size);
		Append(left);
		Append(right);
	}

	Superstring(const Sequence & left, const char * right)
	{
		Allocate(left.Size+Length(right));
		Append(left);
		Append(right);
	}

	Superstring(const char * left, const Superstring & right)
	{
		Acquire(right);
		Prepend(left);
	}

	Superstring(const char * left, const Sequence & right)
	{
		Allocate(Length(left)+right.Size);
		Append(left);
		Append(right);
	}

	Superstring(const Sequence & sequence, int from, int to):
		String((char*)sequence.Data+(int)min(sequence.Size-1,from),min(sequence.Size,to-from))
	{
	}

	Superstring(const Sequence & sequence, int from):
		String((char*)sequence.Data+(int)min(sequence.Size-1,from),sequence.Size-min(sequence.Size,from))
	{
	}

	Superstring(const Superstring & superstring)
	{
		Acquire(superstring);
	}

	Superstring(const Sequence & sequence):String(sequence)
	{

	}

	Superstring(const String & string):
		String(string)
	{
	}

	Superstring(const char * data):
		String(data)
	{
	}

	Superstring(char *data, int size):
		String(data,size)
	{
	}

	Superstring()
	{
	}

	~Superstring()
	{

	}

public:

	static Superstring Partial(const String & string);
	static Superstring Partial(const Superstring & superstring);
	static Superstring Partial(const Sequence & sequence) {return Partial(sequence.Data,sequence.Size);}
	static Superstring Partial(const char * data) {return Partial((char*)data,Length(data));}
	static Superstring Partial(char * data, int size);

	static Superstring Literal(const String & string);
	static Superstring Literal(const Superstring & superstring);
	static Superstring Literal(const Sequence & sequence);
	static Superstring Literal(const char * data) {return Literal((char*)data,Length(data));}
	static Superstring Literal(char * data, int size);

	static Superstring Actual(const Superstring & superstring) {return superstring;}
	static Superstring Actual(const Sequence & sequence) {return Superstring(sequence);}
	static Superstring Actual(const char * data) {return Superstring(data);}
	static Superstring Actual(char * data, int size) {return Superstring(data,size);}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Superstring operator + (const Superstring & left, const Superstring & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Superstring & left, const String & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Superstring & left, const Sequence & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Superstring & left, const char * right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const char * left, const Superstring & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const char * left, const String & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const char * left, const Sequence & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const String & left, const Superstring & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const String & left, const String & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const String & left, const Sequence & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const String & left, const char * right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Sequence & left, const Superstring & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Sequence & left, const String & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Sequence & left, const Sequence & right)
{
	return Superstring(left,right);
};

inline Superstring operator + (const Sequence & left, const char * right)
{
	return Superstring(left,right);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Superstring operator * (const Sequence & sequence, int times)
{
	Superstring mul = sequence;
	return mul *= times;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Stackstring
{
public:

	union Storage
	{
		char Array[64];
		char * Pointer;

		Storage():Pointer(0) {}

		int Size()
		{

			return ((Stackstring *) ((char *)this-(char *)&((Stackstring*)0)->Data))->Size;
		}

		operator char * () 
		{
			return Size() > 64 ? Pointer:Array;
		}

		char & operator[] (int index) 
		{
			return Size() > 64 ? Pointer[index]:Array[index];
		}

	} Data;

	int Size;

	Stackstring() {}
	Stackstring(const String & string) {Construct(string.Data,string.Size);}
	Stackstring(const Sequence &sequence) {Construct(sequence.Data,sequence.Size);}
	Stackstring(const char * data) {Construct((char*)data,String::Length(data));}
	Stackstring(char * data, int size) {Construct(data,size);}
	~Stackstring() {Destroy();}

	void Construct(const String & string)		{Construct(string.Data,string.Size);}
	void Construct(const Sequence &sequence)	{Construct(sequence.Data,sequence.Size);}
	void Construct(const char * data)			{Construct((char*)data,String::Length(data));}
	void Construct(char * data, int size)
	{
		Destroy();

		if (size > (64-1))
		{
			Data.Pointer = new char[size+1];
			memcpy(Data.Pointer,data,size);
			Data.Pointer[size] = 0;
		}
		else
		{
			memcpy(Data.Array,data,size);
			Data.Array[size] = 0;
		}

		Size = size;
	}

	Stackstring & operator = (const String & string) {Construct(string.Data,string.Size);return *this;}
	Stackstring & operator = (const Sequence & sequence) {Construct(sequence.Data,sequence.Size);return *this;}
	Stackstring & operator = (const char * data) {Construct((char*)data,String::Length(data));return *this;}

	void Destroy()
	{
		if (Data.Pointer && Size > 64)
			delete [] Data.Pointer;

		Release();
	}

	void Release()
	{
		memset(Data.Array,0,64);
		Data.Pointer=0;
		Size=0;
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

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

