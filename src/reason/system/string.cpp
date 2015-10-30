
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

#include "reason/system/string.h"
#include "reason/system/format.h"
#include "reason/system/output.h"
#include "reason/system/character.h"
#include "reason/system/indices.h"
#include "reason/language/regex/regex.h"

#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>		

#ifdef REASON_PLATFORM_POSIX
	#include <unistd.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;
using namespace Reason::Language::Regex;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Substring::Substring(const Reason::System::String & string):Sequence(string.Data,string.Size)
{
}

Substring & Substring::Reverse()
{
	char * data = Data;
	int size = Size/2;
	int index = Size-1;
	char c = 0;
	while (data < Data+index)
	{
		c = *data;
		*data = Data[index];
		Data[index] = c;
		--index;
		++data;
	}

	return *this;
}

Substring & Substring::Left(int amount)
{

	Data -= amount;
	Size += amount;
	return *this;
}

Substring & Substring::Right(int amount)
{

	Size += amount;
	return *this;
}

Substring & Substring::Trim(int amount)
{
	TrimLeft(amount);
	TrimRight(amount);	
	return *this;
}

Substring & Substring::TrimLeft(int amount)
{
	if (IsEmpty() || !(amount > 0)) return *this;
	Left(-amount);
	return *this;
}

Substring & Substring::TrimRight(int amount)
{
	if (IsEmpty() || !(amount > 0)) return *this;
	Right(-amount);
	return *this;
}

Substring & Substring::Trim(char * matches, int matchesSize)
{
	TrimLeft(matches,matchesSize);
	TrimRight(matches,matchesSize);
	return *this;
}

Substring & Substring::TrimLeft(char * matches, int matchesSize)
{
	if (IsEmpty()) return *this;

	char *left = Data;
	char *right = Data+Size;
	char *match = 0;

	while (left != right)
	{

		match = left;
		for (int m=0;match != right && m < matchesSize;++m)
			if (*match == matches[m])
				++match;

		if (match == left)
			break;

		left = match;
	}

	if (left != Data)
	{

		Size -= left-Data;
		Data = left;
	}

	return *this;
}

Substring & Substring::TrimRight(char * matches, int matchesSize)
{
	if (IsEmpty()) return *this;

	char *right = PointerAt(Size-1);
	char *left = Data-1;
	char *match = 0;

	while (right != left)
	{

		match = right;
		for (int m=0;match != left && m < matchesSize;++m)
			if (*match == matches[m])
				--match;

		if (match == right)
			break;

		right = match;
	}

	if (right != PointerAt(Size-1))
	{

		Size -= PointerAt(Size-1)-right;
	}

	return *this;
}

Substring Substring::Slice(int from)
{

	if (Indices::Normalise(from,Size))
		return Reason::System::Substring(Data+from,Size-from);

	return Reason::System::Substring();
}

Substring Substring::Before(int index)
{

	Indices::Normalise(index,Size);	

	if (index >= 0)
		return Slice(0,index);

	return Reason::System::Substring();
}

Substring Substring::After(int from, char * data, int size)
{
	if (Indices::Normalise(from,Size))
	{

		int index = IndexOf(from,data,size);
		if (index >= 0)
			return Slice(index+size);
	}

	return Reason::System::Substring();
}

Substring Substring::After(int index)
{

	if (Indices::Normalise(index,Size))
		return Slice(index+1);

	return Reason::System::Substring();
}

char * Substring::At(int index)
{
	if (Indices::Normalise(index,Size))
		return Data+index;

	return 0;
}

Reason::System::Superstring Substring::Superstring()
{
	return Reason::System::Superstring(Data,Size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String::String(const Sequence & sequence, int from, int to):Allocated(0)
{
	Construct(Reason::System::Superstring(sequence,from,to));
}

String::String(const Sequence & sequence, int from):Allocated(0)
{
	Construct(Reason::System::Superstring(sequence,from));
}

String::~String()
{
	if (Allocated)
	{
		OutputAssert(Data != 0);		
		delete [] Data;
	}
}

void String::Destroy()
{
	if (Allocated)
		delete [] Data; 

	Data=0;
	Size=0;
	Allocated=0;

}

void String::Release()
{
	if (Allocated)
	{

		Size=0;	
		OutputAssert(Data != 0);
		Terminate();
	}
	else
	{
		Data=0;
		Size=0;
		Allocated=0;

	}
}

String & String::Construct()
{

	if (IsSubstring())
	{
		if (Allocated)
		{
			Construct("");
		}
		else
		{
			char * data = Data;
			int size = Size;
			Data = 0;
			Size = 0;
			Allocated = 0;

			if (data && size > 0)
				Construct(data,size);
		}

	}

	return *this;
}

String & String::Construct(const Reason::System::Superstring & superstring)
{
	return Acquire(superstring);	
}

String & String::Construct(char * data, int size)
{

	OutputAssert(Data == 0 || (data < Data || data >= (Data+Size)));
	if (data >= Data && data < (Data+Size)) return *this;

	Stackstring stack;
	if (data >= Data && data < (Data+Size))
	{
		stack.Construct(data,size);
		data = stack.Data;
		size = stack.Size;
	}

	OutputAssert((data == 0 && size == 0) || (data != 0 && size >= 0)); 

	if (data == 0) 
	{

		Destroy();
		return *this;
	}

	if (size != Size || Data == 0)
		Resize(size);

	if (data && size > 0)
	{

		memcpy(Data,data,size);
	}

	return *this;
}

String & String::Construct(int i)
{
	return Construct((long long)i);
}

String & String::Construct(long long ll)
{
	String string;
	string.Allocate(32);
	string.Format("%lld",ll);
	return Construct(string);

}

String & String::Construct(double d)
{
	String string;
	string.Allocate(512);
	string.Format("%f",d);
	return Construct(string);

}

String & String::Construct(float f)
{
	String string;
	string.Allocate(64);
	string.Format("%g",f);
	return Construct(string);

}

String & String::Construct(char c)
{	
	Allocate(1);
	Data[0]=c;			
	return Terminate();
}

String & String::Construct(int times, char * data, int size)
{
	if (times > 0)
	{
		int pow = 0;
		Size = 0;
		Allocate(size*times);
		Append(data,size);
		if (size > 0 && *data != 0)
		{
			while ((1 << (pow+1)) < times)
			{
				Append(*this);
				++pow;
			}

			times = times - (1<<pow);
			while (times-- > 0)
			{
				Append(data,size);
			}
		}
	}

	return *this;
}

String & String::operator *= (int times)
{
	if (times > 0)
	{
		int pow = 0;
		int size = Size;
		Allocate(Size*times);
		while ((1 << (pow+1)) < times)
		{

			Append(*this);
			++pow;
		}

		times = times - (1<<pow);
		while (times-- > 0)
		{
			Append(Data,size);
		}
	}

	return *this;
}

String & String::Format(const char * format, ...)
{

	int size = String::Length(format)*1.25;

	Reason::System::Superstring string;
	string.Allocate(size);

	va_list va;
	va_start(va, format);	
	Formatter::FormatVa(string,format,va);
	va_end(va);

	Construct(string);

	return *this;
}

String & String::Replace(int i)
{
	String string;
	string.Allocate(32);
	string.Format("%d",i);
	return Replace(string);

}

String & String::Replace(long long ll)
{
	String string;
	string.Allocate(32);
	string.Format("%d",ll);
	return Replace(string);

}

String & String::Replace(double d)
{
	String string;
	string.Allocate(512);
	string.Format("%f",d);
	return Replace(string);

}

String & String::Replace(float f)
{
	String string;
	string.Allocate(64);
	string.Format("%g",f);
	return Replace(string);

}

String & String::Replace(int from, int to, char * data, int size)
{

	if (from < 0 || !(from <= Size)) return *this; 
	if (!(to >= from) || !(to <= Size)) return *this;

	if (!Allocated) Construct();

	int diff = size-(to-from);

	if (diff < 0)
	{
        memmove(Data+to+diff,Data+to,Size-to);
		Right(diff);
	}
	else
	if (diff > 0)
	{
		Right(diff);
		memmove(Data+to+diff,Data+to,Size-to);
	}

    memcpy(Data+from,data,size); 
	return Terminate();
}

String & String::Acquire(char *& data, int & size)
{
	if (Allocated)
	{
		OutputAssert(data+size <= Data || data >= Data+Size);
		if (data+size >= Data && data <= Data+Size) return *this;
		Destroy();
	}

	Data = data;
	Size = size;
	Allocated = size;

	data = 0;
	size = 0;
	return *this;
}

String & String::Replace(int limit, Reason::Language::Regex::Regex & regex, char * data, int size)
{

	if (limit > 0) ++limit;

	regex.Match(limit,Data,Size);

	Array< Array< Pair<int,int> > > offsets;
	for (int m = 0;m < regex.Matches.Length() && (limit == 0 || --limit > 0);++m)
	{
		offsets.Append(Array< Pair<int,int> >());
		for (int mm = 0;mm < regex.Matches[m].Length();++mm)
			offsets[m] << Pair<int,int>(regex.Matches[m][mm].Data-Data,regex.Matches[m][mm].Size);
	}

	regex.Matches.Release();

	int shift = 0;
	for (int o = 0;o < offsets.Length() && (limit == 0 || --limit > 0);++o)
	{
		Substring matched = Substring(Data+offsets[o][0].First()+shift,offsets[o][0].Second());

		String string;
		StringParser parser(data,size);
		parser.Mark();
		while (!parser.Eof())
		{
			if (parser.Is('\\'))
			{
				parser.Trap();
				string << parser.Token;

				parser.Next();
				parser.Mark();
				if (parser.SkipDecimal())
				{
					parser.Trap();

					int capture = parser.Token.Trim().Decimal();

					if (capture > 0 && capture < offsets[o].Length())
					{
						string << Substring(Data+offsets[o][capture].First()+shift,offsets[o][capture].Second());
					}
					else
					{
						string << parser.Token;
					}

					parser.Mark();
				}
			}
			else
			{
				parser.Next();
			}
		}

		parser.Trap();
		string << parser.Token;

		shift += string.Size-matched.Size;

		Replace(matched.Data-Data,(matched.Data-Data)+matched.Size,string);
	}

	return *this;
}

String & String::Acquire(const Sequence & sequence)
{
	if (Allocated)
	{
		OutputAssert(sequence.Data+sequence.Size <= Data || sequence.Data >= Data+Size);
		if (sequence.Data+sequence.Size >= Data && sequence.Data <= Data+Size) return *this;

		Destroy();
	}

	Data = sequence.Data;
	Size = sequence.Size;
	Allocated = sequence.Size;

	((Sequence&)sequence).Data = 0;
	((Sequence&)sequence).Size = 0;
	return *this;
}

String & String::Acquire(const String & string)
{
	if (Allocated)
	{

		OutputAssert(string.Data+string.Size <= Data || string.Data >= Data+Size);
		if (string.Data+string.Size >= Data && string.Data <= Data+Size) return *this;

		Destroy();
	}

	Data = string.Data;
	Size = string.Size;
	Allocated = string.Allocated;

	((String&)string).Data = 0;
	((String&)string).Size = 0;
	((String&)string).Allocated = 0;
	return *this;
}

String & String::Assign(char *data, int size)
{
	OutputAssert((Size > 0 && Data != 0) || (Size == 0));

	if (Allocated)
	{

		OutputAssert(data+size <= Data || data >= Data+Size);
		if (data+size >= Data && data <= Data+Size) return *this;

		Destroy();
	}

	Data = (char*)data;
	Size = (int)size;

	if (Size < 0) Size = 0;

	Allocated = 0;
	return *this;
}

String & String::Append(char c)
{	
	Right(1);
	Data[Size-1]=c;			
	return *this;
}

String & String::Append(short s)
{
	return Append((long long)s);
}

String & String::Append(int i)
{
	return Append((long long)i);
}

String & String::Append(long long ll)
{

	String string;
	string.Allocate(32);
	string.Format("%d",ll);
	return Append(string);

}

String & String::Append(double d)
{

	String string;
	string.Allocate(512);
	string.Format("%f",d);
	return Append(string);

}

String & String::Append(float f)
{

	String string;
	string.Allocate(64);
	string.Format("%g",f);
	return Append(string);

}

String & String::Append(char *data, int size)
{
	return Insert(data,size,Size);
}

String & String::Append(const Reason::System::Superstring & superstring)
{
	if (IsEmpty())
		return Construct(superstring);
	else
		return Append(superstring.Data,superstring.Size);
}

String & String::Prepend(char c)
{	
	Left(1);
	Data[0]=c;
	return *this;
}

String & String::Prepend(short s)
{
	return Prepend((long long)s);
}

String & String::Prepend(int i)
{
	return Prepend((long long)i);
}

String & String::Prepend(long long ll)
{

	String string;
	string.Allocate(32);
	string.Format("%d",ll);
	return Prepend(string);

}

String & String::Prepend(double d)
{
	String string;
	string.Allocate(512);
	string.Format("%f",d);
	return Prepend(string);

}

String & String::Prepend(float f)
{
	String string;
	string.Allocate(64);
	string.Format("%g",f);
	return Prepend(string);

}

String & String::Prepend(char *data, int size)
{	
	return Insert(data,size,0);
}

String & String::Prepend(const Reason::System::Superstring & superstring)
{
	if (IsEmpty())
		return Construct(superstring);
	else
		return Prepend(superstring.Data,superstring.Size);
}

String & String::Insert(short s, int index)
{
	return Insert((long long)s, index);
}

String & String::Insert(int i, int index)
{
	return Insert((long long)i, index);
}

String & String::Insert(long long ll, int index)
{
	String string;
	string.Allocate(32);
	string.Format("%d",ll);
	return Insert(string,index);
}

String & String::Insert(double d, int index)
{
	String string;
	string.Allocate(512);
	string.Format("%f",d);
	return Insert(string,index);
}

String & String::Insert(float f, int index)
{
	String string;
	string.Allocate(64);
	string.Format("%g",f);
	return Insert(string,index);
}

String & String::Insert(char *data, int size, int index)
{

	OutputAssert((data == 0 && size == 0) || (data != 0 && size >= 0)); 

	if (data == 0 || size < 0) 	return *this;

	Stackstring string;
	if (data >= Data && data < (Data+Size))
	{
		string.Construct(data,size);
		data = string.Data;
		size = string.Size;
	}

	Indices::Normalise(index,Size);

	OutputAssert(index >=0 && index <= Size);

	int offset = Size;
	Resize(Size+size);

	if (index == offset)
	{
		memcpy(Data+offset,data,size);
	}
	else
	{
		memmove(Data+index+size,Data+index,offset-index);
		memcpy(Data+index,data,size);
	}

	return Terminate();
}

String & String::Replace(int limit, char match, char data)
{

	if (IsEmpty())
		return *this;

	if (!Allocated) Construct();

	if (data == 0)
	{

		char *c = Data;
		while ( c <= PointerAt(Size-1) )
		{
			if (*c == match)
			{
				memmove(c,c+1,Size - (c+1-Data));
				--Size;
				Data[Size]=0;

				if (limit != 0 && --limit <= 0)
					break;
			}

			++c;
		}
	}
	else
	{

		char *c = Data;
		while ( c <= PointerAt(Size-1) )
		{
			if ( *c == match )
			{
				*c = data;
				if (limit != 0 && --limit <= 0)
					break;
			}

			++c;
		}
	}
	return *this;
}

String & String::Replace(int limit, char *match, int matchSize,char *data, int dataSize)
{
	if (IsEmpty())
		return *this;

	if (!Allocated) Construct();

	int index = IndexOf(match,matchSize);
	int diff = dataSize - matchSize;

	if (limit > 0) ++limit;
	while (index != -1 && (limit == 0 || --limit > 0))
	{
		if (diff == 0)
		{

			memcpy(Data+index,data,dataSize);
		}
		else
		if (diff < 0)
		{

			memcpy(Data+index,data,dataSize);
			memmove(Data+index+dataSize,Data+index+matchSize,Size-(index+matchSize));

			Right(diff);
		}
		else
		if (diff > 0)
		{

			int size = Size;

			Right(diff);

			memmove(Data+index+dataSize,Data+index+matchSize,size - (Data+index+matchSize-Data));

			memcpy(Data+index,data,dataSize);
		}

		index = IndexOf(index+dataSize,match,matchSize);
	}

	return *this;
}

String & String::Translate(char * matches, int matchesSize, char *replacements, int replacementsSize)
{
	if (!Allocated) Construct();

	for (int m=0;m<matchesSize;++m)
	{
		for(int d=0;d<Size;++d)
		{
			if (Data[d] == matches[m])
			{

				if ( ! (m < replacementsSize) )
				{

					memmove(PointerAt(d),PointerAt(d+1),PointerAt(Size)-PointerAt(d+1));
					Right(-1);

					--d;
				}
				else
				{
					Data[d] = replacements[m];
				}
			}
		}
	}

	return *this;
}

String & String::Uppercase()
{
	if (!Allocated) Construct();
	Sequence::Uppercase(Data,Size);
	return *this;
}

String & String::Lowercase()
{
	if (!Allocated) Construct();
	Sequence::Lowercase(Data,Size);
	return *this;
}

String & String::Proppercase()
{
	if (!Allocated) Construct();
	Sequence::Proppercase(Data,Size);
	return *this;
}

String & String::Trim(int amount)
{
	TrimLeft(amount);
	TrimRight(amount);
	return *this;
}

String & String::TrimLeft(int amount)
{

	if (IsEmpty() || amount <= 0 || amount > Size) return *this;

	if (!Allocated)
	{
		Substring::TrimLeft(amount);
	}
	else
	{
		Left(-amount);
	}
	return *this;
}

String & String::TrimRight(int amount)
{
	if (IsEmpty() || amount <= 0 || amount > Size) return *this;

	if (!Allocated)
	{
		Substring::TrimRight(amount);
	}
	else
	{
		Right(-amount);
	}
	return *this;
}

String & String::Trim(char * matches, int matchesSize)
{
	TrimLeft(matches,matchesSize);
	TrimRight(matches,matchesSize);
	return *this;
}

String & String::TrimLeft(char * matches, int matchesSize)
{
	if (IsEmpty()) return *this;

	if (!Allocated)
	{
		Substring::TrimLeft(matches,matchesSize);
	}
	else
	{
		char *left = Data;
		char *right = Data+Size;
		char *match = 0;

		while (left != right)
		{

			match = left;
			for (int m=0;match != right && m < matchesSize;++m)
				while (match != right && *match == matches[m])
					++match;

			if (match == left)
				break;

			left = match;
		}

		if (left != Data)
		{
			memmove(Data,left,Size - (left-Data));
			Right(-(left-Data));
		}
	}

	return *this;
}

String & String::TrimRight(char * matches, int matchesSize)
{
	if (IsEmpty()) return *this;

	if (!Allocated)
	{
		Substring::TrimRight(matches,matchesSize);
	}
	else
	{

		char *right = PointerAt(Size-1);
		char *left = Data-1;
		char *match = 0;

		while (right != left)
		{

			match = right;
			for (int m=0;match != left && m < matchesSize;++m)
				while (match != left && *match == matches[m])
					--match;

			if (match == right)
				break;

			right = match;
		}

		if (right != PointerAt(Size-1))
		{
			Right(-(PointerAt(Size-1)-right));
		}

	}

	return *this;
}

char * String::Print() 
{
	OutputAssert(Allocated == 0 || Data != 0);
	OutputAssert(Size >= 0);

	return (Data == 0 || (Allocated && Data[Size]==0))?Data:Printable::Print();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int String::Remaining()
{

	return (Allocated-Size-1);
}

String & String::Clear()
{
	Release();
	return *this;
}

String & String::Allocate(int amount)
{
	OutputAssert((Size > 0 && Data != 0) || (Size == 0));

	if (amount == 0)
	{

		if (Allocated > 0)
			amount = Allocated * (1 + Increment());
		else
		if (Size > 0)
			amount = Size;
		else
			amount = 1;

	}
	if (amount < 0 || amount < Size)
	{

		amount = Size;
	}

	amount += 1;	

	char * data = new char[amount];
	if (Size > 0) 
		memcpy(data,Data,sizeof(char)*Size);

	if (Allocated)
		delete [] Data;

	Data = data;
	Allocated = amount;

	Terminate();

	return *this;
}

String & String::Reserve(int amount)
{

	OutputAssert((Size > 0 && Data != 0) || (Size == 0));

	int remaining = Remaining();

	if (amount < 0)
	{
		amount = Size+(remaining-amount);
		if (amount < Size) amount = Size;
	}
	else
	if (amount > 0)
	{
		if (amount <= remaining) return *this;

		amount = Size * (1 + Increment()) + amount;
	}
	else
	{
		amount = Size;
	}

	return Allocate(amount);
}

String & String::Left(int amount)
{
	OutputAssert((Size > 0 && Data != 0) || (Size == 0));

	if (amount > 0)
	{
		Reserve(amount);
		memmove(Data+amount,Data,Size);
	}
	else
	if (amount < 0)
	{
		memmove(Data,Data-amount,Size+amount);
	}
	else
	{
		return *this;
	}

	Size += amount; 
	Terminate();
	return *this;
}

String & String::Right(int amount)
{
	OutputAssert((Size > 0 && Data != 0) || (Size == 0));

	if (amount > 0)
	{
		Reserve(amount);
	}
	else
	if (amount == 0)
	{

		return *this;
	}

	Size += amount;
	Terminate();
	return *this;
}

String & String::Terminate()
{

	OutputAssert((Allocated == 0 || Allocated > Size) && Size >= 0);
	if (Data && Allocated > Size && Size >= 0) Data[Size]=0;
	return *this;
}

String & String::Resize(int amount)
{
	OutputAssert((Size > 0 && Data != 0) || (Size == 0));

	if (!Allocated) 
	{	

		if (Data != 0 && Size > 0)
			Size = amount;

		Allocate(amount);
		Size = amount;
		Terminate();
		return *this;
	}

	if (amount == 0 || amount >  0)
	{

		return Right(amount-Size);
	}
	else
	if (amount < 0)
	{
		OutputError("String::Resize - Cannot resize a negative amount.\n");
		OutputAssert(amount >= 0);
		return *this;		
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Superstring Superstring::Partial(const Superstring & superstring)
{

	if (superstring.Allocated) 
		return Superstring();

	return superstring;
}

Superstring Superstring::Partial(const String & string)
{

	return Reason::System::Superstring(Reason::System::Substring(string));
}

Superstring Superstring::Partial(char * data, int size)
{

	return Reason::System::Superstring(Reason::System::Substring(data,size));
}

Superstring Superstring::Literal(const Superstring & superstring)
{
	if (superstring.Allocated && superstring.Data[superstring.Size]==0)
		return superstring;
	else
		return String(superstring);
}

Superstring Superstring::Literal(const String & string)
{

	Reason::System::Superstring superstring;
	if (string.Allocated && string.Data[string.Size]==0)
		superstring.Assign(string);
	else
		superstring.Append(string);

	return superstring;
}

Superstring Superstring::Literal(const Sequence & sequence)
{

	return Literal(sequence.Data,sequence.Size);
}

Superstring Superstring::Literal(char * data, int size)
{

	return Reason::System::Superstring(data,size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Reason::Platform::Critical Printer::Critical;
char Printer::Data[Printer::Size];
char * Printer::QueuePointer = Printer::Data;
int	Printer::QueueItemSize = 0;

char * Printer::Load(const char *data, const int size)
{
	Terminate();

	if (data == 0)
		return (char *) data;

	if (QueuePointer==0)
		QueuePointer = Data;

	if (size > Size-2 ) 
	{
		OutputError("Printer:Load - String is not large enough for output string.\n");
		return 0;
	}
	else
	{
		Critical.Enter();

		QueuePointer += QueueItemSize;

		if (size+1 > QueueSpaceAvailable())
		{
			QueuePointer = Data;
		}

		memcpy(QueuePointer,data,size);
		*(QueuePointer+size)=0;	

		QueueItemSize = size +1;	

		Critical.Leave();

		return QueuePointer;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
