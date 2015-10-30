
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
#ifndef LANGUAGE_XML_ADAPTOR_H
#define LANGUAGE_XML_ADAPTOR_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/language/xml/xml.h"
#include "reason/language/xml/parser.h"

using namespace Reason::Language::Xml;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Language { namespace Xml {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XmlReader : public XmlParser, public XmlHandler
{
public:

	int Errors;
	int Failures;
	int Warnings;

	XmlNamespaceScope * Scope;
	XmlMarkupAssembler * Assembler;
	XmlDocument * Document;

	XmlHandler * Handler() {return this;}

	XmlReader(XmlDocument *document, XmlMarkupAssembler *assembler, XmlNamespaceScope * scope = 0):
		Document(document),Assembler(assembler),Scope(scope),Errors(0),Failures(0),Warnings(0)
	{

	}

	XmlReader():
		Document(0),Assembler(0),Scope(0),Errors(0),Failures(0),Warnings(0)
	{

	}

	virtual void Process(Sequence & sequence)=0;
	virtual void Report()=0;

};

class XmlReaderValid : public XmlReader
{
public:	

	bool Dtd;
	bool Silent;

	int ValidityErrors;
	int WellFormednessErrors;

	XmlReaderValid(XmlDocument *document, XmlMarkupAssembler *assembler, XmlNamespaceScope *scope);
	~XmlReaderValid();

	void Process(Sequence & sequence);
	void Report();

private:

	virtual void Catch(const unsigned int type,const char * message,va_list args);

	virtual bool ProcessAttribute(XmlElement *element, XmlAttribute *attribute, Substring &prefix);
	virtual bool ProcessAttributeValueReference(Substring &reference);
	virtual bool ProcessElementStart(XmlElement *element,Substring &prefix);
	virtual bool ProcessElementEnd(XmlElement *element,Substring &prefix);
	virtual bool ProcessElementEmpty(XmlElement *element,Substring &prefix);
	virtual bool ProcessXmlDeclaration(XmlDeclaration *xmlDeclaration);
	virtual bool ProcessDoctypeDeclaration(XmlDoctypeDeclaration *doctypeDeclaration, Substring &prefix);
	virtual bool ProcessText(Substring &text);

	virtual bool ProcessCdataSection(XmlCdata *cdataSection);
	virtual bool ProcessComment(Substring &comment);
	virtual bool ProcessReference(Substring &reference);	
	virtual bool ProcessExternalId(XmlExternalId &externalId);
	virtual bool ProcessProcessingInstruction(XmlProcessingInstruction *processingInstruction);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XmlReaderFast : public XmlReader
{
public:	

	XmlReaderFast(XmlDocument *document, XmlMarkupAssembler *assembler);
	~XmlReaderFast();

	void Process(Sequence & sequence);
	void Report();

private:

	virtual void Catch(const unsigned int type,const char * message,va_list args);

	virtual bool ProcessAttribute(XmlElement *element, XmlAttribute *attribute, Substring &prefix);
	virtual bool ProcessAttributeValueReference(Substring &reference);
	virtual bool ProcessElementStart(XmlElement *element,Substring &prefix);
	virtual bool ProcessElementEnd(XmlElement *element,Substring &prefix);
	virtual bool ProcessElementEmpty(XmlElement *element,Substring &prefix);
	virtual bool ProcessXmlDeclaration(XmlDeclaration *xmlDeclaration);
	virtual bool ProcessDoctypeDeclaration(XmlDoctypeDeclaration *doctypeDeclaration, Substring &prefix);
	virtual bool ProcessText(Substring &text);

	virtual bool ProcessCdataSection(XmlCdata *cdataSection);
	virtual bool ProcessComment(Substring &comment);
	virtual bool ProcessReference(Substring &reference);	
	virtual bool ProcessExternalId(XmlExternalId &externalId);
	virtual bool ProcessProcessingInstruction(XmlProcessingInstruction *processingInstruction);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XmlReaderSax
{
public:

	void StartDocument();
	void StartElement();
	void EndDocument();
	void EndElement();

private:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}
#endif

