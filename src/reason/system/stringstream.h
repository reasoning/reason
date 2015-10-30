
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
#ifndef SYSTEM_STRINGSTREAM_H
#define SYSTEM_STRINGSTREAM_H

#include "reason/system/stream.h"
#include "reason/system/string.h"

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class StringChannel;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class StringStream : public Reason::System::String, public Reason::System::Stream
{
public:

	StringStream & operator = (const StringStream & data)	{Construct(data.Data,data.Size);return *this;}
	StringStream & operator = (const String & data)			{Construct(data.Data,data.Size);return *this;}
	StringStream & operator = (const Sequence & data)		{Construct(data.Data,data.Size);return *this;}
	StringStream & operator = (const char * data)			{Construct(data);return *this;}
	StringStream & operator = (char c)						{Construct(c);return *this;}
	StringStream & operator = (int i)						{Construct(i);return *this;}
	StringStream & operator = (float f)						{Construct(f);return *this;}
	StringStream & operator = (double d)					{Construct(d);return *this;}

	StringStream(const String & string):String(string) {}
	StringStream(const Sequence & sequence):String(sequence) {}
	StringStream(const char * data):String(data) {}
	StringStream(char * data, int size): String(data,size) {}
	StringStream(char c):String(c) {}
	StringStream(int i):String(i) {}
	StringStream(float f):String(f) {}
	StringStream(double d):String(d) {}
	StringStream();
	~StringStream();

	void Release()
	{
		String::Release();
		Position=0;
	}

	int Seek(int position, int origin = -1)	
	{
		if (origin < 0)
		{

			if (position < 0) return Position;

			Position = position;
		}
		else
		if (origin == 0)
		{

			if (position == 0 || Position+position < 0) return Position;

			Position += position;
		}
		else
		if (origin > 0)
		{

			if (position+Size < 0) return Position;			

			Position = position+Size;
		}

		return Position;
	}

	bool IsReadable()					
	{
		return !IsEmpty() && Position < Size;
	}

	using Stream::Read;

	int Read(char &data)				
	{
		if (Position < Size)
		{
			data = Data[Position++];
			return 1;
		}

		return 0;
	}

	int Read(char * data, int size)		
	{
		if (Position+size <= Size)
		{
			memcpy(data,Data+Position,size);
			Position+=size;
			return size;
		}
		else
		{
			int remaining = Size - Position;
			if (remaining > 0)
			{
				memcpy(data,Data+Position,remaining);
				Position += remaining;
				return remaining;
			}
		}

		return 0;
	}

	bool IsWriteable()					
	{
		return true;
	}

	using Stream::Write;

	int Write(const char &data)				
	{
		if (Position+1 > Size)
			Right((Position-Size)+1);

		Data[Position++] = data;
		Terminate();
		return 1;
	}

	int Write(char * data , int size)	
	{
		if (Position+size > Size)
		{

			int remaining = Size - Position;
			Right(size-remaining);
		}

		memcpy(Data+Position,data,size);
		Position+=size;
		return size;
	}

	int Write(const char * data)
	{
		return Write((char*)data,strlen(data));
	}

public:

	class StreamAdapter : public Stream
	{
	public:

		Reason::System::String * String;

		StreamAdapter(Reason::System::String & string):String(&string){};
		StreamAdapter(Reason::System::String * string):String(string){};
		~StreamAdapter(){};

		int Seek(int position, int origin = -1)	
		{
			if (origin < 0)
			{

				if (position < 0) return Position;
				Position = position;
			}
			else
			if (origin == 0)
			{

				if (position == 0 || Position+position < 0) return Position;
				Position += position;
			}
			else
			if (origin > 0)
			{

				if (Position+position < 0) return Position;
				Position = String->Size-position;
			}

			return Position;
		}

		bool IsReadable()					
		{
			return !String->IsEmpty() && Position < String->Size;
		}

		using Stream::Read;
		int Read(char &data)				
		{
			if (Position < String->Size)
			{
				data = String->Data[Position++];
				return 1;
			}
			else
			{
				return 0;
			}
		}

		int Read(char * data, int size)		
		{
			if (Position+size <= String->Size)
			{
				memcpy(data,String->Data+Position,size);
				Position+=size;
				return size;
			}
			else
			{
				int remaining = String->Size - Position;
				memcpy(data,String->Data+Position,remaining);
				Position += remaining;
				return remaining;
			}
		}

		bool IsWriteable()					
		{
			return true;
		}

		using Stream::Write;
		int Write(const char &data)				
		{
			if (Position+1 > String->Size)
				String->Right((Position-String->Size)+1);

			String->Data[Position++] = data;
			String->Terminate();
			return 1;
		}

		int Write(char * data , int size)	
		{
			if (Position+size > String->Size)
			{
				int remaining = String->Size - Position;
				String->Right(size-remaining);
			}

			memcpy(String->Data+Position,data,size);
			Position+=size;
			return size;
		}

	};

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SubstringStream : public Reason::System::Substring, public Reason::System::Stream
{
public:

	SubstringStream(const String & string):Substring(string) {}	
	SubstringStream(const Substring & substring):Substring(substring) {}
	SubstringStream(const Sequence & sequence):Substring(sequence) {}
	SubstringStream(const Sequence & sequence, int from, int to):Substring(sequence,from,to) {}
	SubstringStream(const Sequence & sequence, int from):Substring(sequence,from) {}
	SubstringStream (const char * data):Substring(data) {}
	SubstringStream(char *data, int size):Substring(data,size) {}
	SubstringStream() {}
	~SubstringStream() {}

	SubstringStream & operator = (const char *data)				{Assign((char*)data,String::Length(data));return *this;}
	SubstringStream & operator = (const Sequence & sequence)	{Assign(sequence.Data,sequence.Size);return *this;}
	SubstringStream & operator = (const Substring & substring)	{Assign(substring.Data,substring.Size);return *this;}

	void Release()
	{
		Substring::Release();
		Position=0;
	}

	int Seek(int position, int origin = -1)	
	{
		if (origin < 0)
		{

			if (position < 0) return Position;

			Position = position;
		}
		else
		if (origin == 0)
		{

			if (position == 0 || Position+position < 0) return Position;

			Position += position;
		}
		else
		if (origin > 0)
		{

			if (position+Size < 0) return Position;			

			Position = position+Size;
		}

		return Position;
	}

	bool IsReadable()					
	{
		return !IsEmpty() && Position < Size;
	}

	using Stream::Read;

	int Read(char &data)				
	{
		if (Position < Size)
		{
			data = Data[Position++];
			return 1;
		}

		return 0;
	}

	int Read(char * data, int size)		
	{
		if (Position+size <= Size)
		{
			memcpy(data,Data+Position,size);
			Position+=size;
			return size;
		}
		else
		{
			int remaining = Size - Position;
			if (remaining > 0)
			{
				memcpy(data,Data+Position,remaining);
				Position += remaining;
				return remaining;
			}
		}

		return 0;
	}

	bool IsWriteable()					
	{
		return Position < Size;
	}

	using Stream::Write;

	int Write(const char &data)				
	{
		if (Position+1 > Size) return 0;
		Data[Position++] = data;
		return 1;
	}

	int Write(char * data , int size)	
	{
		if (Position+size > Size)
		{

			int remaining = Size - Position;
			size = remaining;
		}

		memcpy(Data+Position,data,size);
		Position+=size;
		return size;
	}

	int Write(const char * data)
	{
		return Write((char*)data,strlen(data));
	}

public:

	class StreamAdapter : public Stream
	{
	public:

		Reason::System::Substring & Substring;

		StreamAdapter(Reason::System::Substring &substring):Substring(substring){};
		~StreamAdapter(){};

		int Seek(int position, int origin = -1)	
		{
			if (origin < 0)
			{

				if (position < 0) return Position;
				Position = position;
			}
			else
			if (origin == 0)
			{

				if (position == 0 || Position+position < 0) return Position;
				Position += position;
			}
			else
			if (origin > 0)
			{

				if (Position+position < 0) return Position;
				Position = Substring.Size-position;
			}

			return Position;
		}

		bool IsReadable()					
		{
			return !Substring.IsEmpty() && Position < Substring.Size;
		}

		using Stream::Read;
		int Read(char &data)				
		{
			if (Position < Substring.Size)
			{
				data = Substring.Data[Position++];
				return 1;
			}
			else
			{
				return 0;
			}
		}

		int Read(char * data, int size)		
		{
			if (Position+size <= Substring.Size)
			{
				memcpy(data,Substring.Data+Position,size);
				Position+=size;
				return size;
			}
			else
			{
				int remaining = Substring.Size - Position;
				memcpy(data,Substring.Data+Position,remaining);
				Position += remaining;
				return remaining;
			}
		}

		bool IsWriteable()					
		{
			return Position < Substring.Size;
		}

		using Stream::Write;
		int Write(char &data)				
		{
			if (Position+1 > Substring.Size) return 0;
			Substring.Data[Position++] = data;
			return 1;
		}

		int Write(char * data , int size)	
		{
			if (Position+size > Substring.Size)
			{
				int remaining = Substring.Size - Position;
				size = remaining;
			}

			memcpy(Substring.Data+Position,data,size);
			Position+=size;
			return size;
		}

	};

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

