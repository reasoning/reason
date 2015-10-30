
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
#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/string.h"
#include "reason/system/object.h"
#include "reason/system/stream.h"
#include "reason/system/sequence.h"
#include "reason/system/character.h"
#include "reason/system/exception.h"

#include "reason/structure/set.h"

using namespace Reason::Structure;

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

class Handler
{
public:

	const static int TypeBoundary	= (1)<<2;
	const static int TypeReserved	= 2;
	const static int TypeMask		= 3;

	static const char * Descriptions[3];

	enum HandlerTypes
	{
		HANDLER_ERROR		=(0),
		HANDLER_FAILURE		=(1),
		HANDLER_WARNING		=(1)<<1,

	};

	Bitmask32	Type;

	Handler();

	~Handler();

	inline bool IsFailure()	{return Type.Is(HANDLER_FAILURE);};
	inline bool IsError()	{return Type.Is(HANDLER_ERROR);};
	inline bool IsWarning()	{return Type.Is(HANDLER_WARNING);};

	inline const char * Description(unsigned int type){return Descriptions[type&0x03];};

	void Error(const char *message,...);
	void Failure(const char *message,...);
	void Warning(const char *message,...);

	void Throw(const unsigned int type,const char * message,...);

	virtual void Catch(const unsigned int type,const char * message,va_list args);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Token : public String
{
public:

	int Position;	
	int Offset;		

	int Line;		
	int Column;		

	Token(const Token & token):Position(token.Position),Offset(token.Offset),Line(token.Line),Column(token.Column)
	{
		if (token.Allocated)
		{
			Construct(token.Data,token.Size);
		}
		else
		{
			Data = token.Data;
			Size = token.Size;
		}
	}

	Token(char * data, int size):Position(0),Offset(0),Line(0),Column(0)
	{
		Data = data;
		Size = size;
	}

	Token():Position(0),Offset(0),Line(0),Column(0)
	{
	}

	~Token()
	{
	}

	void Release()
	{
		Line = Column = Position = Offset = 0;
		String::Release();
	}

	void Reduce()
	{

		int trim = Offset;
		Left(-trim);
		Position += trim;
		Offset -= trim;
	}

	Token & operator = (Token & token)
	{
		Position = token.Position;
		Offset = token.Offset;

		Line = token.Line;
		Column = token.Column;		

		if (token.Allocated)
		{
			Construct(token.Data,token.Size);
		}
		else
		{
			Data = token.Data;
			Size = token.Size;
		}

		return *this;
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Scanner
{
public:

	class Token Token;

	Scanner(char * data, int size):Token(data,size) {}
	Scanner(const Sequence & sequence):Token(sequence.Data,sequence.Size) {}
	Scanner() {}

	bool operator () () {return At()!=0;}
	bool operator () (int size) {return At(size)!=0;}
	bool operator () (int n, int size) {return At(n,size)!=0;}

	virtual char * At()=0;
	virtual char * At(int size)=0;
	virtual char * At(int n, int size)=0;

	virtual bool Accept() {return Accept(1);}
	virtual bool Accept(int n)=0;

	virtual bool Next() {return Next(1);}
	virtual bool Next(int n)=0;

	virtual bool Abort()=0;

	virtual bool Mark(class Token & token)=0;
	virtual bool Trap(class Token & token, int offset)=0;

	virtual bool Mark(Substring & substring)=0;
	virtual bool Trap(Substring & substring, int offset)=0;

	virtual bool Load(class Token & token)=0;
	virtual bool Store(class Token & token)=0;

	virtual int Position()=0;
	virtual int Column()=0;
	virtual int Line()=0;
};

class StringScanner : public Scanner
{
public:

	StringScanner(const char * data):Scanner((char*)data,String::Length(data))
	{
	}

	StringScanner(char * data, int size):Scanner(data,size)
	{
	}

	StringScanner(const Sequence & sequence):Scanner(sequence.Data,sequence.Size)
	{
	}

	StringScanner()
	{
	}

	void Release()
	{
		Token.Release();
	}

	char * At() {return At(0,1);}
	char * At(int size) {return At(0,size);}
	char * At(int n, int size)
	{
		if (Token.Data == 0) return 0;
		return (Token.Offset+n+size <= Token.Size)?Token.Data+Token.Offset+n:0;
	}

	bool Mark(Substring & substring) 
	{
		substring.Release();
		if (Token.Data == 0 || Token.Offset >= Token.Size) return false;
		substring.Data = Token.Data+Token.Offset; 
		return true;
	}

	bool Trap(Substring & substring, int offset) 
	{
		if (Token.Data == 0 || Token.Offset+offset+1 > Token.Size || substring.Data < Token.Data || substring.Data >= Token.Data+Token.Size) return false;
		substring.Size = Token.Data+Token.Offset+offset+1-substring.Data; return true;
	}

	bool Mark(class Token & token) 
	{
		token.Release();
		if (Token.Data == 0 || Token.Offset >= Token.Size) return false;
		token.Data = Token.Data+Token.Offset; return true;
	}

	bool Trap(class Token & token, int offset) 
	{
		if (Token.Data == 0 || Token.Offset+offset+1 > Token.Size || token.Data < Token.Data || token.Data >= Token.Data+Token.Size) return false;
		token.Size = Token.Data+Token.Offset+offset+1-token.Data; return true;
	}

	bool Load(class Token & token) 
	{
		Token = token; return true;
	}

	bool Store(class Token & token)
	{
		token = Token; return true;
	}

	bool Accept()
	{

		if (Token.Data[Token.Offset] == Character::CarriageReturn || Token.Data[Token.Offset] == Character::LineFeed )
		{
			++Token.Line;
			Token.Column=0;
		}
		++Token.Offset;
		++Token.Position;
		return true;
	}

	bool Accept(int n)
	{
		while (n-- > 0)
		{
			if (Token.Data[Token.Offset] == Character::CarriageReturn || Token.Data[Token.Offset] == Character::LineFeed )
			{
				++Token.Line;
				Token.Column=0;
			}
			++Token.Offset;
			++Token.Position;
		}

		return true;
	}

	bool Next() 
	{
		if (Token.Data == 0 || Token.Offset >= Token.Size) return false;
		++Token.Offset;
		++Token.Position;
		++Token.Column;
		return true;
	}

	bool Next(int n) 
	{
		if (Token.Data == 0 || Token.Offset+n > Token.Size) return false;

		Token.Offset+=n;
		Token.Position+=n;
		Token.Column+=n;
		return true;
	}

	bool Abort() {Token.Offset = Token.Size;return true;}

	int Position() {return Token.Position;}
	int Column() {return Token.Column;}
	int Line() {return Token.Line;}
};

class StreamScanner : public Scanner
{
public:

	class Stream * Stream;
	Reason::Structure::Set<class Token *> Tokens;

	StreamScanner(class Stream & stream):Stream(&stream)
	{
	}

	StreamScanner(class Stream * stream):Stream(stream)
	{
	}

	~StreamScanner()
	{
	}

	char * At() {return At(0,1);}
	char * At(int size) {return At(0,size);}
	char * At(int n, int size);

	bool Mark(class Substring & substring) {substring.Release();return false;}
	bool Trap(class Substring & substring, int offset) {return false;}

	bool Mark(class Token & token);
	bool Trap(class Token & token, int offset);

	bool Load(class Token & token) 
	{
		Token = token;
		return true;
	}

	bool Store(class Token & token) 
	{
		token = Token;
		return true;
	}

	bool Accept();
	bool Accept(int n);

	bool Next() {return Next(1);}
	bool Next(int n);

	bool Abort() {Token.Offset = Token.Size+1;return true;}

	int Position() {return Token.Position+Token.Offset;}
	int Column() {return Token.Column;}
	int Line() {return Token.Line;}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Parser
{
public:

	class Scanner * Scanner;
	class Token	Token;

	Parser(class Scanner * scanner);
	Parser(class Scanner & scanner);
	Parser();
	~Parser();

	void Release();

	bool Empty();

	int Position() {return Scanner->Position();}
	int Column() {return Scanner->Column();}
	int Line() {return Scanner->Line();}

	char * At() {return Scanner->At();}
	char * At(int size)	{return Scanner->At(size);}
	char * At(int n, int size) {return Scanner->At(n,size);}

	bool Abort()			{return Scanner->Abort();}
	bool Next()				{return Scanner->Next();}

	bool Accept()		{return Scanner->Accept();}

	bool Load(class Token & token) {return Scanner->Load(token);}
	bool Store(class Token & token) {return Scanner->Store(token);}

	bool Mark() {return Scanner->Mark(Token);}					
	bool Mark(class Token & token) {return Scanner->Mark(token);}
	bool Mark(Substring & substring) {return Scanner->Mark(substring);}
	bool Trap(int offset = -1) {return Scanner->Trap(Token,offset);}
	bool Trap(class Token & token, int offset = -1) {return Scanner->Trap(token,offset);}
	bool Trap(Substring & substring, int offset = -1) {return Scanner->Trap(substring,offset);}

	bool IsWhitespace(int n)		{return (*Scanner)(n,1) && Character::IsWhitespace(*At(n,1));}
	bool IsAlphanumeric(int n)		{return (*Scanner)(n,1) && Character::IsAlphanumeric(*At(n,1));}
	bool IsAlpha(int n)				{return (*Scanner)(n,1) && Character::IsAlpha(*At(n,1));}
	bool IsHex(int n)				{return (*Scanner)(n,1) && Character::IsHex(*At(n,1));}
	bool IsOctal(int n)				{return (*Scanner)(n,1) && Character::IsOctal(*At(n,1));}
	bool IsDecimal(int n)			{return (*Scanner)(n,1) && Character::IsNumeric(*At(n,1));}
	bool IsNumeric(int n)			{return (*Scanner)(n,1) && Character::IsNumeric(*At(n,1));}
	bool IsDigit(int n)				{return (*Scanner)(n,1) && Character::IsNumeric(*At(n,1));}
	bool IsAscii(int n)				{return (*Scanner)(n,1) && Character::IsAscii(*At(n,1));}
	bool IsControl(int n)			{return (*Scanner)(n,1) && Character::IsControl(*At(n,1));}
	bool IsPrintable(int n)			{return (*Scanner)(n,1) && Character::IsPrintable(*At(n,1));}
	bool IsIdentifier(int n)		{return (*Scanner)(n,1) && Character::IsIdentifier(*At(n,1));}
	bool IsPunctuation(int n)		{return (*Scanner)(n,1) && Character::IsPunctuation(*At(n,1));}
	bool IsSymbol(int n)			{return (*Scanner)(n,1) && Character::IsSymbol(*At(n,1));}
	bool IsDelimiter(int n)			{return (*Scanner)(n,1) && Character::IsDelimiter(*At(n,1));}
	bool IsUppercase(int n)			{return (*Scanner)(n,1) && Character::IsUppercase(*At(n,1));}
	bool IsLowercase(int n)			{return (*Scanner)(n,1) && Character::IsLowercase(*At(n,1));}
	bool IsNewline(int n)			{return (*Scanner)(n,1) && Character::IsNewline(*At(n,1));}

	bool IsWhitespace()		{return IsWhitespace(0);}
	bool IsAlphanumeric()	{return IsAlphanumeric(0);}
	bool IsAlpha()			{return IsAlpha(0);}
	bool IsHex()			{return IsHex(0);}
	bool IsOctal()			{return IsOctal(0);}
	bool IsDecimal()		{return IsNumeric(0);}
	bool IsNumeric()		{return IsNumeric(0);}
	bool IsDigit()			{return IsNumeric(0);}
	bool IsAscii()			{return IsAscii(0);}
	bool IsControl()		{return IsControl(0);}
	bool IsPrintable()		{return IsPrintable(0);}
	bool IsIdentifier()		{return IsIdentifier(0);}
	bool IsPunctuation()	{return IsPunctuation(0);}
	bool IsSymbol()			{return IsSymbol(0);}
	bool IsDelimiter()		{return IsDelimiter(0);}
	bool IsUppercase()		{return IsUppercase(0);}
	bool IsLowercase()		{return IsLowercase(0);}
	bool IsNewline()		{return IsNewline(0);}

	bool Eof()						
	{ 
		return !(*Scanner)(0,1);
	}

	bool Eof(const int n)
	{
		return !(*Scanner)(n,1);
	}

	bool Is(char data) {return (*Scanner)() && Characters::Equals(*At(),data);}
	bool Is(const Sequence & data) {return Is(data.Data,data.Size);}
	bool Is(const char *data) {return Is((char*)data,String::Length(data));}	
	bool Is(char *data, int size)
	{
		return (*Scanner)(size) && Sequences::Equals(At(size),data,size);
	}

	bool Is(const int n, char data) {return Is(n,&data,1);}
	bool Is(const int n, char *data)	{return Is(n,data,String::Length(data));}
	bool Is(const int n, char *data, int size)
	{		
		return (*Scanner)(n,size) && Sequences::EqualsCaseless(At(n,size),data,size);
	}

	bool IsCaseless(char data)	{return (*Scanner)() && Characters::EqualsCaseless(*At(),data);}
	bool IsCaseless(const Sequence & data) {return IsCaseless(data.Data,data.Size);}
	bool IsCaseless(const char *data) {return IsCaseless((char*)data,String::Length(data));}	
	bool IsCaseless(char *data, int size)
	{	
		return (*Scanner)(size) && Sequences::EqualsCaseless(At(size),data,size);
	}

	bool IsCaseless(const int n, char data)	{return (*Scanner)(n,1) && Characters::EqualsCaseless(*At(n,1),data);}
	bool IsCaseless(const int n, const Sequence & data) {return IsCaseless(n,data.Data,data.Size);}
	bool IsCaseless(const int n, const char *data) {return IsCaseless(n,(char*)data,String::Length(data));}	
	bool IsCaseless(const int n, char *data, int size)
	{	
		return (*Scanner)(n,size) && Sequences::EqualsCaseless(At(n,size),data,size);
	}

	bool IsAny(const Sequence & tokens) {return IsAny(tokens.Data,tokens.Size);}
	bool IsAny(const char *tokens) {return IsAny((char*)tokens, Sequence::Length(tokens));}

	bool IsAny(char *tokens, int length)	
	{
		if ((*Scanner)())
		{
			for (int i=0;i<length;++i)
				if (*At() == tokens[i]) return true;
		}
		return false;
	}

	bool IsAny(const int n, const Sequence & tokens) {return IsAny(n,tokens.Data,tokens.Size);}
	bool IsAny(const int n, const char *tokens) {return IsAny(n,(char*)tokens, Sequence::Length(tokens));}
	bool IsAny(const int n, char *tokens, int length)	
	{
		if ((*Scanner)(n,1))
		{
			for (int i=0;i<length;++i)
				if (*At(n,1) == tokens[i]) return true;
		}
		return false;
	}

	bool IsAnyCaseless(const Sequence & tokens) {return IsAnyCaseless(tokens.Data,tokens.Size);}
	bool IsAnyCaseless(const char *tokens) {return IsAnyCaseless((char*)tokens, Sequence::Length(tokens));}
	bool IsAnyCaseless(char *tokens, int length)	
	{
		if ((*Scanner)())
		{
			for (int i=0;i<length;++i)
				if (Characters::EqualsCaseless(*At(),tokens[i])) return true;
		}
		return false;
	}

	bool IsAnyCaseless(const int n, const Sequence & tokens) {return IsAnyCaseless(n,tokens.Data,tokens.Size);}
	bool IsAnyCaseless(const int n, const char *tokens) {return IsAnyCaseless(n,(char*)tokens, Sequence::Length(tokens));}
	bool IsAnyCaseless(const int n, char *tokens, int length)	
	{
		if ((*Scanner)(n,1))
		{
			for (int i=0;i<length;++i)
				if (Characters::EqualsCaseless(*At(n,1),tokens[i])) return true;
		}
		return false;
	}

	bool IsRange(const char lower, const char upper )
	{
		OutputAssert(lower < upper);
		return (*Scanner)() && (*At() >= lower && *At() <= upper);
	}

	bool IsRange(const int n, const char lower, const char upper )
	{
		OutputAssert(lower < upper);
		return (*Scanner)(n,1) && (*At(n,1) >= lower && *At(n,1) <= upper);
	}

	bool SkipAny(const Substring tokens[], int length);
	bool SkipAny(const Sequence & tokens) {return ParseAny(tokens.Data,tokens.Size);}
	bool SkipAny(const char * tokens) {return ParseAny((char*)tokens,String::Length(tokens));}
	bool SkipAny(char * tokens, int length);

	bool Skip(const Sequence & sequence) {return Skip(sequence.Data,sequence.Size);}
	bool Skip(const char *data) {return Skip((char*)data,String::Length(data));}	
	bool Skip(char *data, int size) {return Is(data,size) && Skip(size);}	
	bool Skip(const char data) {return Is(data) && Skip(1);}

	bool Skip(const int n) {return Scanner->Next(n);}
	bool SkipLine();
	bool SkipNewline();
	bool SkipWord();
	bool SkipHex();
	bool SkipOctal();
	bool SkipDecimal();
	bool SkipNumber();
	bool SkipInteger();
	bool SkipReal();
	bool SkipSymbol();
	bool SkipIdentifier();
	bool SkipDelimiter();
	bool SkipPunctuation();
	bool SkipWhitespace();
	bool SkipHyphenation();

	bool SkipUntil(const Sequence & sequence) {return SkipUntil(sequence.Data,sequence.Size);}
	bool SkipUntil(const char *data) {return SkipUntil((char*)data,String::Length(data));}	
	bool SkipUntil(char *data, int size);
	bool SkipUntil(const char data) {return SkipUntil((char*)&data,1);}

	bool SkipUntilAny(const Substring tokens[], int length);
	bool SkipUntilAny(const Sequence & tokens) {return SkipUntilAny(tokens.Data,tokens.Size);}
	bool SkipUntilAny(char token) {return SkipUntilAny((char*)&token,1);}
	bool SkipUntilAny(const char * tokens) {return SkipUntilAny((char*)tokens,String::Length(tokens));}
	bool SkipUntilAny(char * tokens, int length);

	bool ParseAny(const Substring tokens[], int length);
	bool ParseAny(const Sequence & tokens) {return ParseAny(tokens.Data,tokens.Size);}
	bool ParseAny(const char * tokens) {return ParseAny((char*)tokens,String::Length(tokens));}
	bool ParseAny(char * tokens, int length);

	bool ParseUntil(const Sequence & sequence) {return ParseUntil(sequence.Data,sequence.Size);}
	bool ParseUntil(const char *data) {return ParseUntil((char*)data,String::Length(data));}	
	bool ParseUntil(char *data, int size);
	bool ParseUntil(const char data) {return ParseUntil((char*)&data,1);}

	bool ParseUntil(const Callback<bool> & callback);
	bool ParseUntil(const Callback<bool,Parser &> & callback);

	bool ParseUntilAny(const Substring tokens[], int length);
	bool ParseUntilAny(const Sequence & tokens) {return ParseUntilAny(tokens.Data,tokens.Size);}
	bool ParseUntilAny(char token) {return ParseUntilAny((char*)&token,1);}
	bool ParseUntilAny(const char * tokens) {return ParseUntilAny((char*)tokens,String::Length(tokens));}
	bool ParseUntilAny(char * tokens, int length);

	bool Parse(const Sequence & sequence) {return Parse(sequence.Data,sequence.Size);}
	bool Parse(const char *data) {return Parse((char*)data,String::Length(data));}	
	bool Parse(char *data, int size) 
	{
		if (Is(data,size))
		{
			Mark();
			Scanner->Next(size);
			Trap();
			return true;
		}
		return false;
	}	

	bool Parse(const char data) 
	{
		if (Is(data))
		{
			Mark();
			Next();
			Trap();
			return true;
		}
		return false;
	}

	bool Parse(const int n) {return (*Scanner)(n,1) && Mark() && Scanner->Next(n) && Trap();}
	bool ParseLine();
	bool ParseNewline();
	bool ParseWord();

	bool ParseHex();
	bool ParseOctal();
	bool ParseDecimal();
	bool ParseNumber();
	bool ParseInteger();
	bool ParseReal();
	bool ParseSymbol();
	bool ParseIdentifier();
	bool ParseDelimiter();
	bool ParsePunctuation();
	bool ParseWhitespace();
	bool ParseQuotation();

	bool ParseHyphenation();

	bool ParseLiteral();
	bool ParseString() {return ParseQuotation();}

	bool ParseInner(char open, char close, char eof=0, int depth=0);
	bool ParseInner(const Sequence &open, const Sequence &close, const Sequence &eof, int depth=0);
	bool ParseInner(const char *open, const char *close, const char *eof, int depth=0);

	bool ParseOuter(char open, char close, char eof=0, int depth=0);
	bool ParseOuter(const char *open, const char *close, const char *eof, int depth=0);
	bool ParseOuter(const Sequence &open, const Sequence &close, const Sequence &eof, int depth=0);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class StringParser : public Parser
{
public:

	class StringScanner StringScanner;

	StringParser(const char * data):StringScanner(data),Parser(StringScanner)
	{
	}

	StringParser(const Sequence & sequence):StringScanner(sequence),Parser(StringScanner)
	{
	}

	StringParser(char * data, int size):StringScanner(data, size),Parser(StringScanner)
	{
	}

	StringParser():Parser(StringScanner)
	{
	}

	void Assign(char *data, int size) 
	{

		StringScanner.Token.Release();
		StringScanner.Token.Assign(data,size);
	}
	void Assign(const char *data) {Assign((char*)data,String::Length(data));};
	void Assign(const char *start, const char * end) {Assign((char*)start,(end-start+1));};
	void Assign(const Sequence & sequence) {Assign(sequence.Data,sequence.Size);};
};

class StreamParser : public Parser
{
public:
	class StreamScanner StreamScanner;

	StreamParser(Stream & stream):StreamScanner(stream),Parser(StreamScanner)
	{
	}

	StreamParser(Stream * stream):StreamScanner(stream),Parser(StreamScanner)
	{
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

