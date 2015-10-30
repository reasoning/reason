
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
#ifndef SYSTEM_BINARY_H
#define SYSTEM_BINARY_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/stream.h"
#include "reason/system/bit.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Bits
{
public:

	static inline int Count(int bits, int size=sizeof(int))
	{
		size = size<<3;

		return (bits + size-1)/size;
	}

	static inline int Index(int bit, int size=sizeof(int))
	{
		size = size<<3;

		return bit/size;
	}

	static inline int Mask(int bits, int index=0, int size=sizeof(int))
	{
		size = size<<3;

		int mask = (int)((1LL<<bits)-1)<<((size-bits)-index);

		return mask;
	}

	static int Append(int & to, int from, int bits, int toIndex=0, int fromIndex=0, int fromDir=1, int size=sizeof(int));
	static int Prepend(int & to, int from, int bits, int toIndex=-1, int fromIndex=0, int fromDir=1, int size=sizeof(int));

	static int Insert(int & to, int from, int bits, int toIndex=0, int fromIndex=0, int toDir=1, int fromDir=1, int size=sizeof(int));

	static inline void Move(void * to, void * from, int bits)
	{

	}

	static inline void Copy(void * to, void * from, int bits)
	{
		int bytes = bits+7/8;

        OutputAssert(&from != &to);
		OutputAssert((&from+bytes < &to) || (&to+bytes < &from));

		const int size = sizeof(int);

		for (;bytes>size;bytes-=size,from=(int*)from+1,to=(int*)to+1)
		{
			*(int*)to = *(int*)from;
		}

		for ( ; bytes%size != 0; --bytes,from=(char*)from+1,to=(char*)to+1 )
		{
			*(char*)to = *(char*)from;
		}

		char mask = (char) Mask(bits%8,0,sizeof(char));
		*(char*)to = *(char*)from & mask;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Bytes
{
public:

	static inline void Copy(char * to, char * from, int size)
	{

		if (to+size >= from && to <= from+size) return;

		{
			memcpy(to,from,size);
		}

	}

	static inline void Move(char * to, char * from, int size)
	{

		{
			memmove(to,from,size);
		}

	}

	static inline int Compare(void * left, void * right, int bytes)
	{
	}

	static inline bool Equals(void * left, void * right, int bytes)
	{
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Hashes
{
public:

	class OneAtATime
	{
	public:

		static unsigned int Hash(unsigned char *key, int length)
		{
			unsigned int hash = 0;
			int i;

			for (i = 0; i < length; i++) {
				hash += key[i];
				hash += (hash << 10);
				hash ^= (hash >> 6);
			}
			hash += (hash << 3);
			hash ^= (hash >> 11);
			hash += (hash << 15);
			return hash;
		}
	};

	class Lookup3Hash
	{
	public:

		#define Hashsize(n) ((unsigned int)1<<(n))
		#define Hashmask(n) (Hashsize(n)-1)
		#define Rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

		#define Mix(a,b,c) \
		{ \
			a -= c;  a ^= Rot(c, 4);  c += b; \
			b -= a;  b ^= Rot(a, 6);  a += c; \
			c -= b;  c ^= Rot(b, 8);  b += a; \
			a -= c;  a ^= Rot(c,16);  c += b; \
			b -= a;  b ^= Rot(a,19);  a += c; \
			c -= b;  c ^= Rot(b, 4);  b += a; \
		}

		#define Final(a,b,c) \
		{ \
			c ^= b; c -= Rot(b,14); \
			a ^= c; a -= Rot(c,11); \
			b ^= a; b -= Rot(a,25); \
			c ^= b; c -= Rot(b,16); \
			a ^= c; a -= Rot(c,4);  \
			b ^= a; b -= Rot(a,14); \
			c ^= b; c -= Rot(b,24); \
		}

		static unsigned int Hashword(const unsigned int * key, int length, unsigned int primary)
		{
			unsigned int a,b,c;

			a = b = c = 0xdeadbeef + (((unsigned int)length)<<2) + primary;

			while (length > 3)
			{
				a += key[0];
				b += key[1];
				c += key[2];
				Mix(a,b,c);
				length -= 3;
				key += 3;
			}

			switch(length)
			{ 
			case 3 : c+=key[2];
			case 2 : b+=key[1];
			case 1 : a+=key[0];
				Final(a,b,c);
			case 0:     
				break;
			}

			return c;
		}

		static void Hashword (const unsigned int * key, int length, unsigned int * primary, unsigned int * secondary)
		{
			unsigned int a,b,c;

			a = b = c = 0xdeadbeef + ((unsigned int)(length<<2)) + *primary;
			c += *secondary;

			while (length > 3)
			{
				a += key[0];
				b += key[1];
				c += key[2];
				Mix(a,b,c);
				length -= 3;
				key += 3;
			}

			switch(length)
			{ 
			case 3 : c+=key[2];
			case 2 : b+=key[1];
			case 1 : a+=key[0];
				Final(a,b,c);
			case 0:     
				break;
			}

			*primary=c; *secondary=b;
		}
	};

	class SuperFastHash
	{
	public:

		#define Get16Bits(d) (*((const unsigned short *) (d)))

		static unsigned int Hash (const char * data, int length) 
		{
			unsigned int hash = length, tmp;
			int rem;

			if (length <= 0 || data == NULL) return 0;

			rem = length & 3;
			length >>= 2;

			for (;length > 0; length--) {
				hash  += Get16Bits (data);
				tmp    = (Get16Bits (data+2) << 11) ^ hash;
				hash   = (hash << 16) ^ tmp;
				data  += 2*sizeof (unsigned short);
				hash  += hash >> 11;
			}

			switch (rem) {
				case 3: hash += Get16Bits (data);
						hash ^= hash << 16;
						hash ^= data[sizeof (unsigned short)] << 18;
						hash += hash >> 11;
						break;
				case 2: hash += Get16Bits (data);
						hash ^= hash << 11;
						hash += hash >> 17;
						break;
				case 1: hash += *data;
						hash ^= hash << 10;
						hash += hash >> 1;
			}

			hash ^= hash << 3;
			hash += hash >> 5;
			hash ^= hash << 4;
			hash += hash >> 17;
			hash ^= hash << 25;
			hash += hash >> 6;

			return hash;
		}

	};

	class FnvHash
	{
	public:

		static unsigned int Hash(unsigned char * key, unsigned int length)
		{

			static unsigned int prime = 16777619;
			static unsigned int offset = 2166136261U;

			unsigned int hash = offset;
			for (unsigned int i=0; i < length; ++i)
			{
				hash ^= key[i];
				hash *= prime;
			}

			return hash;
		}
	};

	class BernsteinHash
	{
	public:

		static unsigned int Hash(unsigned char *key, unsigned int length, unsigned int level)
		{
			unsigned int hash = level;
			for (unsigned int i=0; i < length; ++i) 

				hash = ((hash << 5) + hash) + key[i]; 
			return hash;
		}

	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Binary
{
public:

	static inline int Power(unsigned int key)
	{

		int pow = (key>>16==0?
						(key>>8==0?
							(key>>4==0?
								(key>>2==0?(key>>1==0?0:1):(key>>3==0?2:3)):
								(key>>6==0?(key>>5==0?4:5):(key>>7==0?6:7))
							):
							(key>>12==0?
								(key>>10==0?(key>>9==0?8:9):(key>>11==0?10:11)):
								(key>>14==0?(key>>13==0?12:13):(key>>15==0?14:15))
							)
						):
						(key>>24==0?
							(key>>20==0?
								(key>>18==0?(key>>17==0?16:17):(key>>19==0?18:19)):
								(key>>22==0?(key>>21==0?20:21):(key>>23==0?22:23))
							):
							(key>>28==0?
								(key>>26==0?(key>>25==0?24:25):(key>>27==0?26:27)):
								(key>>30==0?(key>>29==0?28:29):(key>>31==0?30:31)))
							)
					);
		return pow;
	}

public:

	unsigned char * Data;		
	int Size;

	int Count;					
	int Index;					

	enum OrderType
	{
		ORDER_ASCENDING,
		ORDER_DESCENDING,
	};

	Bitmask8 Order;

	void Endian()
	{

		int thirtytwo = 0x01020304L;
		char eight;

		memcpy(&eight,&thirtytwo,1);

		switch (eight)
		{
		case 0x01: Order.Assign(ORDER_DESCENDING); break;	
		case 0x04: Order.Assign(ORDER_ASCENDING); break;	
		default: OutputFailure("Binary::Order - Cannot determine system byte order.\n");
		}

	}

	static bool IsLittleEndian()
	{
		int thirtytwo = 0x01020304L;
		char eight = *((char*)&thirtytwo);		
		return eight == 0x04;
	}

	static bool IsBigEndian()
	{
		int thirtytwo = 0x01020304L;
		char eight = *((char*)&thirtytwo);
		return eight == 0x01;
	}

	Binary()
	{
		Data=0;
		Size=0;
		Count=0;
		Index=0;
	}

	~Binary()
	{
		if (Data)
			delete [] Data;
	}

	int Remaining() 
	{
		return 8*Size-Count;
	}

	Binary & Allocate(int bits=0);

	Binary & Resize(int bits);
	Binary & Reserve(int bits);

	Binary & Left(int bits);
	Binary & Right(int bits);

	void Copy(Binary &bits)
	{
		if (Data == 0)
		{

			Allocate(bits.Count);
			Count = bits.Count;
		}
		else
		{
			Resize(bits.Count - Count);
		}

		memcpy(Data,bits.Data,Bytes());
	}

	void Append(Binary &bits)
	{
		if (bits.Data == 0) 
			return;

		OutputAssert(Data != bits.Data);

		int offset = Count;
		Resize(bits.Count);
		memcpy(Data+offset,bits.Data,bits.Bytes());	
	}

	void Prepend(Binary &bits)
	{		
		OutputAssert(bits.Data != Data);

		if (bits.Data == 0) 
			return;

		Resize(bits.Count);
		Left(bits.Count);

		memcpy(Data,bits.Data,Bytes());
	}

	int Bytes()			
	{
		return (Count+7)/8;

	}

	int Mask()
	{	
		return 2^Count-1;
	}

	bool Is(Binary &bits)
	{ 

		int mask = Mask() & bits.Mask();
		int data = *(int*)Data & mask;
		int comp = *(int*)bits.Data & mask;

		return (data&comp)!=0||data==comp;
	}

	bool Equals(Binary &bits)
	{

	}

	void Clear()
	{

		memset(Data,0,Bytes());
	}

	Bit operator[](int bit) 
	{

		return Bit(&Data[Bits::Index(bit)],bit & 7);
	}

	static Reason::System::Superstring Pack(const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		Reason::System::Superstring superstring;
		superstring.Allocate(String::Length(format)*1.25);	
		PackVa(superstring,format,va);
		va_end(va);
		return superstring;
	}

	static Reason::System::Superstring PackVa(const char * format, va_list va)
	{
		Reason::System::Superstring superstring;
		superstring.Allocate(String::Length(format)*1.25);	
		PackVa(superstring,format,va);
		return superstring;
	}

	static void Pack(String & string, const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		PackVa(string,format,va);
		va_end(va);
	}

	static void PackVa(String & string, const char * format, va_list va);

	static void Pack(String & string, const char * format, String & args);

	static void Unpack(Sequence & sequence, const char * format, ...)
	{
		va_list va;
		va_start(va, format);
		UnpackVa(sequence,format,va);
		va_end(va);
	}

	static void UnpackVa(Sequence & sequence, const char * format, va_list va);

	static void Unpack(Sequence & sequence, const char * format, String & args);

	void Terminate() 
	{

	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Packed
{

};

class Packing
{

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class BinaryStream : public StreamFilter
{
public:

	BinaryStream(Reason::System::Stream & stream):StreamFilter(&stream) {}
	BinaryStream(Reason::System::Stream * stream):StreamFilter(stream) {}
	BinaryStream() {}
	~BinaryStream() {}

	using StreamFilter::Write;
	int Write(const bool & data)				{return Write((char&)data);}
	int Write(const char & data)				{return Write((char *) &data,sizeof(char));}
	int Write(const unsigned char & data)		{return Write((char *) &data,sizeof(unsigned char));}
	int Write(const short & data)				{return Write((char *) &data,sizeof(short));}
	int Write(const unsigned short &data)		{return Write((char *) &data,sizeof(unsigned short));}
	int Write(const int & data)					{return Write((char *) &data,sizeof(int));}
	int Write(const unsigned int & data)		{return Write((char *) &data,sizeof(unsigned int));}
	int Write(const long & data)				{return Write((char *) &data,sizeof(long));}
	int Write(const unsigned long & data)		{return Write((char *) &data,sizeof(unsigned long));}
	int Write(const long long & data)			{return Write((char *) &data,sizeof(long long));}
	int Write(const unsigned long long & data)	{return Write((char *) &data,sizeof(unsigned long long));}
	int Write(const float & data)				{return Write((char *) &data,sizeof(float));}
	int Write(const double & data)				{return Write((char *) &data,sizeof(double));}
	int Write(const void * & data)				{return Write((char*) &data,sizeof(void*));}

	using StreamFilter::Read;
	int Read(bool & data)						{return Read((char&)data);}
	int Read(char & data)						{return Read((char *) &data,sizeof(char));}
	int Read(unsigned char & data)				{return Read((char *) &data,sizeof(unsigned char));}
	int Read(short & data)						{return Read((char *) &data,sizeof(short));}
	int Read(unsigned short &data)				{return Read((char *) &data,sizeof(unsigned short));}
	int Read(int & data)						{return Read((char *) &data,sizeof(int));}
	int Read(unsigned int & data)				{return Read((char *) &data,sizeof(unsigned int));}
	int Read(long & data)						{return Read((char *) &data,sizeof(long));}
	int Read(unsigned long & data)				{return Read((char *) &data,sizeof(unsigned long));}
	int Read(long long & data)					{return Read((char *) &data,sizeof(long long));}
	int Read(unsigned long long & data)			{return Read((char *) &data,sizeof(unsigned long long));}
	int Read(float & data)						{return Read((char *) &data,sizeof(float));}
	int Read(double & data)						{return Read((char *) &data,sizeof(double));}
	int Read(void * & data)						{return Read((char*) &data,sizeof(void*));}

	int Read8(unsigned char & bits);
	int Read16(unsigned short & bits);
	int Read32(unsigned int & bits);
	int Read64(unsigned long long & bits);

	unsigned char Read8() {unsigned char bits;Read8(bits);return bits;}
	unsigned short Read16() {unsigned short bits;Read16(bits);return bits;}
	unsigned int Read32() {unsigned int bits;Read32(bits);return bits;}
	unsigned long long Read64() {unsigned long long bits;Read64(bits);return bits;}

	int Write8(const unsigned char & bits);
	int Write16(const unsigned short & bits);
	int Write32(const unsigned int & bits);
	int Write64(const unsigned long long & bits);

	int ReadNull8(int amount)					{unsigned char data=0;int size=0;while(amount-- > 0) size += Read8(data);return size;}
	int ReadNull16(int amount)					{unsigned short data=0;int size=0;while(amount-- > 0) size += Read16(data);return size;}
	int ReadNull32(int amount)					{unsigned int data=0;int size=0;while(amount-- > 0) size += Read32(data);return size;}
	int ReadNull64(int amount)					{unsigned long long data=0;int size=0;while(amount-- > 0) size += Read64(data);return size;}

	int WriteNull8(int amount)					{unsigned char data=0;int size=0;while(amount-- > 0) size += Write8(data);return size;}
	int WriteNull16(int amount)					{unsigned short data=0;int size=0;while(amount-- > 0) size += Write16(data);return size;}
	int WriteNull32(int amount)					{unsigned int data=0;int size=0;while(amount-- > 0) size += Write32(data);return size;}
	int WriteNull64(int amount)					{unsigned long long data=0;int size=0;while(amount-- > 0) size += Write64(data);return size;}

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

