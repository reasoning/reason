
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
#ifndef LANGUAGE_XML_DTD_ADAPTOR_H
#define LANGUAGE_XML_DTD_ADAPTOR_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/language/xml/dtd.h"
#include "reason/language/xml/dtdparser.h"

using namespace Reason::Language::Xml;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Language { namespace Xml {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XmlDtdReader : public XmlDtdParser, XmlDtdHandler
{
public:

	XmlDtdReader(XmlDtd *dtd, XmlMarkupAssembler * assembler, bool external = true);
	~XmlDtdReader();

	void Process(Sequence & sequence);
	void Report();

	int ValidityErrors;
	int WellFormednessErrors;
	int	Errors;
	int Failures;
	int Warnings;

protected:

	virtual void Catch(const unsigned int type,const char * message,va_list args);

	virtual bool ProcessEntityDeclaration(XmlEntityDeclaration *entityDeclaration);
	virtual bool ProcessElementDeclaration(XmlElementDeclaration *elementDeclaration, Substring &prefix);
	virtual bool ProcessAttributeDeclaration(XmlAttributeListDeclaration *attributeListDeclaration, XmlAttributeDeclaration *attributeDeclaration, Substring &prefix);
	virtual bool ProcessAttributeListDeclaration(XmlAttributeListDeclaration *attributeListDeclaration, Substring &prefix);
	virtual bool ProcessNotationDeclaration(XmlNotationDeclaration *notationDeclaration);
	virtual bool ProcessTextDeclaration(XmlTextDeclaration *textDeclaration);
	virtual bool ProcessIncludeSection(XmlIncludeSection *includeSection);
	virtual bool ProcessIgnoreSection(XmlIgnoreSection *ignoreSection);
	virtual XmlEntityDeclaration * ProcessParameterEntityReference(Substring &reference);

	virtual bool ProcessCdataSection(XmlCdata *cdataSection);
	virtual bool ProcessComment(Substring &comment);
	virtual bool ProcessReference(Substring &reference);	
	virtual bool ProcessExternalId(XmlExternalId &externalId);
	virtual bool ProcessProcessingInstruction(XmlProcessingInstruction *processingInstruction);

	XmlDtd				*Dtd;
	XmlMarkupAssembler	*Assembler;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

