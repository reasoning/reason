
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

#include "reason/system/tokenizer.h"
#include "reason/system/parser.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Tokenizer::Tokenizer()
{
}

Tokenizer::Tokenizer(Sequence & sequence): 
	Substring(sequence)
{

}

void Tokenizer::Tokenise()
{

	Numbers.Destroy();
	Words.Destroy();
	Punctuation.Destroy();

	StringParser parser;
	parser.Assign(*this);

	while (! parser.Eof())
	{
		if (parser.ParseWord())
		{
			Words.Append(parser.Token);
		}
		else
		if (parser.ParseNumber())
		{
			Numbers.Append(parser.Token);
		}
		else
		if (parser.ParsePunctuation())
		{
			Punctuation.Append(parser.Token);
		}
		else
		{
			if (!parser.ParseWhitespace() && !parser.Eof())
				parser.Next();	
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TokenizerStream::TokenizerStream(Reason::System::Stream & stream):Stream(stream)
{
}

TokenizerStream::~TokenizerStream()
{
}

bool TokenizerStream::Move()
{
	return Move(1);
}

bool TokenizerStream::Move(int amount)
{
	amount *= Enumeration.Direction;

	if (amount > 0)
	{
		++ Enumeration.Index;
		Enumeration.Prev = Token;

		if (Enumeration.Next.IsNull())
		{
			Token =	Next();
		}
		else
		{
			Token = Enumeration.Next;
			Enumeration.Next.Release();
		}
	}
	else
	if (amount < 0)
	{		
		-- Enumeration.Index;
		Enumeration.Next = Token;

		if (Enumeration.Prev.IsNull())
		{
			Token = Prev();
		}
		else
		{
			Token = Enumeration.Prev;
			Enumeration.Prev.Release();
		}
	}

	return !Token.IsNull();
}

bool TokenizerStream::Forward()
{
	Enumeration.Index = 0;
	Enumeration.Direction = 1;

	Buffer.Release();
	return Move();

}

bool TokenizerStream::Reverse()
{
	OutputError("StreamEnumerator::Last - Method not implemented.\n");
	return true;
}

bool TokenizerStream::Has()
{
	return !Token.IsNull();
}

Substring TokenizerStream::Next()
{
	if (Buffer.Allocated == 0)
	{
		if (Buffer.Remaining() < 256)
			Buffer.Reserve(256);
		Buffer.Size = Stream.Read(Buffer.Data+Buffer.Size,256);
		Buffer.Data[Buffer.Size]=0;
	}

	Path::Enumerator separator(Separators);
	Substring match;

	if (Separators.IsEmpty()) return match;

	int found = -1;
	int index = 0;

    Buffer.Left(-(Token.Size+Separator.Size));

	Token.Release();
	Separator.Release();

	while(match.IsNull() && (Stream.IsReadable() || Buffer.Size > 0))
	{

		for(separator.Forward();separator.Has();separator.Move())
		{
			index = Buffer.IndexOf(separator.Reference());
			if ( (index >= 0) && (index < found || found < 0)  )
			{

				match = Buffer.Slice(0,index);					
				found = index;
				Separator.Assign(separator.Reference());

				if (index==0) break;
			}
		}

		if (match.IsNull())
		{
			if (Stream.IsReadable())
			{

				if (Buffer.Remaining() < 256)
					Buffer.Reserve(256);
				Buffer.Size += Stream.Read(Buffer.Data+Buffer.Size,256);
				Buffer.Data[Buffer.Size]=0;
			}
			else
			{

				match = Buffer;
			}
		}
	}

	return match;
}

Substring TokenizerStream::Prev()
{

	OutputError("StreamEnumerator::Last - Method not supported.\n");
	Substring found;
	return found;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

