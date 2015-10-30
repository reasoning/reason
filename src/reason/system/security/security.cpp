
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


#include "reason/reason.h"
#include "reason/system/security/security.h"
#include "reason/system/encoding/encoding.h"
#include "reason/system/time.h"

using namespace Reason::System::Security;
using namespace Reason::System::Encoding;
using namespace Reason::System;

#ifdef REASON_USING_OPENSSL
#include <openssl/md4.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char Random::Bit()
{

	unsigned long long n = Timer::Nanoseconds();

	char bit = (unsigned char) (1 &((n>>9)^(n>>8)^(n>>7)^(n>>6)^(n>>5)^(n>>4)^(n>>3)^(n>>2)^(n>>1)^n));

	return bit;
}

void Random::Delay(int & shift)
{
	if (shift <= 0)
	{
		DelayThread();
		if (shift < 5)
			shift = 0;
	}
	else
	{
		DelayProcess();
		if (shift > 5)
			shift = 0;
	}
}

void Random::DelayProcess()
{

	DelayThread();
}

void Random::DelayThread()
{
	struct Wait
	{
		static void Run(void*)
		{

		}
	};

	Callback<void,void*> callback = Wait::Run;
	Fibre fibre(callback);
	fibre.Context = (void*)0x12345698;
	fibre.Start();
	fibre.Join();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Uuid::UuidGenerator Uuid::Generator;

Uuid::UuidGenerator::UuidGenerator()
{

}

Uuid::Uuid(char bytes[16])
{
	Construct(&Octets,bytes);
}

Uuid::Uuid(char * data, int size)
{
	Construct(&Octets,data,size);
}

Uuid::Uuid(const Reason::System::String & string)
{
	Construct(&Octets,string.Data,string.Size);
}

Uuid::Uuid(const char * data)
{
	Construct(&Octets,(char*)data,String::Length(data));
}

Uuid::Uuid(UuidOctets * octets):Octets(*octets)
{

}

Uuid Uuid::Bytes(char bytes[16])
{
	UuidOctets octets;
	Construct(&octets,bytes);
	return Uuid(&octets);
}

Uuid Uuid::String(char * data, int size)
{
	UuidOctets octets;
	Construct(&octets,data,size);
	return Uuid(&octets);	
}

Uuid Uuid::Uuid1()
{
	Generator.Sequence = (short) Random::Integer();

	Epoch epoch(Calendar(15,10,1582),0,10000000);
	long long timestamp = Epoch(Time()).Seconds(epoch) * (Time::NanosecondsPerSecond/100);

	if (timestamp&0xE000000000000000)
		timestamp >>= 4;

	if (Generator.Timestamp > timestamp)
		++Generator.Sequence;

	Generator.Timestamp = timestamp;

	if (!Generator.Node)
	{

		char bytes[6];
		Random::Bytes(bytes,6);

		bytes[0] |= 0x01;	
		bytes[0] |= 0x02;	

		memcpy(&Generator.Node,bytes,6);

	}

	Uuid uuid;
	Construct(&uuid.Octets,Generator.Timestamp,Generator.Sequence,Generator.Node,Uuid::VERSION_1,Uuid::VARIANT_NORMAL);
	return uuid;

}

Uuid Uuid::Uuid2()
{

	char bytes[18];
	Random::Bytes(bytes,18);

	UuidState state;
	memcpy(&state,bytes,18);

	Uuid uuid;
	Construct(&uuid.Octets,state.Timestamp,state.Sequence,state.Node,Uuid::VERISON_2,Uuid::VARIANT_NORMAL);
	return uuid;
}

Uuid Uuid::Uuid3()
{

	Uuid uuid;
	return uuid;
}

void Uuid::Construct(UuidOctets * octets, char * data, int size)
{
	if (!octets)
		return;

	const int delim[] = {1,1,1,0,1,0};
	const int bytes[] = {8,4,4,2,2,12};
	int index = 0;
	int count = 0;
	int byte = 0;

	StringParser parser(data,size);
	while (!parser.Eof() && index < 6)
	{
		count = bytes[index];

		parser.Mark();
		while (count > 0 && !parser.Eof())
		{
			if (!parser.IsHex())
				break;

			parser.Next();
			--count;
		}
		parser.Trap();

		if (count > 0)
			break;

		Reason::System::String hex;
		Hex::Decode(parser.Token.Data,parser.Token.Size,hex);
		memcpy(octets->Bytes+byte,hex.Data,hex.Size);
		byte+=hex.Size;

		if (delim[index] != 0 && !parser.Skip('-'))
			break;

		count = 0;
		++index;
	}

	if (count == 0 && index == 6)
	{

	}
	else
	{
		memset(octets->Bytes,0,16);

	}

}

void Uuid::Construct(UuidOctets * octets, char bytes[16])
{
	if (!octets)
		return;

	memcpy(octets->Bytes,bytes,16);
}

void Uuid::Construct(UuidOctets * octets, unsigned long long timestamp, unsigned short sequence, unsigned long long node, unsigned char version, unsigned char variant)
{
	if (!octets)
		return;

	timestamp = timestamp&0x0FFFFFFFFFFFFFFF;	
	sequence = sequence&0x3FFF;					
	node = node&0x0000FFFFFFFFFFFF;				

	octets->Fields.TimestampLow = timestamp&0xFFFFFFFF;

	octets->Fields.TimestampMid = (timestamp>>32)&0xFFFF;

	octets->Fields.TimestampHighAndVersion = timestamp>>48;
	octets->Fields.TimestampHighAndVersion |= version<<12; 

	octets->Fields.SequenceHighAndReserved = sequence>>8;
	octets->Fields.SequenceHighAndReserved |= variant<<5;

	octets->Fields.SequenceLow = sequence&0x00FF;	

	octets->Fields.Node = node;

}

void Uuid::Print(Reason::System::String & string)
{

	Reason::System::String ascii;
	Hex::Encode(Octets.Bytes,16,ascii);
	string.Format("%.8s-%.4s-%.4s-%.2s%.2s-%.12s",ascii.Data,ascii.Data+8,ascii.Data+12,ascii.Data+16,ascii.Data+18,ascii.Data+20);	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef REASON_USING_OPENSSL

Md4::Md4():
	Context(0)
{
	Open();
}

Md4::Md4(const char * data):
	Context(0)
{
	String::Allocate(MD4_DIGEST_LENGTH);
	MD4((const unsigned char *)data,String::Length(data),(unsigned char *)Data);
	Size = MD4_DIGEST_LENGTH;
	Hash::Digest();
}

Md4::Md4(const Sequence & sequence):
	Context(0)
{
	String::Allocate(MD4_DIGEST_LENGTH);
	MD4((const unsigned char *)sequence.Data,sequence.Size,(unsigned char *)Data);
	Size = MD4_DIGEST_LENGTH;
	Hash::Digest();
}

Md4::Md4(char * data, int size):
	Context(0)
{
	String::Allocate(MD4_DIGEST_LENGTH);
	MD4((const unsigned char *)data,size,(unsigned char *)Data);
	Size = MD4_DIGEST_LENGTH;
	Hash::Digest();
}

Md4::~Md4()
{
	if (Context)
		delete Context;
}

void Md4::Open()
{
	if (Context)
		delete Context;

	Context = new MD4_CTX();
	MD4_Init(Context);
}

void Md4::Update(char * data, int size)
{
	MD4_Update(Context,data,size);
}

void Md4::Close()
{
	String::Allocate(MD4_DIGEST_LENGTH);
	MD4_Final((unsigned char *)Data,Context);
	Size = MD4_DIGEST_LENGTH;

	Hash::Digest();
}

void Md4::Digest()
{
	MD4_CTX context;
	memcpy(&context,Context,sizeof(MD4_CTX));

	if (Allocated < MD4_DIGEST_LENGTH)
		String::Allocate(MD4_DIGEST_LENGTH);

	MD4_Final((unsigned char *)Data,&context);
	Size = MD4_DIGEST_LENGTH;

	Hash::Digest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Md5::Md5():
	Context(0)
{
	Open();
}

Md5::Md5(const char * data):
	Context(0)
{
	String::Allocate(MD5_DIGEST_LENGTH);
	MD5((const unsigned char *)data,String::Length(data),(unsigned char *)Data);
	Size = MD5_DIGEST_LENGTH;
	Hash::Digest();
}

Md5::Md5(const Sequence & sequence):
	Context(0)
{
	String::Allocate(MD5_DIGEST_LENGTH);
	MD5((const unsigned char *)sequence.Data,sequence.Size,(unsigned char *)Data);
	Size = MD5_DIGEST_LENGTH;
	Hash::Digest();
}

Md5::Md5(char * data, int size):
	Context(0)
{
	String::Allocate(MD5_DIGEST_LENGTH);
	MD5((const unsigned char *)data,size,(unsigned char *)Data);
	Size = MD5_DIGEST_LENGTH;
	Hash::Digest();
}

Md5::~Md5()
{
	if (Context)
		delete Context;
}

void Md5::Open()
{
	if (Context)
		delete Context;

	Context = new MD5_CTX();
	MD5_Init(Context);
}

void Md5::Update(char * data, int size)
{
	MD5_Update(Context,data,size);
}

void Md5::Close()
{
	String::Allocate(MD5_DIGEST_LENGTH);
	MD5_Final((unsigned char *)Data,Context);
	Size = MD5_DIGEST_LENGTH;

	Hash::Digest();
}

void Md5::Digest()
{
	MD5_CTX context;
	memcpy(&context,Context,sizeof(MD5_CTX));

	if (Allocated < MD5_DIGEST_LENGTH)
		String::Allocate(MD5_DIGEST_LENGTH);

	MD5_Final((unsigned char *)Data,&context);
	Size = MD5_DIGEST_LENGTH;

	Hash::Digest();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sha1::Sha1()
{
	Context = new SHA_CTX();
	Open();
}

Sha1::Sha1(const char * data)
{
	String::Allocate(SHA_DIGEST_LENGTH);
	SHA1((const unsigned char *)data,String::Length(data),(unsigned char *)Data);
	Size = SHA_DIGEST_LENGTH;
	Hash::Digest();
}

Sha1::Sha1(const Sequence & sequence)
{
	String::Allocate(SHA_DIGEST_LENGTH);
	SHA1((const unsigned char *)sequence.Data,sequence.Size,(unsigned char *)Data);
	Size = SHA_DIGEST_LENGTH;
	Hash::Digest();
}

Sha1::Sha1(char * data, int size)
{
	String::Allocate(SHA_DIGEST_LENGTH);
	SHA1((const unsigned char *)data,size,(unsigned char *)Data);
	Size = SHA_DIGEST_LENGTH;
	Hash::Digest();
}

Sha1::~Sha1()
{
	delete Context;
}

void Sha1::Open()
{
	SHA1_Init(Context);
}

void Sha1::Update(char * data, int size)
{
	SHA1_Update(Context,data,size);
}

void Sha1::Close()
{
	String::Allocate(SHA_DIGEST_LENGTH);
	SHA1_Final((unsigned char *)Data,Context);
	Size = SHA_DIGEST_LENGTH;

	Hash::Digest();
}

void Sha1::Digest()
{
	SHA_CTX context;
	memcpy(&context,Context,sizeof(SHA_CTX));

	if (Allocated < SHA_DIGEST_LENGTH)
		String::Allocate(SHA_DIGEST_LENGTH);

	SHA1_Final((unsigned char *)Data,&context);
	Size = SHA_DIGEST_LENGTH;

	Hash::Digest();
}

#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

