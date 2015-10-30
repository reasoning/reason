
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
#ifndef REASON_EXPORT_H
#define REASON_EXPORT_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/string.h"
#include "reason/platform/thread.h"
#include "reason/messaging/callback.h"
#include "reason/system/parser.h"
#include "reason/system/file.h"
#include "reason/system/folder.h"
#include "reason/language/regex/regex.h"
#include "reason/structure/map.h"
#include "reason/structure/set.h"
#include "reason/structure/array.h"
#include "reason/structure/policy.h"

using namespace Reason::Messaging;
using namespace Reason::Platform;
using namespace Reason::System;
using namespace Reason::Language::Regex;
using namespace Reason::Structure;
using namespace Reason::Structure::Policy;

namespace Reason { namespace Export {

class CodeHandler
{
public:

	virtual void HandleDefinition(const Reason::System::Token & token, const String & path, const String & name) {};
	virtual void HandleReference(const Reason::System::Token & token, const String & path, const String & name) {};
	virtual void HandleComment(const Reason::System::Token & token, const String & path) {};
	virtual void HandleWhitespace(const Reason::System::Token & token, const String & path) {};

};

class TokenCompare : public Compare<Token>
{
public:

	TokenCompare(int option):Compare<Token>(option) {}

	int operator () (const Reason::System::Token & left, const Reason::System::Token & right)
	{

		return left.Offset-right.Offset;

	}
};

class CodeParser : public StringParser
{
public:

	CodeHandler * Handler;

	CodeParser(CodeHandler * handler):Handler(handler)
	{

	}

	bool ParseCode(const String & string, const String & path);

};

class CodeReader : public CodeParser, public CodeHandler
{
public:

	Map<String, int> References;
	Map<Reason::System::Token, String> Definitions;

	typedef List<Reason::System::Token,Ascending<Reason::System::Token,TokenCompare> > Tokens;

	Map<String, Tokens > Files;

	CodeReader():CodeParser(this)
	{

	}

	void HandleToken(const Reason::System::Token & token, const String & path)
	{

		Iterand< Mapped<String,Tokens> > iterand = Files.Update(path);

		int offset=0;

		Iterand<Reason::System::Token> tokens = iterand().Value().Forward();
		while (tokens != 0)
		{

			Reason::System::Token token = tokens();
			if (token.Offset-offset < 0)
			{
				OutputBreak();
			}

			++tokens;
		}

		iterand().Value().Insert(token);	
	}

	void HandleDefinition(const Reason::System::Token & token, const String & path, const String & name)
	{
		HandleToken(token,path);

		if (!Definitions.Insert(token,name))
		{

		}
	}

	void HandleComment(const Reason::System::Token & token, const String & path)
	{	
		HandleToken(token,path);
	}	

	void HandleReference(const Reason::System::Token & token, const String & path, const String & name)
	{
		HandleToken(token,path);
	}

	void HandleWhitespace(const Reason::System::Token & token, const String & path)
	{
		HandleToken(token,path);
	}

	void Read()
	{		
		ReadFolder("src/");
	}

	void ReadFolder(const String & path)
	{
		Folder folder(path);
		folder.List();

		Iterand<Folder> folders = folder.Folders.Forward();
		while (folders != 0)
		{
			if (!folders().EndsWith(".svn"))
			{
				ReadFolder(folders());
			}

			++folders;
		}

		Iterand<File> files = folder.Files.Forward();
		while(files != 0)
		{
			if ((!files().Name->StartsWith(".") && !files().Name->StartsWith("_")) &&
				(files().EndsWith(".cpp") || files().EndsWith(".h")))
			{
				ReadFile(files());
			}

			++files;
		}
	}

	void ReadFile(const String & path)
	{
		Iterand< Mapped<String,Tokens> > iterand = Files.Update(path);

		String code;

		File file(path);
		file.Read(code);
		file.Close();

		CodeParser::ParseCode(code,path);
	}

	void Write();
	void Copyright();

};

}} 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif