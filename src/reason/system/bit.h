
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
#ifndef SYSTEM_BIT_H
#define SYSTEM_BIT_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>

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

#define BitmaskGenerator(Type) \
Type Bits; \
inline bool Is(Type bits)				{ return(Bits & bits)!=0||Bits==bits; };\
inline bool Equals(Type bits)			{ return(Bits == bits); };\
inline void Xor(Type bits)				{ Bits ^= bits; };\
inline void On(Type bits)				{ Bits |= bits; };\
inline void On()						{ Bits |= ~0; };\
inline void Off(Type bits)				{ Bits &= ~bits; };\
inline void Off()						{ Bits &= 0; };\
inline void Flip()						{ Bits = ~Bits; };\
inline void Assign(Type bits)			{ Bits = bits; };\
inline Type Union(Type bits)			{ return Bits | bits; };\
inline Type Difference(Type bits)		{ return Bits & ~bits; };\
inline Type Intersection(Type bits)		{ return Bits & bits; };\
inline operator Type ()					{ return Bits;};\
inline Type & operator = (Type bits)	{ Assign(bits);	return Bits; }\
inline void Clear()						{ Bits = 0; };\
inline Type operator [](int i)			{ return (Bits & (Type(1)<<i)); }

#define BitmaskGeneratorStatic(Type) \
static bool Is(Type bitmask, Type bits)				{ return(bitmask & bits)!=0||bitmask==bits; };\
static bool Equals(Type bitmask, Type bits)			{ return(bitmask == bits); };\
static void Xor(Type bitmask, Type bits)			{ bitmask ^= bits; };\
static void On(Type bitmask, Type bits)				{ bitmask |= bits; };\
static void On(Type bitmask)						{ bitmask |= ~0; };\
static void Off(Type bitmask, Type bits)				{ bitmask &= ~bits; };\
static void Off(Type bitmask)						{ bitmask &= 0; };\
static void Flip(Type bitmask)						{ bitmask = ~bitmask; };\
static void Assign(Type bitmask, Type bits)			{ bitmask = bits; };\
static Type Union(Type bitmask, Type bits)			{ return bitmask | bits; };\
static Type Difference(Type bitmask, Type bits)		{ return bitmask & ~bits; };\
static Type Intersection(Type bitmask, Type bits)	{ return bitmask & bits; };\
static void Clear(Type bitmask, Type bits)			{ bitmask = 0; };\
static Type Bit(Type bitmask, int i)				{ return (bitmask & (Type(1)<<i)); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Bitmask
{
public:

	BitmaskGeneratorStatic(unsigned char)
	BitmaskGeneratorStatic(unsigned short)
	BitmaskGeneratorStatic(unsigned int)
	BitmaskGeneratorStatic(unsigned long long)

	static int Count(unsigned char bits)
	{
		bits -= (bits & 0xaa) >> 1;
		bits =  (bits & 0x33) + ((bits >> 2) & 0x33);
		bits =  (bits + (bits >> 4)) & 0x0f;
		return (int)(bits) & 0xff;
	}

	static int Count(unsigned short bits)
	{
		bits -= (bits & 0xaaaa) >> 1;
		bits =  (bits & 0x3333) + ((bits >> 2) & 0x3333);
		bits =  (bits + (bits >> 4)) & 0x0f0f;
		bits += bits >> 8;     
		return (int)(bits) & 0xff;
	}

	static int Count(unsigned int bits)
	{
		bits -= (bits & 0xaaaaaaaa) >> 1;
		bits =  (bits & 0x33333333) + ((bits >> 2) & 0x33333333);
		bits =  (bits + (bits >> 4)) & 0x0f0f0f0f;
		bits += bits >> 8;     
		bits += bits >> 16;    
		return (int)(bits) & 0xff;
	}

	static int Count(unsigned long long bits)
	{
		bits -= (bits & 0xaaaaaaaaaaaaaaaaLL) >> 1;
		bits =  (bits & 0x3333333333333333LL) + ((bits >> 2) & 0x3333333333333333LL);
		bits =  (bits + (bits >> 4)) & 0x0f0f0f0f0f0f0f0fLL;
		bits += bits >> 8;     
		bits += bits >> 16;    
		return ((int)(bits) + (int)(bits >> 32)) & 0xff;
	}
};

class Bitmask8 : public Bitmask
{
public:
	Bitmask8():Bits(0){};
	Bitmask8(unsigned char bits):Bits(bits){};
	BitmaskGenerator(unsigned char);

	int Count(unsigned char bits=0) { return Bitmask::Count((bits==0)?Bits:bits);}

};	

class Bitmask16 : public Bitmask
{
public:
	Bitmask16():Bits(0){};
	Bitmask16(unsigned short bits):Bits(bits){};
	BitmaskGenerator(unsigned short);

	int Count(unsigned short bits=0) { return Bitmask::Count((bits==0)?Bits:bits);}
};

class Bitmask32 : public Bitmask
{
public:
	Bitmask32():Bits(0){};
	Bitmask32(unsigned int bits):Bits(bits){};
	BitmaskGenerator(unsigned int);

	int Count(unsigned int bits=0) { return Bitmask::Count((bits==0)?Bits:bits);}
};

class Bitmask64 : public Bitmask
{
public:
	Bitmask64():Bits(0){};
	Bitmask64(unsigned long long bits):Bits(bits){};
	BitmaskGenerator(unsigned long long);

	int Count(unsigned long long bits=0) { return Bitmask::Count((bits==0)?Bits:bits);}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Bit
{
public:
	unsigned char * Byte;	
	short Index;			
	short Mask;				

	Bit(unsigned char *byte, int index)
	{
		Assign(byte,index);
	}

	void Assign(unsigned char *byte, int index)
	{
		Byte = byte;
		Index = index;
		Mask = 1 << Index;
	}

	void On()
	{
		*Byte |= Mask;
	}

	void Off()
	{
		*Byte &= ~Mask;
	}

	operator unsigned char () 
	{
		return *Byte & Mask;
	}

	Bit & operator = (Bit bit)
	{
		(bit==0)?Off():On();
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

