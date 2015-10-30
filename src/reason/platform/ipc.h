
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


#ifndef PLATFORM_IPC_H
#define PLATFORM_IPC_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/reason.h"
#include "reason/system/file.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Platform {

class Pipe 
{
public:

	static const int PipeError = -1;
	static const int PipeInvalid = ~(0);

	enum PipeOptions
	{
		ACCESS_READ,
		ACCESS_WRITE,
		ACCESS_MUTABLE,

		TYPE_BINARY,
		TYPE_MESSAGE,

	};

	#ifdef REASON_PLATFORM_POSIX
	FILE * Handle;
	#endif

	#ifdef REASON_PLATFORM_WINDOWS
	int Handle;
	#endif

	int Options;

	Pipe();
	~Pipe();

	bool IsReadable() {return Options&ACCESS_READ;}
	bool IsWriteable() {return Options&ACCESS_WRITE;}

	bool Create(int mode);
	bool Create(const char * name, int mode);

	bool Open(const char * name, int mode);
	bool Close();
};

class Section
{
public:

	char * Data;
	int Size;
	int Allocated;

	Section();
	~Section();

	bool Create(char * data, int size, int allocated = 4096);

	bool Open(char * data, int size);
	bool Close();

	int Read(char * data, int size, int offset=0);
	int Write(char * data, int size, int offset=0);

protected:

	#ifdef REASON_PLATFORM_WINDOWS
	unsigned int Handle;
	#endif

};

class Notify
{
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Mapping : Reason::System::File
{
public:

	bool Open(char * data, int size);
	bool Close();

};

class Map : public Reason::System::FileStorage
{

};

}}

#endif