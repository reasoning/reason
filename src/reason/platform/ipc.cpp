
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

#include "reason/platform/ipc.h"
#include "reason/system/string.h"
#include "reason/system/storage/storage.h"
#include "reason/system/storage/drive.h"

using namespace Reason::System;

#ifdef REASON_PLATFORM_WINDOWS

#include <windows.h>

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Platform {

#ifdef REASON_PLATFORM_WINDOWS
Section::Section():Handle(0),Data(0),Size(0),Allocated(0)
#else
Section::Section():Data(0),Size(0),Allocated(0)
#endif
{

}

Section::~Section()
{

	#ifdef REASON_PLATFORM_WINDOWS
	if (Handle)
		Close();
	#else
	Close();
	#endif
}

bool Section::Create(char * data, int size, int allocated)
{
	Stackstring stack(data,size);

	#ifdef REASON_PLATFORM_WINDOWS
	if (Handle != 0)
		Close();

	unsigned int Handle = (unsigned int)CreateFileMapping(INVALID_HANDLE_VALUE,0,PAGE_READWRITE,0,allocated,stack.Data);
	if (Handle == 0)
	{
		OutputMessage("Section::Create - Could not create mapping.\n");
		return false;
	}

	Data = (char*) MapViewOfFile((void*)Handle,FILE_MAP_ALL_ACCESS,0,0,0);
	if (Data == 0)
	{
		OutputMessage("Section::Create - Could not create view.\n");
		return false;
	}

	return true;

	#endif
}

bool Section::Open(char * data, int size)
{
	Stackstring stack(data,size);

	#ifdef REASON_PLATFORM_WINDOWS
	if (Handle != 0)
		Close();

	Handle = (unsigned int)OpenFileMapping(FILE_MAP_ALL_ACCESS,false,stack.Data);
	if (Handle == 0)
	{
		OutputMessage("Section::Create - Could not open mapping.\n");
		return false;
	}

	Data = (char*) MapViewOfFile((void*)Handle,FILE_MAP_ALL_ACCESS,0,0,0);
	if (Data == 0)
	{
		OutputMessage("Section::Create - Could not open view.\n");
		return false;
	}

	MEMORY_BASIC_INFORMATION info;
	Allocated = VirtualQuery(Data,&info,sizeof(info));

	#endif 

	return true;
}

bool Section::Close()
{
	#ifdef REASON_PLATFORM_WINDOWS

	if (!UnmapViewOfFile(Data)) 
	{ 
		OutputMessage("Section::Create - Could not close view.\n");
		return false;
	} 

	CloseHandle((void*)Handle); 

	#endif

	Size = 0;
	Data = 0;
	Allocated = 0;

	return true;
}

int Section::Read(char * data, int size, int offset)
{
	return 0;
}

int Section::Write(char * data, int size, int offset)
{
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Pipe::Pipe()
{
}

Pipe::~Pipe()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Mapping::Open(char * data, int size)
{
	if (Reason::System::Storage::Drive::Exists(data,size))
	{

		#ifdef REASON_PLATFORM_WIN32

		#endif

		#ifdef REASON_PLATFORM_POSIX
		#endif

	}
	else
	{

		#ifdef REASON_PLATFORM_WIN32
		#endif

		#ifdef REASON_PLATFORM_POSIX
		#endif

	}

	return false;
}

bool Mapping::Close()
{
	#ifdef REASON_PLATFORM_WIN32
	#endif

	#ifdef REASON_PLATFORM_POSIX
	#endif

	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

