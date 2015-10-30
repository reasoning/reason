
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

#include "reason/system/number.h" 
#include "reason/system/folder.h"
#include "reason/system/storage/drive.h"

using namespace Reason::System;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FolderPath::FolderPath()
{
	Construct();
}

FolderPath::FolderPath(const Sequence & sequence):
	FilePath(sequence)
{
	Construct();
}

FolderPath::FolderPath(const char * name):
	FilePath(name)
{
	Construct();
}

FolderPath::FolderPath(char * name, int size):
	FilePath(name,size)
{
	Construct();
}

FolderPath::~FolderPath()
{

}

void FolderPath::Construct(char * path, int size)
{
	String::Construct(path,size);
	Construct();
}

void FolderPath::Construct()
{

	#ifdef REASON_PLATFORM_WINDOWS

	if (! IsEmpty() && CharAt(1) == ':' )
	{
		if ( ! isalpha(CharAt(0)) )
		{

			OutputError("Folder::Construct - Invalid drive letter specified \"%c\"\n", Path[0].CharAt(0));
		}
	}

	#endif

	FilePath::Construct();

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity Folder::Instance;

Folder::Folder(const Folder & folder):
		Storage(0)
{
	if (&folder == this)
		return;

	operator = (folder);
}

Folder::Folder(FolderStorage * driver):
		Storage(0)
{
	Storage = new Reason::System::Storage::DriveFolder();
	Storage = driver;
}

Folder::Folder(Strong<FolderStorage*> & driver):
		Storage(0)
{
	Storage = new Reason::System::Storage::DriveFolder();
	Storage = driver;
}

Folder::Folder(const Sequence & sequence):
	FolderPath(sequence),Storage(0)
{
	Storage = new Reason::System::Storage::DriveFolder();
	Construct();
}

Folder::Folder(const char * path):
	FolderPath(path),Storage(0)
{
	Storage = new Reason::System::Storage::DriveFolder();
	Construct();
}

Folder::Folder(char * path, int size):
	FolderPath(path,size),Storage(0)
{
	Storage = new Reason::System::Storage::DriveFolder();
	Construct();
}

Folder::Folder():
		Storage(0)
{
	Storage = new Reason::System::Storage::DriveFolder();
}

Folder::~Folder()
{
	Files.Destroy();
	Folders.Destroy();
}

void Folder::Construct()
{
	Folders.Destroy();
	Files.Destroy();

	FolderPath::Construct();
}

void Folder::Construct(char * path, int size)
{
	Folders.Destroy();
	Files.Destroy();

	FolderPath::Construct(path,size);
}

Folder & Folder::operator = (const Folder & folder)
{
	if (&folder == this)
		return *this;

	Path::Construct(folder);
	Storage = folder.Storage;

	int offset = 0;
	if (!folder.Path->IsEmpty())
	{
		offset = folder.Path->Data-folder.Data;
		Path->Data = Data+offset;
		Path->Size = folder.Path->Size;
	}

	if (!folder.Name->IsEmpty())
	{
		offset = folder.Name->Data-folder.Data;
		Name->Data = Data+offset;
		Name->Size = folder.Name->Size;
	}

	return *this;
}

bool Folder::CreateFolder(char * name)
{
	return (Storage)?Storage->CreateFolder(*this,name):false;
}

bool Folder::DeleteFolder(char * name)
{
	return (Storage)?Storage->DeleteFolder(*this,name):false;
}

bool Folder::CreateFile(char * name)
{
	return (Storage)?Storage->CreateFile(*this,name):false;
}

bool Folder::DeleteFile(char * name)
{
	return (Storage)?Storage->DeleteFile(*this,name):false;
}

bool Folder::Rename(char * name, int size)
{
	return (Storage)?Storage->Rename(*this,name,size):false;
}

bool Folder::Exists()
{
	return (Storage)?Storage->Exists(*this):false;
}

bool Folder::Create()
{
	return (Storage)?Storage->Create(*this):false;
}

bool Folder::Delete(bool recursive)
{
	return (Storage)?Storage->Delete(*this,Files,Folders,recursive):false;
}

bool Folder::List(bool recursive)
{
	return (Storage)?Storage->List(*this,Files,Folders,recursive):false;
}

bool Folder::Status(Reason::System::Stat & stat)
{
	if (Storage)
	{
		stat = Storage->Status(*this);
		return true;
	}

	return false;
}

Stat Folder::Status()
{
	if (Storage)
	{
		return Storage->Status(*this);
	}

	return Stat();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FolderStorage::FolderStorage()
{
}

FolderStorage::~FolderStorage()
{
}

void FolderStorage::Construct()
{

}

void FolderStorage::Construct(char * path, int size)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

