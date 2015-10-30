
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
#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#include "reason/system/path.h"
#include "reason/system/parser.h"
#include "reason/system/sequence.h"
#include "reason/structure/map.h"
#include "reason/structure/list.h"
#include "reason/system/properties.h"
#include "reason/system/text.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason;
using namespace Reason::System;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Options
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Arguments : public Path
{
public:

	Arguments(){}
	Arguments(const char * data){Construct(data);}
	Arguments(char * data, int size){Construct(data,size);}
	Arguments(const Sequence & sequence){Construct(sequence);}
	Arguments(int length, char * array[]){Construct(length,array);}

	bool Construct(const Sequence & sequence) {return Construct(sequence.Data,sequence.Size);}
	bool Construct(const char * data) {return Construct((char*)data,String::Length(data));}
	bool Construct(char * data, int size);
	bool Construct(int length, char * array[]);

	Path * Select(const Sequence & name, bool caseless = false) {return Select(name.Data,name.Size,caseless);}
	Path * Select(const char * name, bool caseless = false) {return Select((char*)name,String::Length(name),caseless);}
	Path * Select(char * name, int size, bool caseless = false);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Section
{
public:

	String Name;
	Reason::System::Properties Properties;

	Section()
	{
	}

	Section(const String & name):Name(name)
	{
	}

	bool Contains(const char * data){return Contains((char*)data,String::Length(data));}
	bool Contains(const Sequence & sequence){return Contains(sequence.Data,sequence.Size);}	
	bool Contains(char * data, int size)
	{
		return Properties.Contains(data,size);
	}

	String & operator [] (const char * data) {return Properties[data];}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Configuration
{
public:

	Structure::List<System::Section*> Sections;

	Configuration();
	~Configuration();

	void Insert(const String & name, const String & value){Insert("",name,value);}
	void Insert(const String & section, const String & name, const String & value)
	{
		for (int i=0;i<Sections.Count;++i)
		{
			if (Sections[i]->Name.Is(section))
			{
				Sections[i]->Properties.Insert(name,value);
				return;
			}
		}

		System::Section * sec = new System::Section(section);
		sec->Properties.Insert(name,value);
		Sections.Append(sec);
	}

	bool Load(const char * data){return Load((char*)data,String::Length(data));}
	bool Load(const Sequence & sequence){return Load(sequence.Data,sequence.Size);}	
	bool Load(char * data, int size);

	bool Contains(const char * data){return Contains((char*)data,String::Length(data));}
	bool Contains(const Sequence & sequence){return Contains(sequence.Data,sequence.Size);}	
	bool Contains(char * data, int size)
	{
		for (int i=0;i<Sections.Count;++i)
		{
			if (Sections[i]->Properties.Contains(data,size))
				return true;
		}

		return false;
	}

	System::Section * Section(const char * data){return Section((char*)data,String::Length(data));}
	System::Section * Section(const Sequence & sequence){return Section(sequence.Data,sequence.Size);}	
	System::Section * Section(char * data, int size)
	{
		for (int i=0;i<Sections.Count;++i)
		{
			if (Sections[i]->Properties.Contains(data,size))
				return Sections[i];
		}

		return 0;
	}

	String * Property(const char * data){return Property((char*)data,String::Length(data));}
	String * Property(const Sequence & sequence){return Property(sequence.Data,sequence.Size);}	
	String * Property(char * data, int size)
	{
		for (int i=0;i<Sections.Count;++i)
		{
			String string(data,size);
			if (Sections[i]->Properties.Contains(data,size))
				return &Sections[i]->Properties[string.Data];
		}

		return 0;
	}
};

class ConfigurationFile : public TextFile
{
public:

	ConfigurationFile(){}
	ConfigurationFile(const File & file):TextFile(file){}
	ConfigurationFile(const Sequence & sequence):TextFile(sequence){}
	ConfigurationFile(const char * path):TextFile(path){}
	ConfigurationFile(char * path, int size):TextFile(path,size){}
	~ConfigurationFile(){}

	void Read(Reason::System::Properties & properties);
	void Write(Reason::System::Properties & properties);
};

class ConfigurationParser : public StringParser
{
public:

	ConfigurationParser(const Sequence & sequence):StringParser(sequence) {}

	void ParseSection(Section & section);
	void ParseProperties(Properties & properties);
};

class Settings
{
public:

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

