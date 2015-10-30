
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

#include "reason/export.h"

#include "reason/system/stringstream.h"
#include "reason/system/string.h"
#include "reason/system/parser.h"
#include "reason/system/file.h"
#include "reason/system/folder.h"

using namespace Reason::System;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Export {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CodeParser::ParseCode(const String & string, const String & path)
{
	if (!Handler) return false;

	Assign(string);

	Reason::System::Token comment;

	while (!Eof())
	{

		SkipWhitespace();

		if (Is("//"))
		{
			Mark(comment);	
			comment.Offset = Scanner->Token.Offset;
			while (!Eof() && !IsNewline())
			{
				Next();
			}

			Trap(comment);

			if (!comment.StartsWith("////////////////////////////////////////"))
				Handler->HandleComment(comment,path);			
		}
		else
		if (Is("/*"))
		{
			Mark(comment);
			comment.Offset = Scanner->Token.Offset;
			Next();
			while (!Eof(2) && !Is("*/"))
			{
				Next();
			}

			if (!Eof(2) && Skip("*/"))
			{
				Trap(comment);
				Handler->HandleComment(comment,path);			
			}
		}
		else
		if (Is("\""))
		{
			Reason::System::Token literal;
			Mark(literal);
			literal.Offset = Scanner->Token.Offset;

			Next();
			while(!Eof() && !Is("\""))
			{
				if (Is("\\"))
					Skip(2);
				else
					Next();					
			}
			Next();
			Trap(literal);

		}			
		else
		{
			Next();
		}
	}

	return false;	
}

void CodeReader::Write()
{
	Iterand< Mapped<String,Tokens> > files = Files.Forward();
	while (files)
	{
		Reason::System::StringStream code;

		FileStream file(files().Key());

		file.Open(File::OPTIONS_OPEN_BINARY_READ);

		file.Seek(0);	

		files().Value().Sort();

		{

			int offset=0;
			Iterand<Reason::System::Token> tokens = files().Value().Forward();
			while (tokens != 0)
			{
				Reason::System::Token token = tokens();
				if (token.Offset < offset)
				{
					OutputBreak();
				}

				offset = token.Offset;
				++tokens;
			}

		}

		int offset=0;

		Iterand<Reason::System::Token> tokens = files().Value().Forward();
		while (tokens != 0)
		{

			Reason::System::Token token = tokens();
			if (token.Offset-offset < 0)
			{
				OutputBreak();
				++tokens;
				continue;
			}

			if (token.Offset-offset > 0)
				offset += file.Read((Stream&)code,token.Offset-offset);

			if (!Definitions.Select(tokens()))
			{

				offset = file.Seek(tokens().Size,0);
			}

			++tokens;
		}

		file.Read((Stream&)code);

		code.Seek(0);

		Reason::Language::Regex::Regex regex("(\\n\\s*\\n)+");		
		code.Replace(regex,"\n\n");

		file.Close();

		file.Replace("src/reason","exp/reason");
		file.Open(File::OPTIONS_CREATE_BINARY_MUTABLE);
		file.Write((Stream&)code);
		file.Close();

		if (file.EndsWith(".h"))
		{
			file.Replace("exp/reason","include/reason");
			file.Open(File::OPTIONS_CREATE_BINARY_MUTABLE);
			file.Write((Stream&)code);
			file.Close();
		}			

		++files;
	}

}

void CodeReader::Copyright()
{
	String copyright;
	File("copyright.txt").Read(copyright);

	Iterand< Mapped<String,Tokens> > files = Files.Forward();
	while (files)
	{
		StringStream code;
		FileStream file(files().Key());
		file.Replace("src/reason","exp/reason");

		file.Read((Stream&)code);
		file.Close();
		file.Open(File::OPTIONS_OPEN_BINARY_MUTABLE);
		file.Write(copyright);
		code.Seek(0);
		file.Write((Stream&)code);
		file.Close();

		if (file.EndsWith(".h"))
		{
			file.Replace("exp/reason","include/reason");
			file.Open(File::OPTIONS_OPEN_BINARY_MUTABLE);
			file.Write(copyright);
			code.Seek(0);
			file.Write((Stream&)code);
			file.Close();
		}	

		++files;
	}

}

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
