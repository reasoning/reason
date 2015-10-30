
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

#include "reason/system/file.h"
#include "reason/system/storage/drive.h"

#include "reason/system/number.h"

using namespace Reason::System;
using namespace Reason::System::Storage;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FileStorage::Construct(const Reason::System::Path & path)
{

}

bool FileStorage::Open(const Reason::System::Path & path, int options)
{
	return Open(path,options&File::MODE,options&File::TYPE,options&File::ACCESS,options&File::USAGE);
}

int FileStorage::ReadLine(const Reason::System::Path & path, String & string)
{

	int amount=0;

	char data;
	int read = Read(path,&data,1);
	while (read && data != '\n' && data != '\r')
	{
		string.Append(&data,1);
		read = Read(path,&data,1);
		++amount;
	}

	return amount;
}

bool FileStorage::OpenReadable(const Reason::System::Path & path)
{
	if (!IsOpen(path) && !Open(path) && !Open(path,File::OPTIONS_OPEN_BINARY_READ) && !Open(path,File::OPTIONS_CREATE_BINARY_READ)) 
	{

		OutputError("FileSystem::OpenReadable - The file could not be opened for reading.\n");	
		return false;
	}

	if (!IsReadable(path))
	{
		OutputError("FileSystem::OpenReadable - The file could not be opened for reading.\n");	
		return false;
	}

	return true;
}

bool FileStorage::OpenWriteable(const Reason::System::Path & path)
{
	if (!IsOpen(path) && !Open(path) && !Open(path,File::OPTIONS_OPEN_BINARY_WRITE) && !Open(path,File::OPTIONS_CREATE_BINARY_WRITE))
	{

		OutputError("FileSystem::OpenWriteable - The file could not be opened for writing.\n");	
		return false;
	}

	if (!IsWriteable(path))
	{
		OutputError("FileSystem::OpenWriteable - The file could not be opened for writing.\n");	
		return false;
	}

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FilePath::FilePath()
{
	Append(Path = new Reason::System::Path(true));
	Append(Name = new Reason::System::Path(true));

	Construct();
}

FilePath::FilePath(const Sequence & sequence):
	Reason::System::Path(sequence)
{
	Append(Path = new Reason::System::Path(true));
	Append(Name = new Reason::System::Path(true));

	Construct();
}

FilePath::FilePath(const char * name):
	Reason::System::Path(name)
{
	Append(Path = new Reason::System::Path(true));
	Append(Name = new Reason::System::Path(true));

	Construct();
}

FilePath::FilePath(char * name, int size):
	Reason::System::Path(name,size)
{
	Append(Path = new Reason::System::Path(true));
	Append(Name = new Reason::System::Path(true));

	Construct();
}

FilePath::~FilePath()
{

}

void FilePath::Normalise()
{

	Translate(":*?\"<>|","");
	Construct();
}

void FilePath::Construct(char * path, int size)
{
	String::Construct(path,size);
	Construct();
}

void FilePath::Construct()
{

	Path->Release();
	Name->Release();

	if (IsEmpty()) return;

	int size = 0;
	do
	{
		size = Size;
		Replace("//","/");
		Replace("\\\\","\\");
	}
	while (size != Size);

	if (Size > 1 && Data[Size-1] == '/' || Data[Size-1] == '\\')
	{
		Size--;
		Terminate();
	}

	TrimRight(" \t\n\r");

	while (!Is("..") && !Is('.') && EndsWith('.')) TrimRight(1);

	int index = Number::Max(LastIndexOf('/'),LastIndexOf('\\'));
	if (index != -1)
	{
		Path->Assign(Slice(0,index));
		Path->Split("/");
		Path->Split("\\");
		Name->Assign(Data+index+1,Size-(index+1));

		if (Path->CharAt(1) == ':')
		{

			Path->Substring::Left(-2);
		}
	}
	else
	{

		Name->Assign(*this);
		if (Name->Size > 1 && Name->CharAt(1) == ':')
		{

			Name->Substring::Left(-2);
		}
	}

}

Reason::System::Substring FilePath::Foldername()
{
	return Reason::System::Substring(Path->Data,Path->Size);
}

Reason::System::Substring FilePath::Basename() 
{
	Substring basename;
	if (Path->Count)
	{
		basename.Assign(Path->Last->Data,Path->Last->Size);
	}
	else
	{
		basename.Assign(Path->Data,Path->Size);
	}
	return basename;
}

Reason::System::Substring FilePath::Extension() 
{
	Substring extension;
	int index = Name->LastIndexOf('.');
	if (index != -1)
	{
		extension.Assign(Name->Data+index+1,Name->Size-index-1);

	}
	return extension;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity File::Instance;

File::File() 
{
	Storage = new Reason::System::Storage::DriveFile();	
}

File::File(FileStorage * driver) 
{
	Storage = driver;
}

File::File(Strong<FileStorage*> & driver) 
{
	Storage = driver;
}

File::File(const File & file)
{
	if (&file == this)
		return;
	operator = (file);
}

File::File(const Sequence & sequence):
	FilePath((Sequence&)sequence)
{
	Storage = new Reason::System::Storage::DriveFile();		
}

File::File(const char * name):
	FilePath(name)
{
	Storage = new Reason::System::Storage::DriveFile();		
}

File::File(char * name, int size):
	FilePath(name,size)
{
	Storage = new Reason::System::Storage::DriveFile();	
}

File::~File() 
{
	if (Storage) Storage->Close(*this);
}

void File::Construct()
{
	if (Storage) Storage->Close(*this);
	FilePath::Construct();
}

void File::Construct(char * path, int size)
{
	if (Storage) Storage->Close(*this);

	FilePath::Construct(path,size);
}

File & File::operator = (const File & file)
{
	if (&file == this)
		return *this;

	Path::Construct(file);
	Storage = file.Storage;

	int offset = 0;

	if (!file.Path->IsEmpty())
	{
		offset = file.Path->Data-file.Data;
		Path->Data = Data+offset;
		Path->Size = file.Path->Size;
	}

	if (!file.Name->IsEmpty())
	{
		offset = file.Name->Data-file.Data;
		Name->Data = Data+offset;
		Name->Size = file.Name->Size;
	}

	return *this;
}

int File::Compare(Reason::System::Object *object, int comparitor)
{
	if (Inheritance::IsHomozygous(this,object) || Inheritance::IsDominant(this,object))
	{
		return Path::Compare(object);
	}
	else
	{
		return Inheritance::Compare(this,object);
	}
}

bool File::Open(int options)
{
	return (Storage)?Storage->Open(*this,options):false;
}

bool File::Open(int mode, int type, int access, int usage)
{
	return (Storage)?Storage->Open(*this,mode,type,access,usage):false;
}	

bool File::OpenReadable()
{
	return (Storage)?Storage->OpenReadable(*this):false;
}

bool File::OpenWriteable()
{	
	return (Storage)?Storage->OpenWriteable(*this):false;
}

bool File::Close()
{
	return (Storage)?Storage->Close(*this):false;
}

bool File::IsReadable()
{
	return (Storage)?Storage->IsReadable(*this):false;
}

bool File::IsWriteable()
{
	return (Storage)?Storage->IsWriteable(*this):false;
}

int File::Read(char * data, int size)
{
	return (Storage)?Storage->Read(*this,data,size):false;
}

int File::ReadLine(String & string)
{
	return (Storage)?Storage->ReadLine(*this,string):false;
}

int File::ReadLine(char * data, int size)
{
	return (Storage)?Storage->ReadLine(*this,data,size):false;
}

int File::Write(char * data, int size)
{
	return (Storage)?Storage->Write(*this,data,size):false;
}

int File::WriteLine(char * data, int size)
{	
	return (Storage)?Storage->WriteLine(*this,data,size):false;
}

bool File::Flush()
{
	return (Storage)?Storage->Flush(*this):false;
}

bool File::Rename(char * name, int size)
{
	return (Storage)?Storage->Rename(*this,name,size):false;
}

bool File::Create(int access)
{
	return (Storage)?Storage->Create(*this,access):false;
}

bool File::Delete()
{
	return (Storage)?Storage->Delete(*this):false;
}

bool File::Truncate()
{
	return (Storage)?Storage->Truncate(*this):false;
}

int File::Seek(int position, int origin)
{
	return (Storage)?Storage->Seek(*this,position,origin):0;
}

bool File::Eof()
{
	return (Storage)?Storage->Eof(*this):false;
}

int File::Error()
{
	return (Storage)?Storage->Error(*this):false;
}

bool File::Status(Reason::System::Stat & stat)
{
	if (Storage)
	{
		stat = Storage->Status(*this);
		return true;
	}

	return false;
}

Stat File::Status()
{
	if (Storage)
	{
		return Storage->Status(*this);
	}

	return Stat();
}

bool File::Exists()
{
	return (Storage)?Storage->Exists(*this):false;	
}

bool File::IsOpen()
{
	return (Storage)?Storage->IsOpen(*this):false;
}

bool File::IsClosed()
{
	return (Storage)?Storage->IsClosed(*this):false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
