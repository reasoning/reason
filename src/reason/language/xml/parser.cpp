
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

#include "reason/language/xml/xml.h"
#include "reason/language/xml/parser.h"
#include "reason/language/xml/dtd.h"
#include "reason/language/xml/dtdreader.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;
using namespace Reason::Language::Xml;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool XmlNamespaceParser::IsName(Sequence &string)
{
	char *s = string.PointerAt(0);
	char *e = string.PointerAt(string.Size-1);

	if (isalpha(*s) || *s=='_' || *s== ':')
	{
		++s;
		while (s != e)
		{
			if ( ! (isalnum(*e) || *e=='.' || *e=='-' || *e=='_' || *e==':') )
				return false;

			++s;
		}

		return true;
	}

	return false;
}

bool XmlNamespaceParser::IsNameToken(Sequence &string)
{
	if (string.IsEmpty())
		return false;

	char *s = string.PointerAt(0);
	char *e = string.PointerAt(string.Size-1);

	while (s != e)
	{
		if ( ! IsNameChar(*s))
			return false;
		++s;
	}

	return true;
}

bool XmlNamespaceParser::ParseName()
{

	if (!IsAlpha() && !Is('_') &&!Is(':'))	
		return false;

	Mark();		
	Next();

	while( IsNameChar())
	{
		Next();
	}

	Trap();
	return true;
}

bool XmlNamespaceParser::ParseNcname()
{

	if (Is('-') || IsAlpha())
	{
		Mark();
		Next();

		while(IsNcnameChar())	
		{
			Next();
		}

		Trap();

		return true;
	}

	return false;
}

bool XmlNamespaceParser::ParseQualifiedName(Substring &prefix)
{

	if (ParseNcname())
	{
		if (Is(':') && At(1,1) && IsNcnameChar(*At(1,1)))
		{
			prefix.Assign(Token);
			Next();

			if (ParseNcname())
			{
				return true;
			}
			else
			{
				((XmlNamespaceHandler*)Handled)->Error("XmlNamespaceParser::ParseQualifiedName - Invalid qualified name, line %d column %d",Line(),Column());
			}
		}

		return true;
	}

	return false;
}

bool XmlNamespaceParser::ParseQualifiedName()
{

	if (ParseNcname())
	{
		if (Is(':') && At(1,1) && IsNcnameChar(*At(1,1)))
		{
			Next();

			if (ParseNcname())
			{
				return true;
			}
			else
			{
				((XmlNamespaceHandler*)Handled)->Error("XmlNamespaceParser::ParseQualifiedName - Invalid qualified name, line %d column %d",Line(),Column());
			}
		}

		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlPrologueParser::XmlPrologueParser()
{

}

XmlPrologueParser::~XmlPrologueParser()
{

}

bool XmlPrologueParser::ParseVersionInformation()
{

	if (Is("version"))
	{
		Skip(7);

		SkipWhitespace();

		if (Is(Character::Equal))
		{	
			Next();
			SkipWhitespace();

			char sentinel=0;
			if (Is(Character::SingleQuote) || Is(Character::DoubleQuote))
			{
				sentinel = *At();
				Next();
				Mark();

				if (IsAlphanumeric() || IsAny("_.:-"))
				{
					Next();
					while (!Is(sentinel) && (IsAlphanumeric() || IsAny("_.:-")))
					{
						Next();
						if (Eof())
						{
							((XmlPrologueHandler*)Handled)->Failure("XmlPrologueParser::ParseVersionInformation - Premature end of document, line %d column %d",Line(),Column());
							return false;
						}
					}
				}
				else
				{
					((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseVersionInformation - Invalid character in version information, line %d column %d",Line(),Column());
					return false;
				}

				Trap();
				Next(); 
			}
		}
		else
		{
			((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseVersionInformation - Missing version information, line %d column %d",Line(),Column());
			return false;
		}

		return true;
	}

	return false;
}

bool XmlPrologueParser::ParseEncodingDeclaration()
{

	if (Is("encoding"))
	{
		Skip(8);
		SkipWhitespace();

		if (!Is(Character::Equal))
		{
			((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseEncodingDeclaration - Missing \"=\", line %d column %d",Line(),Column());
			return false;
		}

		Next();	

		SkipWhitespace();

		char sentinel;
		if (Is(Character::SingleQuote) || Is(Character::DoubleQuote))
		{
			sentinel = *At();

			Next();
			Mark();

			if (IsAlpha())
			{
				Next();
				while ( !Is(sentinel) && (IsAlphanumeric() || IsAny("_.:-")) )
				{
					Next();
					if (Eof())
					{
						((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseEncodingDeclaration - Invalid encoding name, line %d column %d",Line(),Column());
						return false;
					}
				}
			}
			else
			{
				((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseEncodingDeclaration - Encoding name must start with an alpha character, line %d column %d",Line(),Column());
				return false;
			}

			Trap();
			Next();	
		}
		else
		{
			((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseEncodingDeclaration - Expected string literal containing encoding information, line %d column %d",Line(),Column());
			return false;
		}

		return true;
	}

	return false;
}

bool XmlPrologueParser::ParseCdataSection()
{

	if (Is("<![" ))
	{
		Skip(3);
		Mark();

		if (!ParseWord() || !Token.Is("CDATA") || !Is('['))
		{
			((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseCdata - Expected \"CDATA[\", line %d column %d",Line(),Column());
			return false;
		}

		Next();	

		XmlCdata *cdata = new XmlCdata();
		cdata->Token.Assign(Token);

		Mark();

		while (true)
		{
			if (Eof())
			{
				((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseCdata - Premature end of document, line %d column %d",Line(),Column());
				delete cdata;
				return false;
			}

			if (Is("]]>"))
			{
				Skip(3);
				break;
			}

			Accept();
		}

		Trap(-4);
		cdata->Token.Assign(Token);
		((XmlPrologueHandler*)Handled)->ProcessCdataSection(cdata);
		return true;
	}

	return false;
}

bool XmlPrologueParser::ParseComment()
{
	if (Is("<!-"))
	{
		Next();
		Skip(2);

		if (Is('-'))
		{
			Next();

			if (Is('-'))
			{

				if (Recovery)
				{
					((XmlPrologueHandler*)Handled)->Warning("XmlPrologueParser::ParseComment - (Recovery) Comment cannot begin with more than two \"-\" characters, line %d column %d",Line(),Column());
				}
				else
				{
					((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseComment - Comment cannot begin with more than 2 \"-\" characters, line %d column %d",Line(),Column());
					return false;
				}
			}

			Mark();

			int dash=0;
			while ( !Eof() )
			{
				if ( Is('-') )
				{
					Next();
					++dash;
				}
				else
				if (dash == 2 && Is(Character::GreaterThan))
				{
					break;
				}
				else
				if (Recovery && dash > 2 && Is(Character::GreaterThan))
				{

					((XmlPrologueHandler*)Handled)->Warning("XmlPrologueParser::ParseComment - (Recovery) Comment cannot end with more than two \"-\" characters, line %d column %d",Line(),Column(),Handler::HANDLER_WARNING);
					break;
				}
				else
				{

					Accept();
					dash=0;
				}
			}

			if (Eof())
			{
				((XmlPrologueHandler*)Handled)->Failure("XmlPrologueParser::ParseComment - Premature end of document, line %d column %d",Line(),Column());
				return false;
			}

			Trap(-3);

			((XmlPrologueHandler*)Handled)->ProcessComment(Token);

			Next();	
			return true;

		}
		else
		{
			((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseComment - Expected \"-\", comment must begin with \"<!--\", line %d column %d",Line(),Column());
			return false;
		}
	}

	return false;
}

bool XmlPrologueParser::ParseReference()
{

	if (Is('&'))
	{
		Next();
		Mark();

		if (Is('#'))
		{

			Next();

			if (Is('x'))
			{

				if (IsAlphanumeric())
				{
					Next();

					while(IsAlphanumeric()) 
						Next();

					if (Is(';'))
					{
						Trap();
						Next();

						((XmlPrologueHandler*)Handled)->ProcessReference(Token);
						return true;
					}
				}

				if (!Recovery)
					((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseReference - Expected \";\" at end of character entity reference, line %d column %d",Line(),Column());

				return false;
			}
			else
			{

				if (IsNumeric())
				{
					Next();
					while(IsNumeric())
						Next();

					if (Is(';'))
					{
						Trap();
						Next();

						((XmlPrologueHandler*)Handled)->ProcessReference(Token);
						return true;
					}
				}

			}
		}
		else
		{

			if (!ParseName())
			{

				if (!Recovery)
					((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseReference - Expected name for entity reference, line %d column %d",Line(),Column());

				return false;
			}

			if (Is(';'))
			{
				Next();

				((XmlPrologueHandler*)Handled)->ProcessReference(Token);
				return true;
			}
			else
			{

				if (!Recovery)
					((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseReference - Expected \";\" at end of entity reference, line %d column %d",Line(),Column());

				return false;
			}
		}
	}

	return false;
}

bool XmlPrologueParser::ParseExternalId(XmlExternalId &externalId)
{
	if (Is("SYSTEM"))
	{
		SkipWord();

		if (!SkipWhitespace())
		{
			((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseExternalId - Expected whitespace in external id, line %d column %d",Line(),Column());
			return false;
		}

		if (!ParseSystemLiteral())
		{
			((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseEntityDeclaration - Expected SystemLiteral in external id, line %d column %d",Line(),Column());
			return false;
		}

		externalId.SystemLiteral.Assign(Token);
		externalId.Type = XmlExternalId::SYSTEM;

		return true;

	}
	else
	if (Is("PUBLIC"))
	{
		SkipWord();

		if (!SkipWhitespace())
		{
			((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseExternalId - Expected whitespace in external id, line %d column %d",Line(),Column());
			return false;
		}

		if (!ParsePublicIdLiteral())
		{
			((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseExternalId - Expected PubidLiteral in external id, line %d column %d",Line(),Column());
			return false;
		}

		externalId.PublicIdLiteral.Assign(Token);
		externalId.Type = XmlExternalId::PUBLIC;

		if (SkipWhitespace() && ParseSystemLiteral())
		{
			externalId.SystemLiteral.Assign(Token);
		}
		else
		{

			return false;
		}

		return true;
	}

	return false;
}

bool XmlPrologueParser::ParsePublicIdLiteral()
{

	if (Is(Character::SingleQuote) || Is(Character::DoubleQuote))
	{
		char sentinel = *At();
		Next();
		Mark();
		while( *At() != sentinel )
		{
			if (Eof())
			{
				((XmlPrologueHandler*)Handled)->Failure("XmlPrologueParser::ParseProcessingInstruction - Premature end of document, line %d column %d",Line(),Column());
				return false;
			}
			else
			if ( Is(Character::Space) || Is(Character::CarriageReturn) || Is(Character::LineFeed) || IsAlphanumeric() || IsAny("-'()+,./:=?;!*#@$_%"))
			{

				Next();
			}
			else
			{
				((XmlPrologueHandler*)Handled)->Failure("XmlPrologueParser::ParseProcessingInstruction - Invalid public id literal, line %d column %d",Line(),Column());
				return false;
			}
		}

		Trap();
		Next();	

		return true;

	}
	return false;
}

bool XmlPrologueParser::ParseSystemLiteral()
{

	return ParseString();
}

bool XmlPrologueParser::ParseProcessingInstruction()
{

	if (Is('<') && Is(1,'?') && !IsCaseless(2,"xml"))
	{

		Skip(2);

		if (!ParseName())
		{
			((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseProcessingInstruction - Expected processing instruction name, line %d column %d",Line(),Column());
			return false;
		}

		if (Token.Is("xml",true))
		{
			((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseProcessingInstruction - The name \"xml\" is reserved and cannot be used in processing instructions, line %d column %d",Line(),Column());
			return false;
		}

		if (!SkipWhitespace())
		{
			((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseProcessingInstruction - Expected whitespace.",Line(),Column());
			return false;
		}

		XmlProcessingInstruction * instruction = new XmlProcessingInstruction();
		instruction->Token.Assign(Token);

		Mark();
		while ( ! (Is('?') && Is(1,'>')) )
		{	
			if (Eof() || ParseNewline())
			{
				((XmlPrologueHandler*)Handled)->Error("XmlPrologueParser::ParseProcessingInstruction - Premature end of processing instruction or invalid whitespace parsed, line %d column %d",Line(),Column());
				delete instruction;
				return false;
			}

			Next();		
		}
		Trap();

		instruction->Token.Assign(Token);

		Skip(2);

		((XmlPrologueHandler*)Handled)->ProcessProcessingInstruction(instruction);
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlParser::XmlParser()
{

	Recovery = false;
}

XmlParser::~XmlParser()
{

}

bool XmlParser::Parse(Sequence & sequence)
{

	if (Handled == 0)
	{

		return false;
	}

	if (sequence.IsEmpty())
	{
		((XmlHandler*)Handled)->Failure("XmlParser::Parse - Document source code is empty, line %d column %d",Line(),Column());
		return false;
	}

	Assign(sequence);

	SkipWhitespace();
	ParseProlog();

	while( !Eof() )
	{

		ParseMarkup();

		if (Eof()) break;

		if (Recovery)
		{
			((XmlHandler*)Handled)->Warning("XmlParser::Parse - (Recovery) Invalid markup parsed, line %d column %d",Line(),Column());

			if (ParseProlog())
			{
				((XmlHandler*)Handled)->Warning("XmlParser::Parse - (Recovery) Located out of place prolog, line %d column %d",Line(),Column());
			}
			else
			{

				while( !Eof() && !Is(Character::GreaterThan) )
				{

					Accept();
				}

				if ( Eof() )
				{

					((XmlHandler*)Handled)->Failure("XmlParser::Parse - Premature end of document, line %d column %d",Line(),Column());
					return false;
				}
				else
				{

					Next();
				}
			}
		}
		else
		{
			((XmlHandler*)Handled)->Failure("XmlParser::Parse - Invalid markup parsed, parser will exit, line %d column %d",Line(),Column());
			return false;
		}

	}

	return true;
}

bool XmlParser::ParseText()
{

	if ( !Eof() && !Is(Character::LessThan) )
	{
		Mark();
		Accept();

		while ( !(Eof() || Is(Character::LessThan)) ) 
		{
			Accept();	
		}

		Trap();

		Token.Trim();

		bool null;
		do
		{
			null = false;

			if (Token.Data[0] == 0)
			{
				Token.Data++;
				Token.Size--;
				null = true;
			}
			else
			if (Token.Data[Token.Size-1] == 0)
			{
				Token.Size--;
				null = true;
			}

		} 
		while (null && Token.Size > 0);

		if (!Token.IsEmpty())
		{
			((XmlHandler*)Handled)->ProcessText(Token);
			return true;
		}

	}

	return false;
}

bool XmlParser::ParseAttribute(XmlElement *element)
{

	Substring prefix;
	if (!ParseQualifiedName(prefix))
	{
		((XmlHandler*)Handled)->Error("XmlParser::ParseAttribute - Expected qualified attribute name, line %d column %d",Line(),Column());
		return false;
	}

	XmlAttribute *attribute = new XmlAttribute(element);
	attribute->Name.Assign(Token);

	if ( !Is(Character::Equal) )
	{
		if (Recovery)
		{
			SkipWhitespace();

			if ( Is(Character::Equal) )
			{

				((XmlHandler*)Handled)->Warning("XmlParser::ParseAttribute - (Recovery) Attribute \"%s\" has whitespace before \"=\", line %d column %d",Token.Print(),Line(),Column());
			}
			else
			if ( IsNcnameChar() || Is(Character::GreaterThan) || Is("/>") )
			{

				((XmlHandler*)Handled)->Warning("XmlParser::ParseAttribute - (Recovery) Attribute \"%s\" has no value, minimisation is assumed, line %d column %d",Token.Print(),Line(),Column());

				attribute->Type = XmlAttribute::ATTRIBUTE_MINIMISED;
				((XmlHandler*)Handled)->ProcessAttribute(element,attribute,prefix);	
				return true;
			}
			else
			{
				((XmlHandler*)Handled)->Error("XmlParser::ParseAttribute - Expected attribute value, line %d column %d",Line(),Column());
				delete attribute;
				return false;
			}
		}
	}

	Next();	

	if (Recovery && SkipWhitespace())
	{
		((XmlHandler*)Handled)->Warning("XmlParser::ParseAttribute - Attribute \"%s\" has whitespace after \"=\", line %d column %d",Token.Print(),Line(),Column());
	}

	char sentinel = 0;
	if ( Is(Character::DoubleQuote) || Is(Character::SingleQuote) )
	{	
		sentinel = *At();

		Next();

		Substring value;
		Mark(value);

		while ( !Is(sentinel))
		{

			if (Eof() || Is('<'))
			{
				((XmlHandler*)Handled)->Error("XmlParser::ParseAttribute - Premature end of attribute value, line %d column %d",Line(),Column());
				delete attribute;
				return false;
			}
			else
			if (Is('&'))
			{		
				if (Recovery && (IsWhitespace(1) || Is(sentinel)))
				{

					Accept();
				}
				else
				if (ParseReference())
				{

					((XmlHandler*)Handled)->ProcessAttributeValueReference(Token);
				}
				else
				{
					if (Recovery)
					{

						Accept();
					}
					else
					{
						((XmlHandler*)Handled)->Error("XmlParser::ParseAttribute - Invalid entity reference, line %d column %d",Line(),Column());
						delete attribute;
						return false;	
					}
				}
			}
			else
			{
				Accept();
			}
		}

		Trap(value);
		attribute->Value.Assign(value);
		((XmlHandler*)Handled)->ProcessAttribute(element,attribute,prefix);

		Next(); 
		return true;
	}
	else
	if ( Recovery && ( IsAlphanumeric() || Is('#') || Is(Character::ForwardSlash) || Is('-') || Is('_') ))
	{

		Mark();

		do
		{	
			Accept();

			if (Is("/>"))
			{
				if (Recovery && attribute->Name.Is("href",true))
				{
					Next();
				}

				Trap();
				attribute->Value.Assign(Token);
				((XmlHandler*)Handled)->ProcessAttribute(element,attribute,prefix);
				return true;			
			}
			else
			if (IsWhitespace() || Is(Character::GreaterThan) )
			{
				Trap();
				attribute->Value.Assign(Token);
				((XmlHandler*)Handled)->ProcessAttribute(element,attribute,prefix);
				return true;
			}

		}while ( !Eof());
	}

	delete attribute;
	return false;
}

bool XmlParser::ParseMarkup()
{

	while ((	
			ParseWhitespace()	||
			ParseText()			||
			ParseElement()		||
			ParseComment()		||
			ParseReference()	||
			ParseCdataSection()	||
			ParseProcessingInstruction()) && !Eof())
	{

	};

	return true;
}

bool XmlParser::ParseProlog()
{
	if (!ParseXmlDeclaration())
	{
		if (Recovery)
		{
			((XmlHandler*)Handled)->Warning("XmlParser::ParseProlog - Expected XML declaration - recovery will be attempted, line %d column %d",Line(),Column());
		}
		else
		{

			((XmlHandler*)Handled)->Throw(XmlHandler::XML_EXCEPTION_VALIDITY,"XmlParser::ParseProlog - Expected XML declaration, line %d column %d",Line(),Column());
			return false;
		}
	}

	bool prolog=false;

	while(ParseComment() || ParseProcessingInstruction() || ParseWhitespace()) {prolog=true;};

	if (ParseDoctypeDeclaration()) prolog=true;

	while(ParseComment() || ParseProcessingInstruction() || ParseWhitespace()) {prolog=true;};

	return prolog;
}

bool XmlParser::ParseElement()
{

	if (Is("</"))
	{
		Skip(2);

		Substring prefix;
		if (!ParseQualifiedName(prefix))
		{
			((XmlHandler*)Handled)->Error("XmlParser::ParseElementEnd - Missing qualified name, line %d column %d",Line(),Column());
			return false;
		}

		XmlElement *element = new XmlElement(XmlObject::XML_END,Token);

		SkipWhitespace();

		if ( !Is(Character::GreaterThan))
		{
			((XmlHandler*)Handled)->Error("XmlParser::ParseElementEnd - Missing \">\", line %d column %d",Line(),Column());
			delete element;
			return false; 
		}

		Next();	
		((XmlHandler*)Handled)->ProcessElementEnd(element,prefix);
		return true;
	}
	else
	if (Is('<') && At(1,1) && IsNcnameChar(*At(1,1)) )
	{

		Next();

		Substring prefix;
		if (!ParseQualifiedName(prefix))
		{
			((XmlHandler*)Handled)->Error("XmlParser::ParseElement - Expected qualified name, line %d column %d",Line(),Column());
			return false;
		}

		XmlElement *element = new XmlElement(XmlObject::XML_START,Token);

		if (Is(Character::GreaterThan))
		{
			Next();
		}
		else
		if (Is("/>"))
		{
			element->Type = XmlObject::XML_EMPTY;
			Skip(2);
		}
		else
		if (ParseWhitespace())
		{
			while (true)
			{

				if (Is(Character::GreaterThan))
				{
					Next();
					break;
				}
				else
				if (Is("/>"))
				{
					element->Type = XmlObject::XML_EMPTY;
					Skip(2);
					break;
				}
				else
				if ( !ParseAttribute(element))
				{
					if (Recovery)
					{
						((XmlHandler*)Handled)->Warning("XmlParser::ParseElement - Invalid attribute - recovery will be attempted, line %d column %d",Line(),Column());

						while ( ! (Eof() || IsWhitespace() || Is(Character::GreaterThan) || Is("/>")) )
						{

							Accept();
						}

						if ( Eof() )
						{
							((XmlHandler*)Handled)->Error("XmlParser::ParseElement - Premature end of document, line %d column %d",Line(),Column());
							delete element;
							return false;
						}
					}
					else
					{
						((XmlHandler*)Handled)->Error("XmlParser::ParseElement - Invalid or missing attribute, line %d column %d",Line(),Column());
						delete element;
						return false;
					}
				}

				ParseWhitespace();
			}
		}
		else
		{

			((XmlHandler*)Handled)->Error("XmlParser::ParseElement - Expected whitespace or close of element, line %d column %d",Line(),Column());
			delete element;
			return false;
		}

		bool handled=false;
		if (element->Type.Is(XmlObject::XML_START))
		{
			handled = ((XmlHandler*)Handled)->ProcessElementStart(element,prefix);
		}
		else

		{
			handled = ((XmlHandler*)Handled)->ProcessElementEmpty(element,prefix);
		}

		if (handled)
		{

			if (element->Token.Is("script",true))
			{	
				Mark();
				while (! (Eof() || IsCaseless("</script>")) )
				{

					{
						Accept();
					}
				}

				Trap();

				Token.Trim();
				if (Token.StartsWith("<!--"))
					Token.Left(-4);

				if (Token.EndsWith("-->"))
					Token.Right(-3);

				if (!Token.IsEmpty())
					((XmlHandler*)Handled)->ProcessComment(Token);
			}
			else
			if (element->Token.Is("noscript",true))
			{	
				Mark();
				while (! (Eof() || IsCaseless("</noscript>")) )
				{
					if (Is("<!--"))
					{
						while (! (Eof() || Is("-->") ))
							Accept();
					}
					else
					{
						Accept();
					}
				}
				Trap();

				Token.Trim();
				if (Token.StartsWith("<!--"))
					Token.Left(-4);

				if (Token.EndsWith("-->"))
					Token.Right(-3);

				if (!Token.IsEmpty())
					((XmlHandler*)Handled)->ProcessComment(Token);
			}
			else
			if (element->Token.Is("style",true))
			{	
				Mark();
				while (! (Eof() || IsCaseless("</style>")) )
				{
					if (Is("<!--"))
					{
						while (! (Eof() || Is("-->") ))
							Accept();
					}
					else
					{
						Accept();
					}
				}
				Trap();

				Token.Trim();
				if (Token.StartsWith("<!--"))
					Token.Left(-4);

				if (Token.EndsWith("-->"))
					Token.Right(-3);

				if (!Token.IsEmpty())
					((XmlHandler*)Handled)->ProcessComment(Token);
			}				
		}

		return true;
	}

	return false;
}

bool XmlParser::ParseDoctypeDeclaration()
{

	if (Is("<!DOCTYPE"))
	{
		Skip(2);
		Mark();
		Skip(7);
		Trap();

		XmlDoctypeDeclaration *doctype = new XmlDoctypeDeclaration();
		doctype->Token.Assign(Token);

		Substring prefix;
		if (! (SkipWhitespace() && ParseQualifiedName(prefix)))
		{
			((XmlHandler*)Handled)->Error("XmlParser::ParseDoctypeDeclaration - Expected qualified name, line %d column %d",Line(),Column());
			delete doctype;
			return false;
		}

		doctype->Name.Assign(Token);

		SkipWhitespace();

		if ( !Is('[') && !Is(Character::GreaterThan))	
		{

			ParseExternalId(doctype->ExternalId);

		}

		SkipWhitespace();

		if ( Is('[') )
		{

			Next();

			Mark();
			while ( !Is(']') )
			{
				if (Eof())
				{
					delete doctype;
					return false;
				}

				Next();
			}
			Trap();

			doctype->Dtd.Create(Token);
		}

		SkipWhitespace();

		if ( !Is(Character::GreaterThan) )
		{
			delete doctype;
			return false;
		}

		Next();	
		((XmlHandler*)Handled)->ProcessDoctypeDeclaration(doctype,prefix);
		return true;
	}

	return false;
}

bool XmlParser::ParseXmlDeclaration()
{

	if (Is("<?xml"))
	{
		Skip(2);
		Mark();
		Skip(3);
		Trap();

		XmlDeclaration *xmlDeclaration = new XmlDeclaration();
		xmlDeclaration->Token.Assign(Token);

		if ( !SkipWhitespace() || !ParseVersionInformation() )
		{
			((XmlHandler*)Handled)->Error("XmlParser::ParseXmlDeclaration - Expected xml version information, line %d column %d",Line(),Column());
			delete xmlDeclaration;
			return false;
		}

		xmlDeclaration->Version.Assign(Token);

		SkipWhitespace();

		if (ParseEncodingDeclaration())
		{
			xmlDeclaration->Encoding.Assign(Token);

			SkipWhitespace();
		}

		if (ParseWord() && Token.Is("standalone"))
		{
			if (ParseString() && (Token.Is("yes") || Token.Is("no")))
			{
				xmlDeclaration->Standalone.Assign(Token);
			}
			else
			{
				((XmlHandler*)Handled)->Error("XmlParser::ParseXmlDeclaration - The standalone document declaration must have a value of \"yes\" or \"no\", line %d column %d",Line(),Column());
				delete xmlDeclaration;
				return false;
			}
		}

		SkipWhitespace();

		if (Is("?>"))
		{
			Skip(2);
			((XmlHandler*)Handled)->ProcessXmlDeclaration(xmlDeclaration);
			return true;
		}
		else
		{
			((XmlHandler*)Handled)->Error("XmlParser::ParseXmlDeclaration - Missing \"?>\", line %d column %d",Line(),Column());
			delete xmlDeclaration;
			return false;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
