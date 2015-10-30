
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/indices.h"
#include "reason/system/binary.h"
#include "reason/system/string.h"
#include "reason/system/stringstream.h"
#include "reason/system/parser.h"
#include "reason/system/number.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Bits::Append(int & to, int from, int bits, int toIndex, int fromIndex, int fromDir, int size)
{
	return Insert(to,from,bits,toIndex,fromIndex,1,fromDir,size);
}

int Bits::Prepend(int & to, int from, int bits, int toIndex, int fromIndex, int fromDir, int size)
{
	return Insert(to,from,bits,toIndex-bits,fromIndex,1,fromDir,size);
}

int Bits::Insert(int & to, int from, int bits, int toIndex, int fromIndex, int toDir, int fromDir, int size)
{
	size = size<<3;

	if (!Indices::Normalise(toIndex,size) || !Indices::Normalise(fromIndex,size))
		return 0;

	if (toIndex < 0 || toIndex >= size || fromIndex < 0 || fromIndex >= size)
		return 0;

	if (toDir == fromDir)
	{

		int diff = fromIndex - toIndex;
		from = (diff>0)?from<<diff:from>>abs(diff);
		toIndex = (toDir>0)?toIndex:toIndex-size;

		int mask = Mask(bits,toIndex);
		from &= mask; 
		to &= ~mask;		
		to |= from;	
	}
	else
	{

		int fromMask = Mask(1,fromIndex,size);
		int toMask = Mask(1,toIndex,size);
		int bit = 1<<toIndex;

		while (bits)
		{
			to &= ~toMask;
			if (from&fromMask == 1)
				to |= fromMask;

			fromMask = (toDir>0)?fromMask<<1:fromMask>>1;
			toMask = (toDir>0)?toMask>>1:toMask<<1;

			--bits;		
		}

	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Binary & Binary::Allocate(int bits)
{
	if (bits == 0)
	{
		if (!Size && Count > 0)
			bits = Count;
		else
			bits = (Count>0)?Count+1:1;
	}
	else
	if (bits > 0)
	{
		if (bits < Count)
			return *this;
	}
	else
	if (bits < 0)
	{
		return *this;

	}

	int size = Bits::Count(bits,1);

	unsigned char * data = new unsigned char[size];

	if (Count > 0) 
		memcpy(data,Data,(Count+7)/8);

	if (Size)
		delete [] Data;

	Data = data;
	Size = size;

	return *this;
}

Binary & Binary::Resize(int bits)
{
	if (!Size)
	{
		Allocate(bits);
		return *this;

	}

	if (bits > 0)
	{
		if (Count == 0) 
		{
			Allocate(bits);
		}
		else
		if (Remaining()-bits < 0)
		{
			int size = Bits::Count(Count+bits);
			unsigned char * data = new unsigned char[size];
			memcpy(data,Data,(Count+7)/8);

			if (Size)
				delete [] Data;	

			Data = data;
			Size = size;	
		}

		Count += bits;
	}
	else
	if (bits < 0)
	{
		Count += bits;
	}

	return *this;
}

Binary & Binary::Reserve(int bits)
{
	if (bits > 0)
	{
		if (Count == 0) 
		{
			Allocate(bits);
		}
		else
		if (Remaining()-bits < 0)
		{
			int size = Bits::Count(Count+bits);
			unsigned char * data = new unsigned char[size];
			memcpy(data,Data,(Count+7)/8);

			if (Size)
				delete [] Data;	

			Data = data;
			Size = size;
		}
	}
	else
	if (bits < 0)
	{

		bits = Count;
		Allocate(bits);
	}

	return *this;
}

Binary & Binary::Left(int bits)
{
	OutputAssert((Size > 0 && Data != 0) || (Size == 0));

	if (bits >= 0)
	{
		Reserve(bits);

		if (bits%8 == 0)
		{

			memmove(Data+((bits+7)/8),Data,Size);

		}
		else
		{
			int bit = Size;

			int toByte = Bits::Index(bit+bits,1);
			int fromByte = Bits::Index(bit,1);

			int toIndex = (bit+bits)%8;		
			int toCount = toIndex;			

			int fromIndex = bit%8;
			int fromCount = fromIndex;

			while (bit > 0)
			{
				int to = Bits::Index(toByte,1);
				int from = Bits::Index(fromByte,1);

				bits = (toCount<fromCount)?toCount:fromCount; 

				Bits::Insert(to,from,bits,toIndex-bits,fromIndex-bit,1,1,sizeof(char));

				fromIndex -= bits;
				if (fromIndex < 0)
					fromByte--;

				Data[toByte] = to;

				bit -= 8;
			}
		}
	}
	else
	if (bits < 0)
	{
		memmove(Data,Data-bits,Size+bits);
	}

	Size += bits; 
	Terminate();
	return *this;
}

Binary & Binary::Right(int bits)
{
	OutputAssert((Size > 0 && Data != 0) || (Size == 0));

	if (bits >= 0)
	{
		Reserve(bits);
	}
	else
	{

	}

	Size += bits;
	Terminate();
	return *this;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Binary::PackVa(String & packed, const char * format, va_list va)
{

	String pack;

	StringStream::StreamAdapter packedAdapter(packed);
	BinaryStream packedStream(packedAdapter);

	StringParser parser(format);
	while (!parser.Eof())
	{
		char at = *parser.At();
		if (parser.IsAny("aAZbBhH@xXP"))
		{
			parser.Next();

			int count = 1;
			if (parser.ParseNumber())
				count = ((count = parser.Token.Integer())>0)?count:1;

			switch (at)
			{
			case 'a':case 'A':case 'Z':
				{
					char * data = (char*)va_arg(va,int);
					int size = String::Length(data);

					while(count-- > 0)
					{
						if (size-- > 0)
						{
							packedStream.Write(*data);
							++data;
						}
						else
						{

							packedStream.Write((at=='A')?' ':'\0');
						}
					}
				}
				break;
			case 'h': case 'H':
				{
					char * data = (char*)va_arg(va,int);
					int size = String::Length(data);

					int byte = 0;
					int bits = 0;
					while (count-- > 0)
					{
						if (size-- > 0)
						{
							char hex = 0;
							if ((*data >= 'a' && *data <= 'z') || (*data >= 'A' && *data <= 'Z') || (*data == '0' || *data == '1'))
								hex = Sequences::Hex(data,1);
							else
							if (*data == 0 || *data == 1)
								hex = *data;
							else
								hex = *data&0x0F;

							Bits::Insert(byte,bits,hex,4,(at=='H')?1:-1);
							++data;
						}
						else
						{

							Bits::Insert(byte,bits,'\0',4,(at=='H')?1:-1);
						}

						if (bits%8 == 0)
						{
							packedStream.Write(byte);
							bits=0;
						}
					}

					if (bits)
						packedStream.Write(byte);
				}
				break;
			case 'b': case 'B':
				{

					char * data = (char*)va_arg(va,int);
					int size = String::Length(data);

					char byte = 0;
					int bit = 0;

					int shift = 0;
					while (count-- > 0)
					{
						if (size-- > 0)
						{

							bit = (int)(*data&0xFE);

							byte |= ((at=='b')?bit<<shift:bit<<(8-shift));
							++shift;
							++data;
						}

						if (shift == 8)
						{
							packedStream.Write(byte);
							shift=0;
						}

					}

					if (shift)
						packedStream.Write(byte);
				}
				break;
			}
		}
		else
		if (parser.IsAny("cCsSiIlLnNvVqQjJ") || parser.IsAny("fdFDp"))
		{

			parser.Next();

			int count=1;
			if (parser.ParseNumber())
				count = ((count = parser.Token.Integer())>0)?count:1;

			while (count-- > 0)
			{
				switch (at)
				{

				case 'c':case 'C': packedStream.Write((unsigned char)va_arg(va,int)); break;
				case 's':case 'S': packedStream.Write((unsigned short)va_arg(va,int)); break;
				case 'i':case 'I':
				case 'j':case 'J': packedStream.Write((unsigned int)va_arg(va,int)); break;
				case 'l':case 'L': packedStream.Write((unsigned long)va_arg(va,unsigned long)); break;
				case 'q':case 'Q': packedStream.Write((unsigned long long)va_arg(va,unsigned long long)); break;

				case 'n': packedStream.Write(htons((unsigned short)va_arg(va,int))); break;
				case 'N': packedStream.Write(htonl((unsigned long)va_arg(va,unsigned long))); break;

				case 'v': packedStream.Write((unsigned short)va_arg(va,int)); break;
				case 'V': packedStream.Write((unsigned long)va_arg(va,unsigned long)); break;			

				case 'f':case 'F': packedStream.Write((float)va_arg(va,double)); break;

				case 'd':case 'D': packedStream.Write((double)va_arg(va,double)); break;

				#ifdef REASON_PLATFORM_I386
				case 'p': packedStream.Write((int)(char*)va_arg(va,int)); break;
				#else

				case 'p': packedStream.Write((long long)(char*)va_arg(va,long long)); break;
				#endif

				}
			}
		}
		else
		if (parser.Is("uUw("))
		{

			OutputAssert(false);
			parser.Next();
		}
		else
		{
			parser.Next();
		}
	}

}

void Binary::UnpackVa(Sequence & packed, const char * format, va_list va)
{

}

void Binary::Unpack(Sequence & packed, const char * format, String & unpacked)
{
	String unpack;

	Substring packedSubstring(packed);
	SubstringStream::StreamAdapter packedAdapter(packedSubstring);
	BinaryStream packedStream(packedAdapter);

	StringStream::StreamAdapter unpackedStream(unpacked);

	StringParser parser(format);
	while (!parser.Eof())
	{
		unpack.Release();

		char at = *parser.At();
		if (parser.IsAny("aAZbBhH@xXP"))
		{
			parser.Next();

			int count = 1;
			if (parser.ParseNumber())
				count = ((count = parser.Token.Integer())>0)?count:1;

			switch (at)
			{
			case 'a':case 'A':case 'Z':
				{
					char data;

					while(count-- > 0)
					{
						packedStream.Read(data);
						unpack.Append(data);
					}

					if (at == 'A')
					{

						unpack.TrimRight("\0 ");
					}
					else
					if (at == 'Z')
					{

						unpack.TrimRight("\0");
						unpack.Append('\0');
					}
					else
					if (at == 'a')
					{

					}	
				}
				break;
			case 'h': case 'H':
				{

					char byte = 0;
					int bits = 0;					

					while (count > 0)
					{					
						packedStream.Read(byte);

						unsigned int hex = (unsigned char)((at=='H')?byte:((byte&0x0F)<<4)|((byte&0xF0)>>4));
						String hexed = Numbers::Hex(hex);

						unpack.Append(hexed.Data[0]);
						--count;

						if (count > 0)
						{
							unpack.Append(hexed.Data[1]);
							--count;
						}
					}					
				}
				break;
			case 'b': case 'B':
				{

					char byte = 0;
					int bits = 0;					

					while (count > 0)
					{					
						packedStream.Read(byte);
						bits = (count < 8)?count:8;

						int shift=0;
						while (bits > 0)
						{
							char bit = byte&((at=='B')?1<<(8-shift):1<<shift);
							unpack.Append((bit)?'1':'0');
							++shift;
							--bits;
						}

						count -= 8;
					}
				}
				break;
			}
		}
		else
		if (parser.IsAny("cCsSiIlLnNvVqQjJ") || parser.IsAny("fdFDp"))
		{

			parser.Next();

			int count=1;
			if (parser.ParseNumber())
				count = ((count = parser.Token.Integer())>0)?count:1;

			while (count-- > 0)
			{
				switch (at)
				{

				case 'c':case 'C': 
					{
						unsigned char bits;
						packedStream.Read(bits);
						unpack.Append((char)bits);
					}
					break;
				case 's':case 'S':
					{
						unsigned short bits;
						packedStream.Read(bits);
						unpack.Append((short)bits);
					}
					break;	
				case 'i':case 'I':
				case 'j':case 'J': 
					{
						unsigned int bits;
						packedStream.Read(bits);
						unpack.Append((int)bits);
					}
					break;	
				case 'l':case 'L':
					{
						unsigned long bits;
						packedStream.Read(bits);
						unpack.Append((long)bits);
					}
					break;	
				case 'q':case 'Q':
					{
						unsigned long long bits;
						packedStream.Read(bits);
						unpack.Append((long long)bits);
					}
					break;	

				case 'n': 
					{
						unsigned short bits;
						packedStream.Read(bits);
						unpack.Append((short)ntohs(bits));
					}
					break;	
				case 'N': 
					{
						unsigned long bits;
						packedStream.Read(bits);
						unpack.Append((long)ntohl(bits));
					}
					break;	

				case 'v': 
					{
						unsigned short bits;
						packedStream.Read(bits);
						unpack.Append((short)bits);
					}
					break;
				case 'V': 
					{
						unsigned long bits;
						packedStream.Read(bits);
						unpack.Append((long)bits);
					}
					break;

				case 'f':case 'F':
					{
						float bits;
						packedStream.Read(bits);
						unpack.Append(bits);
					}
					break;

				case 'd':case 'D': 
					{
						double bits;
						packedStream.Read(bits);
						unpack.Append(bits);
					}
					break;

				case 'p': 
					{
						int bits;
						packedStream.Read(bits);
						unpack.Append(bits);
					}
					break;
				}
			}
		}
		else
		if (parser.Is("uUw("))
		{

			OutputAssert(false);
			parser.Next();
		}
		else
		{
			parser.Next();
		}

		unpackedStream.Write(unpack);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int BinaryStream::Read8(unsigned char & bits)
{
	return Read((char &)bits);
}

int BinaryStream::Read16(unsigned short & bits)
{
	return Read((char *)&bits,2);
}

int BinaryStream::Read32(unsigned int & bits)
{
	return Read((char *)&bits,4);
}

int BinaryStream::Read64(unsigned long long & bits)
{
	return Read((char*)&bits,8);
}

int BinaryStream::Write8(const unsigned char & bits)
{
	return Write((char &)bits);
}

int BinaryStream::Write16(const unsigned short & bits)
{
	return Write((char*)&bits,2);
}

int BinaryStream::Write32(const unsigned int & bits)
{
	return Write((char*)&bits,4);
}

int BinaryStream::Write64(const unsigned long long & bits)
{
	return Write((char*)&bits,8);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

