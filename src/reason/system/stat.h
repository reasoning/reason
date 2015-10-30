
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
#ifndef SYSTEM_STAT_H
#define SYSTEM_STAT_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/string.h"
#include "reason/system/path.h"

#include <sys/stat.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Stat
{
public: 

	unsigned int Device;		
	unsigned short Inode;		
	unsigned short Mode;		
	short Links;				
	short User;					
	short Group;				
	unsigned int Extended;		
	long Size;					
	time_t Accessed;			
	time_t Modified;			
	time_t Changed;				

	bool Exists;

	enum StatModes
	{
		MODE_FOLDER		= S_IFDIR,
		MODE_FILE		= S_IFREG,
		MODE_READ		= S_IREAD,
		MODE_WRITE		= S_IWRITE,
		MODE_EXEC		= S_IEXEC,
	};

	bool IsFolder()		{return (Mode & MODE_FOLDER) != 0;}
	bool IsFile()		{return (Mode & MODE_FILE) != 0;}
	bool IsReadable()	{return (Mode & MODE_READ) != 0;}
	bool IsWriteable()	{return (Mode & MODE_WRITE) != 0;}
	bool IsExecutable() {return (Mode & MODE_EXEC) != 0;}

	Stat(): 
		Exists(false),Device(0),Inode(0),Mode(0),Links(0),User(0),Group(0),Extended(0),Size(0),Accessed(0),Modified(0),Changed(0)
	{
	}

	Stat(Path & path): 
		Exists(false),Device(0),Inode(0),Mode(0),Links(0),User(0),Group(0),Extended(0),Size(0),Accessed(0),Modified(0),Changed(0)
	{
		Construct(path.Data,path.Size);
	}	

	Stat(Sequence & sequence): 
		Exists(false),Device(0),Inode(0),Mode(0),Links(0),User(0),Group(0),Extended(0),Size(0),Accessed(0),Modified(0),Changed(0)
	{
		Construct(sequence);
	}

	Stat(char * data, int size): 
		Exists(false),Device(0),Inode(0),Mode(0),Links(0),User(0),Group(0),Extended(0),Size(0),Accessed(0),Modified(0),Changed(0)
	{
		Construct(data,size);
	}

	Stat(const char * data): 
		Exists(false),Device(0),Inode(0),Mode(0),Links(0),User(0),Group(0),Extended(0),Size(0),Accessed(0),Modified(0),Changed(0)
	{
		Construct(data);
	}

	Stat(const struct stat & st, bool exists=false):
		Exists(exists),Device(0),Inode(0),Mode(0),Links(0),User(0),Group(0),Extended(0),Size(0),Accessed(0),Modified(0),Changed(0)
	{
		operator = (st);
	}

	Stat & operator = (const struct stat &st);

	void Construct(Stat & stat) {*this = stat;}
	void Construct(Sequence & sequence) {Construct(sequence.Data,sequence.Size);}
	void Construct(const char * data) {Construct((char*)data,String::Length(data));}
	void Construct(char * data, int size);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
