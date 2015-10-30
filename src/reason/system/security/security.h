
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
#ifndef SYSTEM_SECURITY_H
#define SYSTEM_SECURITY_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/reason.h"
#include "reason/system/sequence.h"
#include "reason/system/string.h"
#include "reason/system/stream.h"

using namespace Reason::System;

#ifdef REASON_USING_OPENSSL
struct MD4state_st;
struct MD5state_st;
struct SHAstate_st;
#endif

namespace Reason { namespace System { namespace Security {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Random
{
public:

	enum RandomOptions
	{
		RANDOM_PROCESS,		
		RANDOM_THREAD,
		RANDOM_SHA1,
		RANDOM_MD5,
	};

	static int Integer(int options=0)
	{
		char bytes[4];
		Bytes(bytes,4,options);
		return *((int*)bytes);
	}

	static void Integers(int * data, int size, int options=0)
	{
		while(size > 0)
		{
			*data++ = Integer(options);
			--size;
		}	
	}

	static char Byte(int options=0)
	{
		int bit=0;
		char byte=0;

		int shift=0;

		while(bit < 8)
		{

			switch (options)
			{
				case RANDOM_PROCESS: DelayProcess(); break;
				case RANDOM_THREAD: DelayThread(); break;
				default: Delay(shift);					
			}

			if (options&RANDOM_SHA1)
			{

			}

			byte |= Bit() << bit;

			++bit;
		}

		return byte;
	}	

	static void Bytes(char * data, int size, int options=0)
	{
		while(size > 0)
		{
			*data++ = Byte(options);
			--size;
		}
	}	

	static void Bytes(String & string, int length, int options=0)
	{
		string.Release();
		while(length > 0)
		{
			string.Append(Byte(options));
			--length;
		}
	}

protected:

	static char Bit();

	static void Delay(int & shift);
	static void DelayProcess();
	static void DelayThread();
};      

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Guid
{
public:

	Guid() {}
	Guid(const Sequence & sequence) {Construct(sequence);}

	void Construct(const char * data) {Construct((char*)data,String::Length(data));}
	void Construct(const Sequence & sequence) {Construct(sequence.Data,sequence.Size);}
	void Construct(char * data, int size);

	static String Generate();

};

class Uuid : public Printable, public Comparable
{
public:

	enum UuidVariant
	{
		VARIANT_NCS_COMPAT			=(0),	
		VARIANT_NORMAL				=(1),	
		VARIANT_MICROSOFT_COMPAT	=(3),	
		VARIANT_FUTURE				=(7),	
	};

	enum UuidVersion
	{
		VERSION_1	=(1),		
		VERISON_2	=(2),		
		VERISON_3	=(3),		
		VERSION_4	=(4),		
	};

	#pragma pack(push,1)

	union UuidOctets
	{
		struct UuidFields
		{
		unsigned int TimestampLow:32;
		unsigned short TimestampMid:16;
		unsigned short TimestampHighAndVersion:16;
		unsigned char SequenceHighAndReserved:8;
		unsigned char SequenceLow:8;
		unsigned long long Node:48;
		} Fields;

		char Bytes[16];
	};

	#pragma pack(pop)

	struct UuidState
	{
		unsigned long long Timestamp;	
		unsigned long long Node;		
		unsigned short Sequence;		
	};

	struct UuidGenerator : public UuidState
	{
	public:

		UuidState State;

		UuidGenerator();

		void Load(Reader & reader);
		void Save(Writer & writer);

		void Generate(UuidOctets * octets);

	};

	static UuidGenerator Generator;

	UuidOctets Octets;

	void ClockSequence(int sequence=0);	

	static void Construct(UuidOctets * octets, unsigned long long timestamp, unsigned short sequence, unsigned long long node, unsigned char version, unsigned char variant);
	static void Construct(UuidOctets * octets, char bytes[16]);
	static void Construct(UuidOctets * octets, char * data, int size);

	Uuid() {}
	Uuid(char bytes[16]);
	Uuid(char * data, int size);
	Uuid(const Reason::System::String & string);
	Uuid(const char * data);

	Uuid(UuidOctets * octets);

	static Uuid Uuid1();	
	static Uuid Uuid2();	
	static Uuid Uuid3();	
	static Uuid Uuid4();

	void Bytes(char *data, int size);

	static Uuid Bytes(char bytes[16]);
	static Uuid String(char * data, int size);
	static Uuid String(const Reason::System::String & string) {return String(string.Data,string.Size);}
	static Uuid String(const char * data) {return String((char*)data,String::Length(data));}

	void Print(Reason::System::String & string);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Cipher
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PublicKey
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Certificate
{
public:

};

class X509 : public Certificate
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Hash : public String
{
public:

	Hash()
	{	

	}

	~Hash()
	{
	}

	virtual void Open()=0;
	void Update(const char * data) {Update((char*)data, String::Length(data));}
	void Update(const Sequence & sequence) {Update(sequence.Data,sequence.Size);}
	virtual void Update(char * data, int size)=0;
	virtual void Close()=0;

	virtual void Digest() 
	{

		String code;
		code.Allocate(Size*2);

		const char hex[] = "0123456789abcdef";
		for (int i=0;i<Size;++i)
		{
			code.Append(hex[(unsigned char)Data[i]>>4]);
			code.Append(hex[(unsigned char)Data[i]&0xF]);
		}

		Acquire(code);
	}

};

class HashStream : public StreamFilter
{
public:

	Reason::System::Security::Hash & Hash;

	HashStream(Reason::System::Stream & stream, Reason::System::Security::Hash & hash):
		StreamFilter(stream),Hash(hash)
	{

	}

	String & Digest() {return Hash.Digest(),Hash;}

	int Seek(int position, int origin = -1)	
	{
		Hash.Open();
		return StreamFilter::Seek(position,origin);
	}

	using Stream::Read;
	int Read(char * data, int size)		
	{
		if (!Position) Hash.Open();

		int result = StreamFilter::Read(data,size);
		if (result)	Hash.Update(data,size);
		return result;
	}

	using Stream::Write;
	int Write(char * data , int size)	
	{
		if (!Position) Hash.Open();

		int result = StreamFilter::Write(data,size);
		if (result)	Hash.Update(data,size);
		return result;
	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef REASON_USING_OPENSSL

class Md4 : public Hash
{
public:

	MD4state_st * Context;

	Md4();
	Md4(const char * data);
	Md4(const Sequence & sequence);
	Md4(char * data, int size);
	~Md4();

	void Open();
	void Update(char * data, int size);
	void Close();

	void Digest();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Md5 : public Hash
{
public:

	MD5state_st * Context;

	Md5();
	Md5(const char * data);
	Md5(const Sequence & sequence);
	Md5(char * data, int size);
	~Md5();

	void Open();
	void Update(char * data, int size);
	void Close();

	void Digest();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Sha1 : public Hash
{
public:

	SHAstate_st * Context;

	Sha1();
	Sha1(const char * data);
	Sha1(const Sequence & sequence);
	Sha1(char * data, int size);
	~Sha1();

	void Open();
	void Update(char * data, int size);
	void Close();

	void Digest();
};

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

