
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

#include "reason/language/xml/reader.h"

#include "reason/language/xml/dtd.h"
#include "reason/language/xml/dtdreader.h"
#include "reason/language/bnf/bnf.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::Language;
namespace Reason { namespace Language { namespace Xml {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlReaderValid::XmlReaderValid(XmlDocument *document,XmlMarkupAssembler *assembler, XmlNamespaceScope *scope):
	XmlReader(document,assembler,scope),Silent(false),Dtd(false),ValidityErrors(0),WellFormednessErrors(0)	
{

	Handled = this;
}

XmlReaderValid::~XmlReaderValid()
{

}

void XmlReaderValid::Process(Sequence & sequence)
{
	if (Assembler)
		Assembler->Initialise((XmlMarkup *)Document,(Reason::System::Handler *)this);

	Parse(sequence);

	if (Assembler)
		Assembler->Finalise();
}

bool XmlReaderValid::ProcessReference(Substring &reference)
{

	return true;

}

bool XmlReaderValid::ProcessAttributeValueReference(Substring &reference)
{

	if (reference.StartsWith('#'))
		return false;

	if (Document->DoctypeDeclaration != 0 && !Document->DoctypeDeclaration->Dtd.IsEmpty())
	{

		Reason::Structure::Objects::Entry *entry = Document->DoctypeDeclaration->Dtd.EntityTable.MatchEntry(reference.Hash());

		while (entry != 0)
		{
			XmlEntityDeclaration * entityDeclaration = (XmlEntityDeclaration*)entry->Object;

			if (entityDeclaration->Name.Is(reference,true))
			{

				if (entityDeclaration->IsExternalEntity())
				{

					((XmlHandler*)Handled)->Throw(XML_EXCEPTION_WELL_FORMEDNESS,"ProcessAttributeValueReference","Attribute values cannot contain direct or indirect references to external entities, line %d column %d",Line(), Column());

				}
				else
				{

					if (entityDeclaration->EntityValue.Contains("<"))
						((XmlHandler*)Handled)->Throw(XML_EXCEPTION_WELL_FORMEDNESS,"ProcessAttributeValueReference","< not allowed in Attribute Values, line %d column %d",Line(), Column());
				}

				break;
			}
			entry = entry->Next;
		}

	}

	return true;
}

bool XmlReaderValid::ProcessXmlDeclaration(XmlDeclaration *xmlDeclaration)
{

	return Assembler->Assemble(xmlDeclaration);
}

bool XmlReaderValid::ProcessAttribute(XmlElement *element, XmlAttribute *attribute, Substring &prefix)
{

	if ( !prefix.IsEmpty())
	{

		if (prefix.Is("xmlns",true))
		{
			XmlNamespace *ns = new XmlNamespace();
			ns->Prefix.Assign(attribute->Name);
			ns->Resource.Assign(attribute->Value);
			ns->DeclarationAttribute = attribute;
			ns->DeclarationElement = element;

			attribute->Type.Assign(XmlAttribute::NAMESPACE_DECLARATION);
			attribute->Namespace = ns;	

			Scope->Push(ns);

			Document->Namespaces.Insert(ns);
		}
		else
		{

			XmlNamespace *ns =  Scope->Peek(prefix);

			if (ns)
			{

				attribute->Namespace = ns;
			}
			else
			{
				((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","Attribute uses undefined namespace, line %d column %d",Line(), Column());

			}

		}
	}
	else
	if (attribute->Name.Is("xmlns"))
	{

		XmlNamespace *ns = new XmlNamespace();
		ns->Prefix.Assign(attribute->Name);
		ns->Resource.Assign(attribute->Value);

		attribute->Type.Assign(XmlAttribute::NAMESPACE_DEFAULT_DECLARATION);
		attribute->Namespace = ns;	

		Scope->Push(ns);

		Document->Namespaces.Insert(ns);
	}

	if (element->Attributes.Select(attribute))
	{

		((XmlHandler*)Handled)->Throw(XML_EXCEPTION_WELL_FORMEDNESS,"ProcessAttribute","No attribute name may appear more than once in the same start-tag or empty-element tag, line %d column %d",Line(), Column());	
		delete attribute;
		return false;
	}

	if (Document->DoctypeDeclaration != 0 && ! Document->DoctypeDeclaration->Dtd.IsEmpty() )	
	{

		XmlAttributeListDeclaration * attributeListDeclaration = (XmlAttributeListDeclaration *)Document->DoctypeDeclaration->Dtd.AttributeListTable.Select(element);

		if (attributeListDeclaration != 0)
		{

			XmlAttributeDeclaration * attributeDeclaration = (XmlAttributeDeclaration *)attributeListDeclaration->Select(&attribute->Name);

			if (attributeDeclaration != 0)
			{

				if (attributeDeclaration->DefaultDeclarationType.Is(XmlAttributeDeclaration::DECLARATION_FIXED))
				{
					if (! attribute->Value.Is(attributeDeclaration->FixedDeclaration))
					{
						((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","Instances of #FIXED attributes must match the default value in the declaration, line %d column %d",Line(), Column());	
					}
				}

				switch (attributeDeclaration->Type)
				{
					case XmlAttributeDeclaration::TOKEN_ID:
					{

						if ( !XmlParser::IsName(attribute->Value) )
						{
							((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","Attribute value of type ID must match the Name production, line %d column %d",Line(), Column());	
						}

						if ( Document->Identifiers.Select(&attribute->Value) )
						{
							((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","ID values must be unique, value must not appear more than once in an XML document, line %d column %d",Line(), Column());	
						}
						else
						{

							XmlId *id = new XmlId(element, attribute);
							Document->Identifiers.Insert(id);
						}

					}	
					break;
					case XmlAttributeDeclaration::TOKEN_IDREF:
					{

						if ( IsName(attribute->Value) )
						{
							if ( Document->Identifiers.Select(&attribute->Value) == 0)
							{
								((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","IDREF Names must match the value of an Instance attribute on some element in the document, line %d column %d",Line(), Column());	
							}	
						}
						else
						{
							((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","IDREF must match the Name production, line %d column %d",Line(), Column());	
						}

					}
					break;
					case XmlAttributeDeclaration::TOKEN_IDREFS:
					{

						Path set;
						set.Construct(attribute->Value);
						set.Tokenise(" \n\r\t");	

						Path *node = set.First;
						while (node != 0)
						{
							if ( IsName(*node) )
							{
								if ( Document->Identifiers.Select(node) == 0 )
								{
									((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","IDREFS Names must match the value of an ID attribute on some element in the document, line %d column %d",Line(), Column());	
								}	
							}
							else
							{
								((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","IDREFS must match the Name production, line %d column %d",Line(), Column());	
							}

							node = node->Next;
						}

					}
					break;
					case XmlAttributeDeclaration::TOKEN_ENTITY:
					{

						if ( !IsName(attribute->Value) )
						{
							((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","Values of type ENTITY must match the Name production, line %d column %d",Line(), Column());	
						}

						Reason::Structure::Objects::Entry * entry = Document->DoctypeDeclaration->Dtd.EntityTable.SelectEntry(&attribute->Value);

						if (entry == 0 || ((XmlEntityDeclaration*)entry->Object)->IsUnparsedEntity())
						{
							((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","Values of type ENTITY must match the name of an unparsed entity declared in the DTD, line %d column %d",Line(), Column());	
						}

					}
					break;
					case XmlAttributeDeclaration::TOKEN_ENTITIES:
					{

						Path set;
						set.Construct(attribute->Value);
						set.Tokenise(" \n\r\t");	

						Path *node = set.First;
						while (node != 0)
						{

							Reason::Structure::Objects::Entry *entry = Document->DoctypeDeclaration->Dtd.EntityTable.SelectEntry(node);

							if (entry == 0 || ((XmlEntityDeclaration*)entry->Object)->IsUnparsedEntity())
							{
								((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","Values of type ENTITY must match the name of an unparsed entity declared in the DTD, line %d column %d",Line(), Column());	
							}

							node = node->Next;
						}

					}
					break;
					case XmlAttributeDeclaration::TOKEN_NMTOKEN:
					{

						if ( !IsNameToken(attribute->Value))
						{
							((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","Values of type NMTOKEN must match the Nmtoken production, line %d column %d",Line(), Column());	
						}

					}
					break;
					case XmlAttributeDeclaration::TOKEN_NMTOKENS:
					{

						Path sequence(attribute->Value);
						sequence.Tokenise(" \n\r\t");	

						Path::Enumerator iterator(sequence);

						for (iterator.Forward();iterator.Has();iterator.Move())
						{
							if ( !IsNameToken(iterator.Reference()))
							{
								((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","Values of type NMTOKENS must match the Nmtokens production, line %d column %d",Line(), Column());	
							}
						}
					}
					break;
					case XmlAttributeDeclaration::STRING_CDATA:
					{

					}
					break;
					case XmlAttributeDeclaration::ENUMERATED:
					{

						BnfMatcher matcher;
						if (! matcher.Match(attribute->Value))

						{
							((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","Enumerated attribute value must match one of the values provided in the declaration, line %d column %d",Line(), Column() );	
						}

					}
					break;
					case XmlAttributeDeclaration::ENUMERATED_NOTATION:
					{

						BnfMatcher matcher;
						if (! matcher.Match(attribute->Value))

						{
							((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","Enumerated attribute value must match one of the values provided in the declaration, line %d column %d",Line(), Column());	
						}

					}
					break;
				};

			}
			else
			{

				((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","The attribute must have been declared, no attribute declaration found in attribute list declaration for this element, line %d column %d",Line(), Column());	

			}

		}
		else
		{

			((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute","The attribute must have been declared, no attribute list declaration found for this element, line %d column %d",Line(), Column());	
		}

	}

	element->Attributes.Append(attribute);

	return true;
}

bool XmlReaderValid::ProcessElementStart(XmlElement *element, Substring &prefix)
{

	if (!prefix.IsEmpty())
	{

		element->Namespace = Scope->Peek(prefix);

		if (element->Namespace == 0)
			((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessElementStart","Qualified element start %s uses undefined namespace \"%s\", line %d column %d", element->Print(),prefix.Print(), Line(),Column());
	}
	else
	{

		element->Namespace = Scope->Peek();

		if (element->Namespace == 0)
			((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessElementStart","Could not locate default namespace for element start %s , line %d column %d", element->Print(),Line(),Column());
	}

	if (Document->Dtd != 0 && ! Document->Dtd->IsEmpty())
	{

		XmlDtd *dtd = Document->Dtd;

		XmlElementDeclaration * elementDeclaration = (XmlElementDeclaration *) dtd->ElementTable.Select(element);
		XmlAttributeListDeclaration * attributeListDeclaration = (XmlAttributeListDeclaration *)dtd->AttributeListTable.Select(element);

		if (elementDeclaration != 0)
		{

			if (elementDeclaration->ContentspecType.Is(XmlElementDeclaration::CONTENTSPEC_EMPTY) && !element->Type.Is(XmlObject::XML_EMPTY))
			{
				if (Recovery)
				{

					element->Type.Assign(XmlObject::XML_EMPTY);
					return ProcessElementEmpty(element,prefix);

				}
				else
				{
					((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessElementStart","Element declaration contentspec is EMPTY, content not allowed in this element, line %d column %d",Line(), Column());
				}
			}

		}
		else
		{
			((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessElementStart","No matching element declaration found in DTD, line %d column %d",Line(), Column());
		}

		if (attributeListDeclaration != 0)
		{

			XmlAttributeDeclaration * attributeDeclaration = attributeListDeclaration->FindDefaultDeclaration(XmlAttributeDeclaration::DECLARATION_REQUIRED);
			while (attributeDeclaration != 0)
			{
				if ( element->Attributes.Select(&attributeDeclaration->Name) == 0)
				{
					((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessElementStart","Missing required attribute, line %d column %d",Line(), Column());
				}

				attributeDeclaration = attributeListDeclaration->FindDefaultDeclaration(XmlAttributeDeclaration::DECLARATION_REQUIRED,attributeDeclaration);
			}

		}
	}

	if (!Assembler->Assemble(element))
	{
		((XmlHandler*)Handled)->Error("ProcessElementStart","Qualified element start could not be added to the document, line %d column %d",Line(),Column());
		return false;
	}

	return true;
}

bool XmlReaderValid::ProcessElementEnd(XmlElement *element,Substring &prefix)
{

	if (!prefix.IsEmpty())
	{

		element->Namespace = Scope->Peek(prefix);

		if (element->Namespace == 0)
			((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessElementEnd","Qualified element end %s uses undefined namespace \"%s\", line %d column %d",element->Print(),prefix.Print(), Line(),Column());
	}
	else
	{

		element->Namespace = Scope->Peek();
	}

	if (!Assembler->Assemble(element))
	{
		((XmlHandler*)Handled)->Throw(XML_EXCEPTION_WELL_FORMEDNESS,"ProcessElementEnd","Qualified element end has no matching element start, line %d column %d",Line(),Column());
		return false;
	}

	XmlNamespace *ns = Scope->Peek();
	if (ns != 0 && ns->DeclarationElement == element->Before)
	{
		Scope->Pop();
	}

	if (Document->Dtd != 0 && ! Document->Dtd->IsEmpty())
	{

		XmlDtd *dtd = Document->Dtd;

		XmlElementDeclaration * elementDeclaration = (XmlElementDeclaration *) dtd->ElementTable.Select(element);
		XmlAttributeListDeclaration * attributeListDeclaration = (XmlAttributeListDeclaration *)dtd->AttributeListTable.Select(element);

		if (elementDeclaration != 0)
		{

			if (elementDeclaration->ContentspecType.Is(XmlElementDeclaration::CONTENTSPEC_EMPTY) && !element->Type.Is(XmlObject::XML_EMPTY))
			{
				((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessElementEnd","Element declaration contentspec is EMPTY, content not allowed in this element, line %d column %d",Line(), Column());
			}

			if (elementDeclaration->ContentspecType.Is(XmlElementDeclaration::CONTENTSPEC_MIXED) || elementDeclaration->ContentspecType.Is(XmlElementDeclaration::CONTENTSPEC_CHILDREN))
			{

				BnfMatcher matcher;
				if (! matcher.Match(element))

				{
					((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessElementEnd","Element content does not match contentspec grammar in DTD, line %d column %d",Line(), Column());
				}
			}

		}
		else
		{
			((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessElementEnd","No matching element declaration found in DTD, line %d column %d",Line(), Column());
		}

	}

	return true;

}

bool XmlReaderValid::ProcessElementEmpty(XmlElement *element,Substring &prefix)
{
	if (!prefix.IsEmpty())
	{

		element->Namespace = Scope->Peek(prefix);

		if (element->Namespace == 0)
			((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessElementEmpty","Qualified element empty uses undefined namespace %s, line[%d] character[%d]\n",prefix.Print(), Line(),Column());
	}
	else
	{

		element->Namespace = Scope->Peek();
	}

	if (Document->Dtd != 0 && ! Document->Dtd->IsEmpty())
	{

		XmlDtd *dtd = Document->Dtd;

		XmlElementDeclaration * elementDeclaration = (XmlElementDeclaration *) dtd->ElementTable.Select(element);
		XmlAttributeListDeclaration * attributeListDeclaration = (XmlAttributeListDeclaration *)dtd->AttributeListTable.Select(element);

		if (elementDeclaration != 0)
		{

			if (! elementDeclaration->ContentspecType.Is(XmlElementDeclaration::CONTENTSPEC_EMPTY))
			{

				if (Recovery)
				{

					element->Type.Assign(XmlObject::XML_START);
					return ProcessElementStart(element,prefix);
				}
				else
				{
					((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessElementEmpty","EMPTY element does not match contentspec in declaration, line %d column %d",Line(), Column());
				}

			}

		}
		else
		{
			((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessElementEmpty","No matching element declaration found in DTD, line %d column %d",Line(), Column());
		}

		if (attributeListDeclaration != 0)
		{

			XmlAttributeDeclaration * attributeDeclaration = attributeListDeclaration->FindDefaultDeclaration(XmlAttributeDeclaration::DECLARATION_REQUIRED);
			while (attributeDeclaration != 0)
			{
				if ( element->Attributes.Select(&attributeDeclaration->Name) == 0)
				{
					((XmlHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessElementEmpty","Missing required attribute, line %d column %d",Line(), Column());
				}

				attributeDeclaration = attributeListDeclaration->FindDefaultDeclaration(XmlAttributeDeclaration::DECLARATION_REQUIRED,attributeDeclaration);
			}

		}
	}

	return Assembler->Assemble(element);
}

bool XmlReaderValid::ProcessDoctypeDeclaration(XmlDoctypeDeclaration *doctypeDeclaration, Substring &prefix)
{	
	if (Dtd)
	{

		if (!doctypeDeclaration->ExternalId.SystemLiteral.IsEmpty())
		{
			Document->Dtd = new XmlDtd();
			Document->Dtd->Create(doctypeDeclaration->ExternalId.SystemLiteral);
		}
	}

	return Assembler->Assemble(doctypeDeclaration);
}

bool XmlReaderValid::ProcessCdataSection(XmlCdata *cdataSection)
{
	return Assembler->Assemble(cdataSection);
}

bool XmlReaderValid::ProcessExternalId(XmlExternalId &externalId)
{

	return false;
}

bool XmlReaderValid::ProcessComment(Substring &comment)
{
	return Assembler->Assemble(new XmlComment(comment));
}

bool XmlReaderValid::ProcessProcessingInstruction(XmlProcessingInstruction *processingInstruction)
{
	return Assembler->Assemble(processingInstruction);
}

bool XmlReaderValid::ProcessText(Substring &text)
{
	if (Document->IsEmpty())
	{
		((XmlHandler*)Handled)->Throw(XML_EXCEPTION_WELL_FORMEDNESS,"ProcessText","Text occurs before the first element at line %d column %d",Line(),Column());
	}

	XmlText *xmlText = new XmlText();
	xmlText->Token.Assign(text);
	return Assembler->Assemble(xmlText);
}

void XmlReaderValid::Catch(const unsigned int type,const char * message,va_list args)
{

	switch (type & Handler::TypeMask)
	{
		case Handler::HANDLER_FAILURE:
			++Failures;
		break;
		case Handler::HANDLER_WARNING:
			++Warnings;
		break;

		default:	
			++Errors;
	};

	if (!Silent)
	{
		OutputMessage("XmlReader::Catch - %s: ", Description(type));

		if (type & XML_EXCEPTION_VALIDITY)
		{
			++ValidityErrors;
			OutputMessage("(Validity constraint) ");
		}
		else
		if (type & XML_EXCEPTION_WELL_FORMEDNESS)
		{
			++WellFormednessErrors;
			OutputMessage("(Well-formedness constraint) ");
		}

		#ifdef REASON_PRINT_MESSAGE
		vfprintf(REASON_HANDLE_OUTPUT,message,args);
		#endif

		OutputMessage("\n");
	}
}

void XmlReaderValid::Report()
{
	OutputMessage("XmlReader::Report - %d Failure(s), %d Error(s), %d Warning(s)",Failures,Errors,Warnings);
	OutputMessage(" - %d Validity Error(s), %d Well-formedness Error(s)\n",ValidityErrors, WellFormednessErrors);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlReaderFast::XmlReaderFast(XmlDocument *document,XmlMarkupAssembler *assembler):
	XmlReader(document,assembler)
{

	Handled = this;
}

XmlReaderFast::~XmlReaderFast()
{

}

void XmlReaderFast::Catch(const unsigned int type, const char * message,va_list args)
{
	OutputMessage("XmlReaderFast::Catch - %s: ", Description(type));

	switch (type & Handler::TypeMask)
	{
		case Handler::HANDLER_FAILURE:
			++Failures;
		break;
		case Handler::HANDLER_WARNING:
			++Warnings;
		break;

		default:	
			++Errors;
	};

	#ifdef REASON_PRINT_MESSAGE
	vfprintf(REASON_HANDLE_OUTPUT,message,args);
	#endif

	OutputMessage("\n");

}

void XmlReaderFast::Process(Sequence & sequence)
{
	if (Assembler)
		Assembler->Initialise((XmlMarkup*)Document,(Reason::System::Handler*)this);

	Parse(sequence);

	if (Assembler)
		Assembler->Finalise();
}

void XmlReaderFast::Report()
{
	OutputMessage("XmlReaderFast::Report:\n");
	OutputMessage("%d Failure(s), %d Error(s), %d Warning(s)\n",Failures,Errors,Warnings);
}

bool XmlReaderFast::ProcessAttribute(XmlElement *element,XmlAttribute *attribute,Substring &prefix)
{
	element->Attributes.Append(attribute); 
	return true;
}

bool XmlReaderFast::ProcessAttributeValueReference(Substring &reference)
{

	return true;
}

bool XmlReaderFast::ProcessElementStart(XmlElement *element,Substring &prefix)
{
	return Assembler->Assemble(element);
}

bool XmlReaderFast::ProcessElementEnd(XmlElement *element,Substring &prefix)
{
	return Assembler->Assemble(element);
}

bool XmlReaderFast::ProcessElementEmpty(XmlElement *element,Substring &prefix)
{
	return Assembler->Assemble(element);
}

bool XmlReaderFast::ProcessXmlDeclaration(XmlDeclaration *xmlDeclaration)
{
	return Assembler->Assemble(xmlDeclaration);
}

bool XmlReaderFast::ProcessDoctypeDeclaration(XmlDoctypeDeclaration *doctypeDeclaration,Substring &prefix)
{
	return Assembler->Assemble(doctypeDeclaration);
}

bool XmlReaderFast::ProcessText(Substring &text)
{
	return Assembler->Assemble(new XmlText(text));
}

bool XmlReaderFast::ProcessCdataSection(XmlCdata *cdataSection)
{
	return Assembler->Assemble(cdataSection);
}

bool XmlReaderFast::ProcessComment(Substring &comment)
{
	return Assembler->Assemble(new XmlComment(comment));
}

bool XmlReaderFast::ProcessReference(Substring &reference)
{

	return false;
}

bool XmlReaderFast::ProcessExternalId(XmlExternalId &externalId)
{

	return false;
}

bool XmlReaderFast::ProcessProcessingInstruction(XmlProcessingInstruction *processingInstruction)
{
	return Assembler->Assemble(processingInstruction); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

