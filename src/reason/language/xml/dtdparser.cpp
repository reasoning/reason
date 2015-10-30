
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

#include "reason/language/xml/dtdparser.h"
#include "reason/system/sequence.h"
#include "reason/system/string.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;
using namespace Reason::Language::Xml;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlDtdParser::XmlDtdParser()
{

	External = true;	
	ParameterEntityState=0;
}

XmlDtdParser::~XmlDtdParser()
{

	if (ParameterEntityState)
		delete ParameterEntityState;
}

bool XmlDtdParser::Parse(Sequence &sourceCode)
{

	if (sourceCode.IsEmpty())
	{
		((XmlDtdHandler*)Handled)->Failure("Parse","DTD source code is empty, line %d column %d",Line(),Column());
		return false;
	}

	Assign(sourceCode);

	if (Handled == 0)
	{

		return false;
	}

	SkipWhitespace();

	if( !Eof() )
	{
		if (External)
		{
			ParseExternalSubset();
		}
		else
		{
			ParseInternalSubset();
		}

	}

	return true;
}

bool XmlDtdParser::ParseMarkupDeclaration()
{

	if (Is("<!"))
	{

		if ( ! (ParseElementDeclaration() || 
				ParseAttributeListDeclaration() || 
				ParseEntityDeclaration() || 
				ParseNotationDeclaration() ||
				ParseProcessingInstruction() ||
				ParseComment() ))
		{

			if (Recovery)
			{
				((XmlDtdHandler*)Handled)->Warning("ParseMarkupDeclaration","Invalid markup parsed - recovery will be attempted, line %d column %d",Line(),Column());

				while( !Eof() && !Is(Character::GreaterThan) )
				{

					Accept();
				}

				Next();	

				if ( Eof() )
				{

					((XmlDtdHandler*)Handled)->Failure("ParseMarkupDeclaration","Premature end of document, line %d column %d",Line(),Column());
					return false;
				}
			}
			else
			{
				((XmlDtdHandler*)Handled)->Failure("ParseMarkupDeclaration","Invalid markup parsed, line %d column %d",Line(),Column());
				return false;
			}
		}

		return true;
	}

	return false;
}

bool XmlDtdParser::ParseDeclarationSeparator()
{

	if (ParseParameterEntityReference() || SkipWhitespace())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool XmlDtdParser::ParseInternalSubset()
{

	while (!Eof())
	{
		if ( ! (ParseMarkupDeclaration() || ParseDeclarationSeparator()) )
		{
			return false;
		}
	}

	return true;
}

bool XmlDtdParser::ParseExternalSubset()
{

	ParseTextDeclaration();

	return ParseExternalSubsetDeclaration();
}

bool XmlDtdParser::ParseExternalSubsetDeclaration()
{

	while (!Eof())
	{
		if ( ! (ParseConditionalSection() || ParseMarkupDeclaration() ||  ParseDeclarationSeparator()) )
		{
			return false;
		}
	}

	return true;
}

bool XmlDtdParser::ParseConditionalSection()
{

	if (Is("<!["))
	{
		Skip(3);

		SkipWhitespace();

		EnterParameterEntityReference();

		if (Is("INCLUDE"))
		{
			Mark();
			Skip(8);
			Trap();

			LeaveParameterEntityReference();

			SkipWhitespace();

			if (Is('['))
			{
				Next();

				XmlIncludeSection *includeSection = new XmlIncludeSection();
				includeSection->Token.Assign(Token);

				Substring content;
				Mark(content);

				ParseExternalSubsetDeclaration();

				if (Is("]]>"))
				{
					Trap(content);

					includeSection->Token.Assign(content);

					((XmlDtdHandler*)Handled)->ProcessIncludeSection(includeSection);

					Skip(4);
					return true;
				}
				else
				{
					((XmlDtdHandler*)Handled)->Error("ParseConditionalSection","Expected \"]]>\" at end of INCLUDE section, line %d column %d",Line(),Column());
					delete includeSection;
				}

			}
			else
			{
				((XmlDtdHandler*)Handled)->Error("ParseConditionalSection","Expected external subset declaration in INCLUDE section, line %d column %d",Line(),Column());
			}
		}
		else
		if (Is("IGNORE"))
		{
			Mark();
			Skip(7);
			Trap();

			LeaveParameterEntityReference();

			SkipWhitespace();

			if (Is('['))
			{
				Next();

				XmlIgnoreSection *ignoreSection = new XmlIgnoreSection();
				ignoreSection->Token.Assign(Token);

				char *openingString = "<![";
				char *closingString = "]]>";
				int openingStringSize = strlen(openingString);
				int closingStringSize = strlen(closingString);

				Mark();

				int setCount=1;

				do 
				{
					if (Is(openingString))
					{
						++setCount;
						Skip(openingStringSize);
					}
					else
					if (Is(closingString))
					{
						--setCount;
						Skip(closingStringSize);
					}
					else
					{

						Accept();	
					}

					if (Eof())
					{
						((XmlDtdHandler*)Handled)->Error("ParseConditionalSection","Unexpected end of file in INCLUDE section, line %d column %d",Line(),Column());
						delete ignoreSection;		
						return false;
					}
				}
				while (setCount > 0);

				Trap(-3);	

				ignoreSection->Token.Assign(Token);				

				((XmlDtdHandler*)Handled)->ProcessIgnoreSection(ignoreSection);
				return true;
			}

			((XmlDtdHandler*)Handled)->Error("ParseConditionalSection","Expected external subset declaration in INCLUDE section, line %d column %d",Line(),Column());
		}
		else
		{
			((XmlDtdHandler*)Handled)->Error("ParseConditionalSection","Expected INCLUDE or IGNORE, line %d column %d",Line(),Column());
		}

	}

	return false;

}

bool XmlDtdParser::ParseTextDeclaration()
{

	if (Is("<?xml"))
	{
		Skip(2);
		Mark();
		Skip(3);
		Trap();

		if (!SkipWhitespace())
		{
			((XmlDtdHandler*)Handled)->Error("ParseTextDeclaration","Expected whitespace, line %d column %d",Line(),Column());	
			return false;
		}

		XmlTextDeclaration *textDeclaration = new XmlTextDeclaration();
		textDeclaration->Token.Assign(Token);

		if (ParseVersionInformation())
		{
			if (!SkipWhitespace())
			{
				((XmlDtdHandler*)Handled)->Error("ParseTextDeclaration","Expected whitespace, line %d column %d",Line(),Column());
				delete textDeclaration;
				return false;
			}			

		;
			textDeclaration->Version.Assign(Token);
		}

		if ( !ParseEncodingDeclaration() )
		{
			((XmlDtdHandler*)Handled)->Error("ParseTextDeclaration","Expected encoding declaration, line %d column %d",Line(),Column());	
			delete textDeclaration;
			return false;
		}

		textDeclaration->Encoding.Assign(Token);

		SkipWhitespace();

		if (Is('?') && Is(1,'>'))
		{
			Skip(2);
			((XmlDtdHandler*)Handled)->ProcessTextDeclaration(textDeclaration);
			return true;
		}
		else
		{
			((XmlDtdHandler*)Handled)->Error("ParseTextDeclaration","Missing \"?>\", line %d column %d",Line(),Column());
			delete textDeclaration;
			return false;
		}

	}

	return false;
}

bool XmlDtdParser::ParseEntityDeclaration()
{
	if (Is("<!ENTITY"))
	{
		Skip(2);
		Mark();
		Skip(6);
		Trap();

		XmlEntityDeclaration *entity = new XmlEntityDeclaration();
		entity->Token.Assign(Token);

		if ( !SkipWhitespace() )
		{
			((XmlDtdHandler*)Handled)->Error("ParseEntityDeclaration","Expected whitespace, line %d column %d",Line(),Column());
			delete entity;
			return false;
		}

		if (Is('%'))
		{
			Next();

			if (!SkipWhitespace())
			{
				((XmlDtdHandler*)Handled)->Error("ParseEntityDeclaration","Expected whitespace, line %d column %d",Line(),Column());
				delete entity;
				return false;
			}

			entity->EntityType = XmlEntityDeclaration::ENTITY_PARAMETER;
		}
		else
		{
			entity->EntityType = XmlEntityDeclaration::ENTITY_GENERAL;
		}

		if (ParseName() && SkipWhitespace())
		{
			entity->Name.Assign(Token);

			if (ParseExternalId(entity->ExternalId))
			{		
				if (entity->EntityType.Is(XmlEntityDeclaration::ENTITY_GENERAL))
				{

					if (SkipWhitespace() && ParseWord())
					{
						if (! (Token.Is("NDATA") && SkipWhitespace() && ParseName()) )
						{
							((XmlDtdHandler*)Handled)->Error("ParseEntityDeclaration","Expected \"NDATA\" declaration, line %d column %d",Line(),Column());
							delete entity;
							return false;					
						}

						entity->NotationDataDeclaration.Assign(Token);
					}
				}
			}
			else
			if ( Is(Character::SingleQuote) || Is(Character::DoubleQuote) )
			{

				Substring literal;

				char sentinel = *At();

				Next(); 
				Mark(literal);

				while (*At() != sentinel)
				{

					if (ParseParameterEntityReference())
					{
						XmlEntityDeclaration *entityDeclaration = ((XmlDtdHandler*)Handled)->ProcessParameterEntityReference(Token);
						if (entityDeclaration)
						{
							entity->ReplacementText << entityDeclaration->ReplacementText;
						}
					}
					else
					{
						entity->ReplacementText << *At();
						Accept();
					}
				}	

				Trap(literal);
				Next();	

				entity->ReplacementText.Trim();
				entity->EntityValue.Assign(literal);
			}
		}
		else
		{
			((XmlDtdHandler*)Handled)->Error("ParseEntityDeclaration","Expected entity name, line %d column %d",Line(),Column());
			delete entity;
			return false;
		}

		SkipWhitespace();
		if (Is('>'))
		{
			Next();
			((XmlDtdHandler*)Handled)->ProcessEntityDeclaration(entity);
			return true;
		}

		((XmlDtdHandler*)Handled)->Error("ParseEntityDeclaration","Expected > at end of entity declaration, line %d column %d",Line(),Column());
		delete entity;

	}

	return false;
}

bool XmlDtdParser::ParseElementDeclaration()
{
	if (Is("<!ELEMENT"))
	{
		Skip(2);
		Mark();
		Skip(7);
		Trap();

		if (!SkipWhitespace())
		{
			((XmlDtdHandler*)Handled)->Error("ParseElementDeclaration","Expected whitespace, line %d column %d",Line(),Column());
			return false;
		}

		XmlElementDeclaration * elementDeclaration = new XmlElementDeclaration();
		elementDeclaration->Token.Assign(Token);

		EnterParameterEntityReference();

		Substring prefix;
		if ( !ParseQualifiedName(prefix))
		{

			LeaveParameterEntityReference();

			((XmlDtdHandler*)Handled)->Error("ParseElementDeclaration","Expected qualified name for element, line %d column %d",Line(),Column());
			delete elementDeclaration;
			return false;
		}

		LeaveParameterEntityReference();

		if (!SkipWhitespace())
		{
			((XmlDtdHandler*)Handled)->Error("ParseElementDeclaration","Expected whitespace after element name \"%s\", line %d column %d",Token.Print(),Line(),Column());
			delete elementDeclaration;
			return false;
		}

		elementDeclaration->Name.Assign(Token);

		Reason::System::String grammar;

		while (!Is('>'))
		{
			if (Eof())
			{
				((XmlDtdHandler*)Handled)->Failure("ParseElementDeclaration","Premature end of document, line %d column %d",Line(),Column());
				return false;
			}		

			if (ParseParameterEntityReference())
			{
				XmlEntityDeclaration *entityDeclaration = ((XmlDtdHandler*)Handled)->ProcessParameterEntityReference(Token);
				grammar << entityDeclaration->ReplacementText;
			}
			else
			{
				grammar << *At();
				Accept();
			}
		}

		Next();	

		elementDeclaration->ContentspecGrammar.Construct(grammar);

		BnfToken * G = elementDeclaration->ContentspecGrammar.Object;

		if (G->Type.Is(BnfToken::LABEL))
		{
			if (((BnfTokenLabel*)G)->Value.Is("EMPTY"))
			{
				elementDeclaration->ContentspecType.Assign(XmlElementDeclaration::CONTENTSPEC_EMPTY);
			}
			else
			if (((BnfTokenLabel*)G)->Value.Is("ANY"))
			{
				elementDeclaration->ContentspecType.Assign(XmlElementDeclaration::CONTENTSPEC_ANY);
			}
			else
			{
				((XmlDtdHandler*)Handled)->Error("ParseElementDeclaration","Expected \"EMPTY\" or \"ANY\" as label for content specification, line %d column %d",Line(),Column());
				return false;
			}
		}
		else
		if (G->Type.Is(BnfToken::GROUP))
		{

			BnfTokenGroup *group = (BnfTokenGroup*)G;
			G = group->One;

			if (G == 0)
			{

				((XmlDtdHandler*)Handled)->Error("ParseElementDeclaration","Grammar contained an empty group, must specify atleast one element, line %d column %d",Line(),Column());
				return false;
			}

			elementDeclaration->ContentspecType.Assign(XmlElementDeclaration::CONTENTSPEC_CHILDREN);

			BnfEnumerator enumerator;
			enumerator.Enumerate(G,false);

			if (enumerator.Forward())
			{
				if (enumerator().Type.Is(BnfToken::LABEL) && enumerator().Value.Is("#PCDATA"))
				{

					if (! (group->Operator.Is(BnfOperator::ZERO_OR_MORE) || group->Operator.Is(BnfOperator::NOOP)) )
					{
						((XmlDtdHandler*)Handled)->Error("ParseElementDeclaration","Only the \"*\" operator is allowed for a mixed grammar, line %d column %d",Line(),Column());
						return false;
					}

					elementDeclaration->ContentspecType.Assign(XmlElementDeclaration::CONTENTSPEC_MIXED);
				}

			}
			else
			{

			}

		}

		((XmlDtdHandler*)Handled)->ProcessElementDeclaration(elementDeclaration,prefix);
		return true;

	}

	return false; 
}

bool XmlDtdParser::ParseNotationDeclaration()
{

	if (Is("<!NOTATION"))
	{
		Skip(2);
		Mark();
		Skip(8);
		Trap();

		XmlNotationDeclaration *notation = new XmlNotationDeclaration();		
		notation->Token.Assign(Token);

		if ( ! (SkipWhitespace() && ParseName() && SkipWhitespace()) )
		{
			((XmlDtdHandler*)Handled)->Error("ParseNotationDeclaration","Invalid token, expected name, line %d column %d",Line(),Column());
			delete notation;
			return false;
		}

		notation->Name.Assign(Token);

		if (! ParseExternalId(notation->ExternalId))
		{

			if (!notation->ExternalId.PublicIdLiteral.IsEmpty())
			{
				notation->PublicIdLiteral.Assign(notation->ExternalId.PublicIdLiteral);

				notation->ExternalId.PublicIdLiteral.Release();
				notation->ExternalId.SystemLiteral.Release();
				notation->ExternalId.Type = XmlExternalId::NONE;
			}
			else
			{
				((XmlDtdHandler*)Handled)->Error("ParseNotationDeclaration","Expected PublicID, line %d column %d", Line(),Column());
				return false;
			}
		}

		SkipWhitespace();
		if (!Is('>'))
		{
			((XmlDtdHandler*)Handled)->Error("ParseNotationDeclaration","Invalid token, expected \">\", line %d column %d",Line(),Column());
			delete notation;
		}
		else
		{
			Next(); 
		}

		((XmlDtdHandler*)Handled)->ProcessNotationDeclaration(notation);

	}

	return false;
}

bool XmlDtdParser::ParseAttributeListDeclaration()
{

	if (Is("<!ATTLIST"))
	{
		Skip(2);
		Mark();
		Skip(7);
		Trap();

		if (!SkipWhitespace())
		{
			((XmlDtdHandler*)Handled)->Error("ParseAttributeListDeclaration","Whitespace expected after \"<!ATTLIST\", line %d column %d",Line(),Column());
			return false;
		}

		XmlAttributeListDeclaration *attributeList = new XmlAttributeListDeclaration();
		attributeList->Token.Assign(Token);

		EnterParameterEntityReference();

		Substring prefix;
		if ( !ParseQualifiedName(prefix) )
		{

			LeaveParameterEntityReference();

			((XmlDtdHandler*)Handled)->Error("ParseAttributeListDeclaration","Expected qualified name for attribute list, line %d column %d",Line(),Column());
			delete attributeList;
			return false;
		}

		LeaveParameterEntityReference();

		if ( !SkipWhitespace() )
		{
			((XmlDtdHandler*)Handled)->Error("ParseAttributeListDeclaration","Expected whitespace after attribute list name, line %d column %d",Line(),Column());
			delete attributeList;
			return false;
		}

		attributeList->Name.Assign(Token);

		while ( !Is(Character::GreaterThan))
		{			

			EnterParameterEntityReference();

			Substring attributePrefix;
			if (!ParseQualifiedName(attributePrefix))
			{

				LeaveParameterEntityReference();

				((XmlDtdHandler*)Handled)->Error("ParseAttributeListDeclaration","Expected qulified name for attribute, line %d column %d",Line(),Column());
				delete attributeList;
				return false;
			}

			if (Eof()) LeaveParameterEntityReference();

			if (!SkipWhitespace())
			{
				((XmlDtdHandler*)Handled)->Error("ParseAttributeListDeclaration","Expected whitespace after attribute name, line %d column %d",Line(),Column());
				delete attributeList;
				return false;
			}

			XmlAttributeDeclaration *attributeDeclaration = new XmlAttributeDeclaration();
			attributeDeclaration->Name.Assign(Token);

			EnterParameterEntityReference();

			if (! ParseWord())
			{
				if (Is('('))
				{

					Next();

					Reason::System::String grammar;

					while ( !Is(')') )
					{
						if (Eof())
						{
							((XmlDtdHandler*)Handled)->Error("ParseElementDeclaration","Premature end of document, line %d column %d",Line(),Column());
							return false;
						}	
						else
						if (Is('>'))
						{
							((XmlDtdHandler*)Handled)->Error("ParseElementDeclaration","Unclosed enumeration, expected \")\", line %d column %d",Line(),Column());
							return false;
						}
						else
						if (ParseParameterEntityReference())
						{
							XmlEntityDeclaration *entityDeclaration = ((XmlDtdHandler*)Handled)->ProcessParameterEntityReference(Token);
							grammar << entityDeclaration->ReplacementText;
						}
						else
						{
							grammar << *At();
							Accept();
						}
					}

					Next(); 

					attributeDeclaration->EnumeratedType.Construct(grammar);

					BnfEnumerator enumerator;
					enumerator.Enumerate(&attributeDeclaration->EnumeratedType,false );

					if (enumerator.Forward())
					{	
						do
						{
							if (!enumerator.Object->Type.Is(BnfToken::LABEL) || !IsNameToken(((BnfTokenLabel*)enumerator.Object)->Value))
							{
								((XmlDtdHandler*)Handled)->Error("ParseAttributeListDeclaration","Invalid token in EnumeratedType grammar, expected an attribute value matching the Nmtoken production, line %d column %d",Line(),Column());
								delete attributeList;
								delete attributeDeclaration;
								return false;
							}
						}
						while (enumerator.Move());
					}

					attributeDeclaration->Type = XmlAttributeDeclaration::ENUMERATED;				

				}
				else
				{
					((XmlDtdHandler*)Handled)->Error("ParseAttributeListDeclaration","Expected StringType, TokenisedType, or EnumeratedType for attribute definition, line %d column %d",Line(),Column());
					delete attributeList;
					delete attributeDeclaration;
					return false;
				}
			}

			if (Token.Is("CDATA"))
			{
				attributeDeclaration->Type = XmlAttributeDeclaration::STRING_CDATA;
			}
			else
			if (Token.Is("Instance"))
			{
				attributeDeclaration->Type = XmlAttributeDeclaration::TOKEN_ID;
			}
			else
			if (Token.Is("IDREF"))
			{
				attributeDeclaration->Type = XmlAttributeDeclaration::TOKEN_IDREF;
			}
			else
			if (Token.Is("IDREFS"))
			{
				attributeDeclaration->Type = XmlAttributeDeclaration::TOKEN_IDREFS;
			}
			else
			if (Token.Is("ENTITY"))
			{
				attributeDeclaration->Type = XmlAttributeDeclaration::TOKEN_ENTITY;
			}
			else
			if (Token.Is("ENTITIES"))
			{
				attributeDeclaration->Type = XmlAttributeDeclaration::TOKEN_ENTITIES;
			}
			else
			if (Token.Is("NMTOKEN"))
			{
				attributeDeclaration->Type = XmlAttributeDeclaration::TOKEN_NMTOKEN;
			}
			else
			if (Token.Is("NMTOKENS"))
			{
				attributeDeclaration->Type = XmlAttributeDeclaration::TOKEN_NMTOKENS;
			}
			else
			if (Token.Is("NOTATION") || Token.IsEmpty() )
			{

				if (Eof()) LeaveParameterEntityReference();

				attributeDeclaration->Type = XmlAttributeDeclaration::ENUMERATED_NOTATION;

				if ( ! (SkipWhitespace() && ParseOuter('(',')',Character::GreaterThan,false)) )
				{
					delete attributeList;
					delete attributeDeclaration;
					return false;
				}

				attributeDeclaration->EnumeratedType.Construct(Token);
				BnfEnumerator enumerator;

				enumerator.Enumerate(&attributeDeclaration->EnumeratedType,false);

				enumerator.Reverse();
				while (enumerator.Has())
				{	

					if ( ! ( enumerator.Object->Type.Is(BnfToken::LABEL)  ) )
					{
						((XmlDtdHandler*)Handled)->Error("ParseAttributeListDeclaration","Invalid token in NotationType grammar, expected a Name production, line %d column %d",Line(),Column());
						delete attributeList;
						delete attributeDeclaration;
						return false;
					}

					enumerator.Move();
				}

			}

			if (Eof()) LeaveParameterEntityReference();

			if (! SkipWhitespace())
			{		
				delete attributeList;
				delete attributeDeclaration;
				return false;
			}

			bool useFixedDeclaration = false;

			if (Is('#'))
			{
				Next();
				ParseWord();

				if (Token.Is("REQUIRED"))
				{
					attributeDeclaration->DefaultDeclarationType = XmlAttributeDeclaration::DECLARATION_REQUIRED;
				}
				else
				if (Token.Is("IMPLIED"))
				{
					attributeDeclaration->DefaultDeclarationType = XmlAttributeDeclaration::DECLARATION_IMPLIED;
				}
				else
				if (Token.Is("FIXED"))
				{
					useFixedDeclaration = true;	

					if (! SkipWhitespace())
					{

						delete attributeList;
						delete attributeDeclaration;
						return false;
					}
				}
				else
				{
					((XmlDtdHandler*)Handled)->Error("ParseAttributeListDeclaration","Invalid default declaration, expected #REQUIRED, #IMPLIED, or #FIXED, line %d column %d",Line(),Column());
					delete attributeList;
					delete attributeDeclaration;
					return false;
				}

			}

			if (Eof()) LeaveParameterEntityReference();

			if (Is(Character::DoubleQuote) || Is(Character::SingleQuote) || useFixedDeclaration)
			{

				attributeDeclaration->DefaultDeclarationType = XmlAttributeDeclaration::DECLARATION_FIXED;

				if ( !ParseString())
				{
					((XmlDtdHandler*)Handled)->Error("ParseAttributeListDeclaration","Invalid fixed declaration, expected string literal, line %d column %d",Line(),Column());
					delete attributeList;
					delete attributeDeclaration;
					return false;
				}

				attributeDeclaration->FixedDeclaration.Assign(Token);
			}

			((XmlDtdHandler*)Handled)->ProcessAttributeDeclaration(attributeList,attributeDeclaration,prefix);

			if (Eof())
			{

				LeaveParameterEntityReference();

				if (Eof())
				{
						delete attributeList;
						delete attributeDeclaration;
						return false;
				}
			}

			SkipWhitespace();
		}

		Next();
		((XmlDtdHandler*)Handled)->ProcessAttributeListDeclaration(attributeList,prefix);
		return true;
	}

	return false; 
}

void XmlDtdParser::EnterParameterEntityReference()
{
	if (ParseParameterEntityReference())
	{

		XmlEntityDeclaration * entityDeclaration = ((XmlDtdHandler*)Handled)->ProcessParameterEntityReference(Token);
		if (entityDeclaration)
		{
			ParameterEntityState = new Reason::System::Token(Scanner->Token);
			Assign(entityDeclaration->ReplacementText);
		}
	}

}

void XmlDtdParser::LeaveParameterEntityReference()
{
	if (ParameterEntityState != 0)
	{
		Load(*ParameterEntityState);

		delete ParameterEntityState;
		ParameterEntityState = 0;
	}
}

bool XmlDtdParser::ParseParameterEntityReference()
{
	if (Is('%'))
	{
		Next();

		if (ParseName() && Is(';'))
		{
			Next();

			return true;
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

