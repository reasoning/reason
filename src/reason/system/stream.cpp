
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

#include "reason/system/stream.h"
#include "reason/system/number.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System { 

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Reader::Read(String &string, int amount)
{
	OutputAssert(amount >= 0);
	if (amount < 0) return 0;

	if (amount)
	{

		if (string.Allocated <= amount)
			string.Allocate(amount);

		string.Size = Read(string.Data,amount);
	}
	else
	{

		String reader;
		if (string.Allocated == 0)
			reader.Allocate(0x2000);
		else
			reader.Allocate(string.Allocated);

		amount = reader.Allocated-1;

		OutputAssert(amount > 0);

		int offset = 0;
		reader.Size = 0;

		while(amount > 0 && (amount=Read(reader.Data+offset,amount)))
		{
			reader.Size += amount;
			if (reader.Size == (reader.Allocated-1))
			{

				if (IsReadable())
				{

					reader.Allocate(0);
				}
			}

			offset += amount;
			amount = (reader.Allocated-1)-reader.Size;		
		}

		string.Replace(reader);
	}

	string.Terminate();
	return string.Size;
}

int Reader::Read(Writer & writer, int amount)
{
	OutputAssert(amount >= 0);
	if (amount < 0) return 0;

	String reader;
	reader.Resize((amount>0)?amount:0x2000);
	int read=0;
	int write=0;

	if (amount > 0)
	{
		read = amount = Read(reader,amount);
		write = writer.Write(reader,amount);		
	}
	else
	{
		while ((read += amount = Read(reader)) && amount)
			write += writer.Write(reader,amount);
	}

	if (read != write)
	{
		OutputError("Reader::Read - Amount read and amount written differ.\n");
		return 0;
	}

	return read;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Writer::Write(String & string, int amount)
{
	OutputAssert(amount >= 0);
	if (amount < 0) return 0;

	if (amount > string.Size)
		amount = 0;

	if (amount == 0)
		amount = string.Size;

	return Write(string.Data,amount);
}

int Writer::Write(Reader & reader, int amount)
{
	OutputAssert(amount >= 0);
	if (amount < 0) return 0;

	String writer;
	writer.Resize((amount>0)?amount:0x2000);
	int read=0;
	int write=0;

	if (amount > 0)
	{
		read = amount = reader.Read(writer,amount);
		write = Write(writer,amount);		
	}
	else
	{
		while ((read += amount = reader.Read(writer)) && amount)
			write += Write(writer,amount);
	}

	if (read != write)
	{
		OutputError("Write::Write - Amount read and amount written differ.\n");
		return 0;
	}

	return write;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Buffer::Write(char * data , int size)
{
	String::Append(data,size);
	return size;
}

int Buffer::Read(char * data, int size)
{
	OutputAssert((data == 0 && size == 0) || (data != 0 && size >= 0)); 

	if (data == 0 || size < 0) return 0;

	switch(Mode)
	{
	case MODE_FIFO:
		{
			if (size <= Size)
			{
				memcpy(data,Data,size);
				TrimLeft(size);
				return size;
			}
			else
			{
				int remaining = Size;
				if (remaining > 0)
				{
					memcpy(data,Data,remaining);
					TrimLeft(remaining);
					return remaining;
				}
			}
		}
		break;
	case MODE_LIFO:
		{
			if (size <= Size)
			{
				memcpy(data,Data+Size-size,size);
				TrimRight(size);
				return size;
			}
			else
			{
				int remaining = Size;
				if (remaining > 0)
				{
					memcpy(data,Data+Size-remaining,remaining);
					TrimRight(remaining);
					return remaining;
				}
			}
		}
		break;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Stream::Stream():
	Position(0)
{

}

Stream::~Stream()
{

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

StreamBuffer::StreamBuffer(Reason::System::Stream &stream,int buffer):
	StreamFilter(stream),Offset(0)
{
	if (buffer == 0)
	{

		Buffer.Allocate(4096+1);
	}
	else
	{
		Buffer.Allocate(buffer+1);
	}

	Offset = Buffer.Data;
}

StreamBuffer::~StreamBuffer()
{

	Reset();
}

int StreamBuffer::Seek(int position, int origin)				
{
	if (IsWriting())
	{

		Reset();
	}

	if (origin < 0)
	{
		if (position < 0) return Position;

		int relative = position - Position;
		if (relative > 0)
		{

			position -= Position;
		}
		else

		if (Offset+relative >= Buffer.Data && Offset+relative <= Buffer.Data+Buffer.Size)
		{

			Offset += relative;
			Position += relative;
			position = 0;
		}
		else
		{

			if (IsRelative()) return Position;

			Position = 0;
			Stream->Seek(0);
		}
	}
	else
	if (origin == 0)
	{		
		if (position == 0) return Position;

		int relative = position;
		if (relative > 0)
		{

		}
		else

		if (Offset+relative >= Buffer.Data && Offset+relative <= Buffer.Data+Buffer.Size)
		{

			Offset += relative;
			Position += relative;
			position = 0;
		}
		else
		{

			if (IsRelative()) return Position;

			position = Position + relative;
			Position = 0;
			Stream->Seek(0);
		}
	}
	else
	if (origin > 0)
	{
		if (position > 0) return Position;

		if (IsRelative()) return Position;

		String read;
		read.Resize(Buffer.Allocated-1);
		int size = 0;
		do 
		{

			size = Read(read,read.Size);
		}
		while (size > 0);

		int relative = position;

		if (Offset+relative >= Buffer.Data && Offset+relative <= Buffer.Data+Buffer.Size)
		{

			Offset += relative;
			Position += relative;
			position = 0;
		}
		else
		{

			position = Position + relative;
			Position = 0;
			Stream->Seek(0);
		}
	}

	if (Stream->Position == 0)
	{

		Reset();	
	}

	if (position > 0)
	{

		String read;
		read.Resize(Number::Min(position,Buffer.Allocated-1));

		while (position > 0)
		{
			int size = Read(read,Number::Min(position,Buffer.Allocated-1));
			if (size == 0) break;
			position -= size;				
		}
	}

	return Position;

}

int StreamBuffer::ReadIndirect()
{
	Clear();
	int read = ReadDirect(Buffer.Data,Buffer.Allocated-1);
	Position -= read;
	Buffer.Size = read;
	Buffer.Terminate();
	return read;
}

int StreamBuffer::ReadIndirect(char *data, int size)
{
	if (!IsReadable()) return 0;

	if (IsWriting())
	{

		Flush();
	}

	if( Offset+size < Buffer.PointerAt(Buffer.Size) )
	{

		memcpy(data,Offset,size);
		Offset += size;
		Position += size;
		return size;
	}
	else
	{

		int remaining = Buffer.PointerAt(Buffer.Size) - Offset;
		if (remaining > 0)
		{
			memcpy(data,Offset,remaining);
			Offset += remaining;
			Position += remaining;			
		}

		if (size-remaining < Buffer.Allocated)
		{

			if (ReadIndirect() == 0 && IsReadable())
			{
				OutputError("StreamBuffer::ReadIndirect - Failed to read stream buffer.\n");
				return 0;
			}

			return remaining + ReadIndirect(data+remaining,size-remaining);
		}
		else
		{

			Clear();

			int read = remaining + ReadDirect(data+remaining,size-remaining);
			return read;
		}
	}

}

int StreamBuffer::WriteIndirect()	
{
	int write = WriteDirect(Buffer.Data,Buffer.Size);
	Position -= Buffer.Size;
	Clear();
	return write;
}

int StreamBuffer::WriteIndirect(char *data, int size)
{
	if (!IsWriteable()) return 0;

	if (IsReading())
	{

		Flush();
		Stream->Seek(-Buffer.Size,0);
		Clear();
	}

	if( Offset+size <= Buffer.PointerAt(Buffer.Allocated-1))
	{

		memcpy(Offset,data,size);
		Buffer.Size += size;
		Offset += size;
		Position += size;	
		Buffer.Terminate();
		return size;		
	}
	else
	{

		if (size < Buffer.Allocated)
		{

			int remaining = Buffer.PointerAt(Buffer.Allocated-1) - Offset;
			if (remaining > 0)
			{
				memcpy(Offset,data,remaining);
				Buffer.Size += remaining;
				Offset += remaining;
				Position += remaining;
				Buffer.Terminate();
			}

			if (WriteIndirect() == 0 && IsWriteable())
			{
				OutputError("StreamBuffer::WriteIndirect - Failed to write stream buffer.\n");
				return 0;
			}

			return remaining + WriteIndirect(data+remaining,size-remaining);
		}
		else
		{

			Flush();
			int write = WriteDirect(data,size);
			return write;
		}
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

