
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
#ifndef SYSTEM_FOLDER_H
#define SYSTEM_FOLDER_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/stat.h"
#include "reason/system/file.h"
#include "reason/structure/enumerator.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FolderStorage;

class FolderPath : public FilePath
{
public:

	FolderPath();
	FolderPath(const Sequence & sequence);
	FolderPath(const char * name);
	FolderPath(char * name, int size);
	~FolderPath();

	void Construct();
	void Construct(char * path, int size);
	void Construct(const Sequence &path) {return Construct(path.Data,path.Size);};
	void Construct(const char * path) {return Construct((char*)path,String::Length(path));};

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Folder : public FolderPath
{
public:

public:

	static Identity Instance;

	virtual Identity& Identify()
	{
		return Instance;
	}

	virtual bool InstanceOf(Identity &identity)
	{
		return Instance == identity || Path::InstanceOf(identity);
	}

public:

	Strong<FolderStorage*> Storage;

	Reason::Structure::Enumerator<File>	Files;
	Reason::Structure::Enumerator<Folder> Folders;

	Folder(const Folder & folder);
	Folder(FolderStorage * driver);
	Folder(Strong<FolderStorage*> & driver);	
	Folder(const Sequence & path);
	Folder(const char *path);
	Folder(char * path, int size);
	Folder();
	~Folder();

	Folder & operator = (const Folder & folder);

	void Construct();
	void Construct(char * path, int size);
	void Construct(const Sequence &path) {return Construct(path.Data,path.Size);};
	void Construct(const char * path) {return Construct((char*)path,String::Length(path));};

	bool CreateFolder(char *name);	
	bool DeleteFolder(char *name);

	bool CreateFile(char *name);
	bool DeleteFile(char *name);

	bool Rename(Sequence & name) {Rename(name.Data,name.Size);};
	bool Rename(const char *name) {Rename((char*)name,String::Length(name));}
	bool Rename(char * name, int size); 

	bool Exists();

	bool Create();
	bool Delete(bool recursive = false);

	bool List(bool recursive = false);

	bool Status(Reason::System::Stat & stat);
	Stat Status();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FolderStorage : public Reason::System::Kind
{
public:

	FolderStorage();
	~FolderStorage();

	virtual void Construct();
	void Construct(const Path & path) {return Construct(path.Data,path.Size);};
	void Construct(const char *path) {return Construct((char*)path,String::Length(path));};
	void Construct(char *path, int size);

	virtual bool CreateFolder(const Path & path, char *name)=0;	
	virtual bool DeleteFolder(const Path & path, char *name)=0;

	virtual bool CreateFile(const Path & path, char *name)=0;
	virtual bool DeleteFile(const Path & path, char *name)=0;

	bool Rename(const Path & path, Sequence & name) {Rename(path,name.Data,name.Size);};
	bool Rename(const Path & path, const char *name) {Rename(path,(char*)name,String::Length(name));}
	virtual bool Rename(const Path & path, char * name, int size)=0;

	virtual bool Exists(const Path & path)=0;
	virtual bool Create(const Path & path)=0;
	virtual bool Delete(const Path & path, List<Reason::System::File> & files, List<Reason::System::Folder> & folders, bool recursive=false)=0;

	virtual bool List(const Path & path, List<Reason::System::File> & files, List<Reason::System::Folder> & folders, bool recursive=false)=0;

	virtual Stat Status(const Path & path)=0;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

