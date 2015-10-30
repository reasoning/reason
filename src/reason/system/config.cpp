
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


#include "reason/system/config.h"
#include "reason/system/parser.h"
#include "reason/system/stream.h"
#include "reason/system/string.h"

#include "reason/system/text.h"

#include "reason/structure/map.h"
#include "reason/system/tokenizer.h"

using namespace Reason::Structure;
using namespace Reason::System;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Arguments::Construct(int length, char * array[])
{
	String arguments;
	for (int i=0;i<length;++i)
		arguments << array[i] << " ";

	return Construct(arguments);
}

bool Arguments::Construct(char * data, int size)
{
	String::Construct(data,size);

	StringParser parser;
	parser.Assign(*this);
	bool error = false;
	while ( !error && !parser.Eof() )
	{
		if (parser.Is('-'))
		{
			parser.Next();
			if (parser.Is('-'))
				parser.Next();

			Path * path = new Path();
			Append(path);

			if (parser.ParseWord())
			{
				if (parser.Eof() || parser.SkipWhitespace())
				{	
					Path * name = new Path();
					Path * value = new Path();
					path->Append(name);
					path->Append(value);
					name->Assign(parser.Token);

					if (!parser.Is('-'))
					{
						parser.Mark();
						while (!parser.Eof() && !parser.IsWhitespace()) parser.Next();
						parser.Trap();

						if (!parser.Token.IsEmpty())
							value->Assign(parser.Token);
					}
				}
				else
				{
					error = true;
				}
			}
			else
			{
				error = true;
			}
		}
		else
		{
			parser.Mark();
			while (!parser.Eof() && !parser.IsWhitespace())
				parser.Next();
			parser.Trap();

			if (!parser.Token.IsEmpty())
			{
				Path * path = new Path();
				path->Assign(parser.Token);
				Append(path);
			}
		}

		if ( !error && !parser.Eof() && !parser.SkipWhitespace() && !parser.Is('-'))
			error = true;
	}

	if (error)
	{
		Release(false);
		OutputError("Arguments::Construct - Invalid token in arguments at column %d.",parser.Column());
		return false;
	}
	else
	{
		return true;
	}
}

Path * Arguments::Select(char * name, int size, bool caseless)
{
	Path::Enumerator iterator(*this);
	for (iterator.Forward();iterator.Has();iterator.Move())
	{
		if (iterator.Reference().Count == 2)
		{

			if (iterator.Reference()[0].Is(name,size,caseless))
			{
				return &iterator.Reference()[1];
			}
		}
		else
		{

			if (iterator.Reference().Is(name,size,caseless))
				return &iterator.Reference();
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Configuration::Configuration()
{

}

Configuration::~Configuration()
{
	Sections.Destroy();
}

bool Configuration::Load(char * data, int size)
{
	TextFileStream textFileStream(data,size);
	TokenizerStream streamIterator(textFileStream);
	streamIterator.Separators.Append(new Path("\r"));
	streamIterator.Separators.Append(new Path("\n"));

	System::Section * section = 0;
	Iterand< Reason::Structure::Mapped<String,String> > iterand;

	bool continued=false;
	for (streamIterator.Forward();streamIterator.Has();streamIterator.Move())
	{

		StringParser parser;
		parser.Assign(*streamIterator());
		parser.ParseWhitespace();

		if (continued)
		{
			parser.Mark();
			while (!parser.Eof() && !parser.Is("\\")) parser.Next();
			parser.Trap();
			continued = (parser.Is("\\"))?true:false;
			iterand().Value().Append(parser.Token);
		}
		else
		if (parser.Is("#") || parser.Is("!") || parser.Is(";") || parser.Is("//"))
		{

		}
		else
		if (parser.Is("["))
		{
			parser.Mark();
			while (!parser.Eof() && !parser.Is("]"))
				parser.Next();
			parser.Trap();

			if (parser.Is("]"))
			{
				if (section) Sections.Append(section);
				section = new System::Section();

				section->Name = parser.Token;
				parser.Next();
			}
			else
			{
				OutputError("Properties::Load","Missing \"]\" after section name, line %d column %d\n",parser.Line(),parser.Column());
			}

		}
		else
		if (!parser.Eof())
		{
			parser.Mark();
			while (!parser.Eof() && !parser.IsWhitespace() && !parser.Is("="))
				parser.Next();
			parser.Trap();

			Substring name(parser.Token);

			if (parser.Is("="))
			{
				parser.Next();
				parser.Mark();
				while (!parser.Eof() && !parser.Is("\\")) parser.Next();
				parser.Trap();
				continued = (parser.Is("\\"))?true:false;

				Substring value(parser.Token);

				if (!section)
				{
					section = new System::Section();
				}

				iterand = section->Properties.Insert(name,value);
			}
			else
			{
				OutputError("Configuration::Load","Missing \"=\" after attribute name, line %d column %d\n",parser.Line(),parser.Column());
			}
		}
	}

	if (section)
		Sections.Append(section);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConfigurationParser::ParseSection(Section & section)
{

	while(!Eof())
	{
		SkipWhitespace();

		if (Is("["))
		{
			Mark();
			while (!Eof() && !Is("]"))
				Next();
			Trap();

			if (Is("]"))
			{
				section.Name = Token;
				Next();
				ParseProperties(section.Properties);
			}
			else
			{
				OutputError("ConfigurationParser::Load","ParseSection \"]\" after section name, line %d column %d\n",Line(),Column());
			}
		}
	}

}

void ConfigurationParser::ParseProperties(Properties & properties)
{

	bool continued=false;

	while (!Eof() && !Is("["))
	{
		ParseWhitespace();

		if (continued)
		{
			Mark();
			while (!Eof() && !Is("\\")) Next();
			Trap();
			continued = (Is("\\"))?true:false;

		}
		else
		if (Is("#") || Is("!") || Is(";") || Is("//"))
		{

			SkipLine();
		}
		else
		if (!Eof())
		{
			Mark();
			while (!Eof() && !IsWhitespace() && !Is("="))
				Next();
			Trap();
			SkipWhitespace();

			Substring name(Token);

			if (Is("="))
			{
				Next();
				SkipWhitespace();

				Mark();
				while (!Eof() && !Is("\\") && !Is(Characters::NewLine) && !Is(Characters::CarriageReturn)) Next();
				Trap();
				continued = (Is("\\"))?true:false;

				Substring value(Token);

				if ((value.StartsWith("'") && value.EndsWith("'")) || (value.StartsWith('"') && value.EndsWith('"')))
				{

					value.Trim(1);
				}

				properties.Insert(name,value);
			}
			else
			{
				OutputError("ConfigurationParser::ParseProperties","Missing \"=\" after attribute name, line %d column %d\n",Line(),Column());
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ConfigurationFile::Read(Reason::System::Properties & properties)
{

	String string;
	File::Read(string);

	ConfigurationParser configurationParser(string);

	while (!configurationParser.Eof())
		configurationParser.ParseProperties(properties);
}

void ConfigurationFile::Write(Reason::System::Properties & properties)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

