
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
#ifndef SYSTEM_CHARACTER_H
#define SYSTEM_CHARACTER_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/bit.h"
#include "reason/system/object.h"

#include <ctype.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Characters
{
public:

	static const int Null			= 0x00; 	
	static const int Backspace		= 0x08;		
	static const int Tab			= 0x09;		
	static const int LineFeed		= 0x0A;		
	static const int NewLine		= LineFeed;
	static const int VerticalTab	= 0x0B;		
	static const int FormFeed		= 0x0C;		
	static const int CarriageReturn = 0x0D;		
	static const int Eof			= 0x1A;		
	static const int Escape			= 0x1B;		
	static const int Space			= 0x20;		
	static const int DoubleQuote	= 0x22;		
	static const int SingleQuote	= 0x27; 	
	static const int ForwardSlash	= 0x2F;		
	static const int LessThan		= 0x3C;		
	static const int Equal			= 0x3D;		
	static const int GreaterThan	= 0x3E;		
	static const int BackwardSlash	= 0x5C;		
	static const int Delete			= 0x7F;		

	static const int LeftParenthesis	= '(';
	static const int LeftBracket		= '[';
	static const int LeftBrace			= '{';

	static const int RightParenthesis	= ')';
	static const int RightBracket		= ']';
	static const int RightBrace			= '}';

public:

	static char Caseless(char character)
	{
		return ( !(character < 'A') && !(character > 'Z') )?(character|0x20):(character);
	}

	static bool IsNewline(char character)
	{

		return character == CarriageReturn || character == LineFeed;
	}

	static bool	IsWhitespace(char character)
	{
		return character == Space || character == Tab || character == CarriageReturn || character == LineFeed;
	}

	static bool	IsAlphanumeric(char character)
	{

		return ((Caseless(character) > 96) && (Caseless(character) < 123)) || ((character>47) && (character<58));
	}

	static bool	IsAlpha(char character)
	{

		return (Caseless(character) > 96) && (Caseless(character) < 123);
	}

	static bool IsNumeric(char character)
	{

		return (character>47) && (character<58);
	}

	static bool IsDecimal(char character)
	{

		return IsNumeric(character);
	}

	static bool IsOctal(char character)
	{
		return character >= '0' && character <= '7';
	}

	static bool IsHex(char character)
	{

		return IsNumeric(character) || (Caseless(character) > 96 && Caseless(character) < 103);
	}

	static bool IsAscii(char character)
	{

		return isascii(character) != 0;
	}

	static bool IsControl(char character)
	{

		return character < 32 || character == 127;
	}

	static bool IsPrintable(char character)
	{

		return character > 31 && character < 127;
	}

	static bool IsPunctuation(char character)
	{

		return character > 0 && ispunct(character) != 0;
	}

	static bool IsSymbol(char character)
	{

		return !IsAlphanumeric(character);
	}

	static bool IsDelimiter(char character)
	{

		return character == Tab || character == ',';
	}

	static bool IsIdentifier(char character)
	{

		return character == '_' || IsAlphanumeric(character);
	}

	static bool IsUppercase(char character)
	{
		return ( !(character < 'A') && !(character > 'Z') );
	}

	static bool IsLowercase(char character)
	{
		return ( !(character < 'a') && !(character > 'z') );
	}

	static char Lowercase(char character)
	{
		return ( !(character < 'A') && !(character > 'Z') )?(character|0x20):(character);
	}

	static char Uppercase(char character)
	{
		return ( !(character < 'A') && !(character > 'Z') )?(character&~0x20):(character);
	}

	static int Compare(const char character, const char compare, bool caseless)
	{
		return (caseless)?Lowercase(character)-Lowercase(compare):character-compare;
	}

	static int Compare(const char character, const char compare)
	{
		return character-compare;
	}

	static int CompareCaseless(const char character, const char compare)
	{
		return Caseless(character)-Caseless(compare);
	}

	static bool Equals(const char character, const char compare, bool caseless)
	{
		return (caseless)?Lowercase(character)==Lowercase(compare):character==compare;
	}

	static bool Equals(const char character, const char compare)
	{
		return character==compare;
	}

	static bool EqualsCaseless(const char character, const char compare)
	{
		return Caseless(character)==Caseless(compare);
	}
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

class Symbol : public Reason::System::Object
{
public:

	unsigned short * Data;

	inline void Next()	{++Data;};
	inline void Move()	{--Data;};

	operator unsigned short & (void)					{return *Data;};
	Symbol & operator = (unsigned short *pointer)		{Data = pointer;return *this;};
};

class Character : public Characters
{
public:
	enum CharacterLanguage
	{
		LANGUAGE_ENGLISH,
		LANGUAGE_FRENCH,
		LANGUAGE_RUSSIAN,
	};

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

