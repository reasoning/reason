
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

#ifndef SYSTEM_TOKENIZER_H
#define SYSTEM_TOKENIZER_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/stream.h"
#include "reason/system/string.h"
#include "reason/system/path.h"
#include "reason/structure/objects/objects.h"
#include "reason/structure/enumerator.h"

using namespace Reason::System;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Tokenizer : public Substring
{
public:

	Tokenizer();
	Tokenizer(Sequence & sequence);

	Reason::Structure::Enumerator<Substring>	Numbers;
	Reason::Structure::Enumerator<Substring>	Words;
	Reason::Structure::Enumerator<Substring>	Punctuation;

	void Tokenise(Sequence &sequence){Assign(sequence);Tokenise();};
	virtual void Tokenise();

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TokenizerStream : public Reason::Structure::Objects::Enumerator
{
public:

	class Stream & Stream;
	Path Separators;

	String Token;
	Substring Separator;

	int Index(){return Enumeration.Index;};

	TokenizerStream(class Stream & stream);
	~TokenizerStream();

	String * operator()(void)		{return &Token;};
	String * Pointer()				{return &Token;};
	String & Reference()			{return Token;};

	bool Has();

	bool Move(int amount);
	bool Move();

	bool Forward();
	bool Reverse();

private:

	struct StreamEnumeration
	{
		int Direction;		
		int Index;
		String Next;
		String Prev;

		StreamEnumeration():Direction(0),Index(0)
		{

		}
	};

	String Buffer;
	StreamEnumeration Enumeration;

	Substring Next();
	Substring Prev();

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
