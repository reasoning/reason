
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
#ifndef SYSTEM_FILESYSTEM_H
#define SYSTEM_FILESYSTEM_H

#include "reason/platform/platform.h"
#include "reason/system/string.h"
#include "reason/system/stream.h"
#include "reason/system/primitive.h"
#include "reason/system/path.h"

#include "reason/system/object.h"
#include "reason/structure/stack.h"
#include "reason/structure/list.h"

#include "reason/system/storage/storage.h"
#include "reason/system/file.h"
#include "reason/system/folder.h"

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;
using namespace Reason::Structure;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Reason { namespace System { namespace Storage {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Drive : public Path
{
public:

	enum FileExtensionTypes
	{

		 TEXT					=0x20,	
		 TEXT_PLAIN				=0x21,	
		 TEXT_HTML				=0x22,
		 TEXT_XML				=0x23,
		 TEXT_SCRIPT			=0x24,	
		 TEXT_CSS				=0x25,

		 BINARY					=0x80,

		 BINARY_EXEC			=0xA0,
		 BINARY_EXEC_EXE		=0xA1,
		 BINARY_EXEC_SCRIPT		=0xA2,
		 BINARY_EXEC_BAT		=0xA3,
		 BINARY_EXEC_COM		=0xA4,
		 BINARY_EXEC_DLL		=0xA5,

		 BINARY_ARCH			=0xC0,
		 BINARY_ARCH_ZIP		=0xC1,
		 BINARY_ARCH_ARJ		=0xC2,
		 BINARY_ARCH_RAR		=0xC3,
		 BINARY_ARCH_GZ			=0xC4,
		 BINARY_ARCH_TAR		=0xC5,

		 BINARY_MEDIA			=0xE0,

		 BINARY_MEDIA_WAV		=0xE1,
		 BINARY_MEDIA_MP3		=0xE2,
		 BINARY_MEDIA_RM		=0xE3,

		 BINARY_MEDIA_GIF		=0xE4,
		 BINARY_MEDIA_JPG		=0xE5,
		 BINARY_MEDIA_PNG		=0xE6,
		 BINARY_MEDIA_BMP		=0xE7,
		 BINARY_MEDIA_TIF		=0xE8,
		 BINARY_MEDIA_PSD		=0xE9,
	};

	struct FileExtensionMap
	{
		char		* Name;
		unsigned char Type;
	};

	static const int FileExtensionCount = 27;
	static const FileExtensionMap FileExtension[FileExtensionCount];

	void ResolveContentType();
    void ResolveMimeType();

public:

	enum 
	{
		DRIVE_DISC,
		DRIVE_TAPE,
		DRIVE_SSD,
		DRIVE_OPTICAL,
	};

	Drive(const Drive & filesystem);
	Drive(const Sequence & sequence);
	Drive(const char * path);
	Drive(char * path, int size);
	Drive();
	~Drive();

	Drive & operator = (const Drive & filesystem);

	void Normalise();

	static bool Status(const Path & path, class Stat & stat); 
	static Stat Status(const Path & path) {return Stat(path.Data,path.Size);}

	Stat Status() {return Reason::System::Stat(*this);}

	bool IsFolder()		{return (Status().Mode & Reason::System::Stat::MODE_FOLDER) != 0;}
	bool IsFile()		{return (Status().Mode & Reason::System::Stat::MODE_FILE) != 0;}
	bool IsReadable()	{return (Status().Mode & Reason::System::Stat::MODE_READ) != 0;}
	bool IsWriteable()	{return (Status().Mode & Reason::System::Stat::MODE_WRITE) != 0;}
	bool IsExecutable() {return (Status().Mode & Reason::System::Stat::MODE_EXEC) != 0;}

	void Release();

	bool Current() {return Drive::Current(*this);}	
	static bool Current(const Path & path); 

	static bool Exists(const Sequence & sequence) {return Exists(sequence.Data,sequence.Size);}
	static bool Exists(const char * data) {return Exists((char*)data, String::Length(data));}
	static bool Exists(char * data, int size) {return Stat(data,size).Exists;}
	bool Exists() {return Stat(Data,Size).Exists;}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DriveFile : public Reason::System::FileStorage
{
public:

	static Identity	Instance;

	virtual Identity & Identify() 
	{
		return Instance;
	} 

public:

	FILE * Handle;
	int Options;

	DriveFile();
	~DriveFile();

	void Construct(const Path & path);

	void Construct(const Path & path, char * data, int size);		

	virtual bool Open(const Path & path, int options = File::OPTIONS_OPEN_BINARY_MUTABLE);
	virtual bool Open(const Path & path, int mode, int type, int access, int usage);

	virtual bool Close(const Path & path);

	bool IsReadable(const Path & path) {return (Stat(path.Data,path.Size).Mode & Reason::System::Stat::MODE_READ) != 0;}
	bool IsWriteable(const Path & path) {return (Stat(path.Data,path.Size).Mode & Reason::System::Stat::MODE_WRITE) != 0;}

	virtual int Read(const Path & path, char * data, int size);	

	virtual int ReadLine(const Path & path, String & string);
	virtual int ReadLine(const Path & path, char * data, int size);

	virtual int Write(const Path & path, char * data, int size);
	virtual int WriteLine(const Path & path, char * data, int size);

	virtual bool Flush(const Path & path);

	virtual bool Create(const Path & path, int access = Reason::System::File::ACCESS_MUTABLE);
	virtual bool Delete(const Path & path);
	virtual bool Truncate(const Path & path);

	virtual bool Rename(const Path & path, char * name, int size);

	virtual int Seek(const Path & path, int position, int origin=-1);	

	virtual bool Eof(const Path & path) {return Handle && feof(Handle);};

	virtual bool Exists(const Path & path) {return Drive::Exists(path);}

	int Error(const Path & path) {return ferror(Handle);};

	bool IsOpen(const Path & path) {return Handle != 0;}
	bool IsClosed(const Path & path) {return Handle == 0;}

	virtual Stat Status(const Path & path);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DriveFolder : public Reason::System::FolderStorage
{
public:

	static Identity	Instance;

	virtual Identity & Identify() 
	{
		return Instance;
	} 

public:

	DriveFolder();
	~DriveFolder();

	bool CreateFolder(const Path & path, char *name);	
	bool DeleteFolder(const Path & path, char *name);

	bool CreateFile(const Path & path, char *name);
	bool DeleteFile(const Path & path, char *name);

	bool Rename(const Path & path, char * name, int size);

	bool Exists(const Path & path);
	bool Create(const Path & path);
	bool Delete(const Path & path,  Reason::Structure::List<Reason::System::File> & files, Reason::Structure::List<Reason::System::Folder> & folders, bool recursive=false);

	bool List(const Path & path, Reason::Structure::List<Reason::System::File> & files, Reason::Structure::List<Reason::System::Folder> & folders, bool recursive=false);

	Stat Status(const Path & path);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif