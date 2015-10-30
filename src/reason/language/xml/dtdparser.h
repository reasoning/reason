
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
#ifndef LANGUAGE_XML_DTD_PARSER_H
#define LANGUAGE_XML_DTD_PARSER_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/language/xml/dtd.h"

using namespace Reason::Language::Xml;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Language { namespace Xml {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XmlDtdHandler : public XmlPrologueHandler
{
public:

	enum XmlDocumentTypeDefinitionExceptions
	{
		XML_EXCEPTION_DTD_ENTITY				=1<<Handler::TypeReserved,
		XML_EXCEPTION_DTD_ELEMENT				=2<<Handler::TypeReserved,
		XML_EXCEPTION_DTD_ATTRIBUTE				=3<<Handler::TypeReserved,
		XML_EXCEPTION_DTD_ATTRIBUTE_LIST		=4<<Handler::TypeReserved,
		XML_EXCEPTION_DTD_NOTATION				=5<<Handler::TypeReserved,
		XML_EXCEPTION_DTD_MARKUP				=6<<Handler::TypeReserved,
		XML_EXCEPTION_DTD_DOCUMENT				=7<<Handler::TypeReserved & Handler::HANDLER_FAILURE,

		XML_EXCEPTION_CDATA						=8<<Handler::TypeReserved,
		XML_EXCEPTION_PCDATA					=9<<Handler::TypeReserved,
		XML_EXCEPTION_COMMENT					=10<<Handler::TypeReserved,
		XML_EXCEPTION_REFERENCE					=11<<Handler::TypeReserved,
		XML_EXCEPTION_EXTERNAL_ID				=12<<Handler::TypeReserved,
		XML_EXCEPTION_PROCESSING_INSTRUCTIONS	=13<<Handler::TypeReserved,

		XML_EXCEPTION_WELL_FORMEDNESS			=14<<Handler::TypeReserved,
		XML_EXCEPTION_VALIDITY					=15<<Handler::TypeReserved,
		XML_EXCEPTION_NAMESPACE					=15<<Handler::TypeReserved,
	};

	virtual bool ProcessEntityDeclaration(XmlEntityDeclaration *entityDeclaration)=0;
	virtual bool ProcessElementDeclaration(XmlElementDeclaration *elementDeclaration, Substring &prefix)=0;
	virtual bool ProcessAttributeDeclaration(XmlAttributeListDeclaration *attributeListDeclaration, XmlAttributeDeclaration *attributeDeclaration, Substring &prefix)=0;
	virtual bool ProcessAttributeListDeclaration(XmlAttributeListDeclaration *attributeDeclaration, Substring &prefix)=0;
	virtual bool ProcessNotationDeclaration(XmlNotationDeclaration *notationDeclaration)=0;
	virtual bool ProcessTextDeclaration(XmlTextDeclaration *textDeclaration)=0;
	virtual bool ProcessIncludeSection(XmlIncludeSection *includeSection)=0;
	virtual bool ProcessIgnoreSection(XmlIgnoreSection *ignoreSection)=0;

	virtual XmlEntityDeclaration * ProcessParameterEntityReference(Substring &reference)=0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XmlDtdParser : public XmlPrologueParser
{
public:

	XmlDtdParser();
	~XmlDtdParser();

	bool Parse(Sequence &sourceCode);

private:

	bool ParseMarkupDeclaration();				
	bool ParseEntityDeclaration();				
	bool ParseElementDeclaration();				
	bool ParseAttributeListDeclaration();		
	bool ParseNotationDeclaration();
	bool ParseDeclarationSeparator();
	bool ParseInternalSubset();
	bool ParseTextDeclaration();
	bool ParseConditionalSection();
	bool ParseExternalSubset();
	bool ParseExternalSubsetDeclaration();
	bool ParseParameterEntityReference();		

	void EnterParameterEntityReference();
	void LeaveParameterEntityReference();
	class Token *ParameterEntityState;

protected:

	bool External;	
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

