
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

#include "reason/language/xml/dtdreader.h"
#include "reason/system/string.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;
using namespace Reason::Language::Xml;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlDtdReader::XmlDtdReader(XmlDtd *dtd, XmlMarkupAssembler * assembler, bool external):
	Assembler(assembler),Dtd(dtd),Errors(0),Failures(0),Warnings(0),ValidityErrors(0),WellFormednessErrors(0)
{

	Handled = this;
	External = external;
	Recovery = false;
}

XmlDtdReader::~XmlDtdReader()
{
}

void XmlDtdReader::Process(Sequence & sequence)
{
	if (Assembler)
		Assembler->Initialise((XmlMarkup*)Dtd,(Reason::System::Handler *)this);

	Parse(sequence);

	if (Assembler)
		Assembler->Finalise();
}

bool XmlDtdReader::ProcessEntityDeclaration(XmlEntityDeclaration *entityDeclaration)
{

	Dtd->EntityTable.Insert(entityDeclaration);
	return Assembler->Assemble(entityDeclaration);

}

bool XmlDtdReader::ProcessElementDeclaration(XmlElementDeclaration *elementDeclaration,Substring &prefix)
{

	XmlElementDeclaration *ed = (XmlElementDeclaration *)Dtd->ElementTable.Select(elementDeclaration);
	if (ed != 0)
	{

		((XmlDtdHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttributeDeclaration - Duplicate element declaration \"%s\", line %d column %d",elementDeclaration->Name.Print(),Line(),Column());
	}

	if (elementDeclaration->ContentspecType.Is(XmlElementDeclaration::CONTENTSPEC_MIXED))
	{

		Path sequence;

		BnfEnumerator enumerator;
		enumerator.Enumerate(&elementDeclaration->ContentspecGrammar);

		while (enumerator.Has())
		{
			if (enumerator.Object->Type.Is(BnfToken::LABEL))
			{
				if (sequence.Select(((BnfTokenLabel *)enumerator.Object)->Value,true))
				{
					((XmlDtdHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttributeDeclaration - Duplicate name \"%s\" in mixed-content declaration, line %d column %d",((BnfTokenLabel *)enumerator.Object)->Value.Print(),Line(),Column());
				}
				else
				{
					sequence.Append(new Path(((BnfTokenLabel *)enumerator.Object)->Value));
				}
			}

			enumerator.Move();
		}
	}

	Dtd->ElementTable.Insert(elementDeclaration);
	return Assembler->Assemble(elementDeclaration);
}

bool XmlDtdReader::ProcessAttributeDeclaration(XmlAttributeListDeclaration *attributeListDeclaration, XmlAttributeDeclaration *attributeDeclaration,Substring &prefix)
{

	if (prefix.Is("xmlns"))
	{
		attributeDeclaration->Type.Assign(XmlAttributeDeclaration::NAMESPACE_DECLARATION);

		XmlNamespace *ns = new XmlNamespace();
		ns->Prefix.Assign(attributeDeclaration->Name);
		ns->Resource.Assign(attributeDeclaration->Value);
		ns->DeclarationAttribute = attributeDeclaration;
		ns->DeclarationElement = attributeListDeclaration;

		Dtd->Namespaces.Insert(ns);
	}
	else
	if (prefix.IsEmpty() && attributeDeclaration->Name.Is("xmlns"))
	{
		attributeDeclaration->Type.Assign(XmlAttributeDeclaration::NAMESPACE_DEFAULT_DECLARATION);

		XmlNamespace *ns = new XmlNamespace();
		ns->Prefix.Assign(attributeDeclaration->Name);
		ns->Resource.Assign(attributeDeclaration->Value);
		ns->DeclarationAttribute = attributeDeclaration;
		ns->DeclarationElement = attributeListDeclaration;

		Dtd->Namespaces.Insert(ns);
	}

	if ( ! attributeDeclaration->DefaultDeclarationType.Is(XmlAttributeDeclaration::DECLARATION_FIXED) )
	{

	}

	if (attributeDeclaration->Type.Equals(XmlAttributeDeclaration::TOKEN_ID))
	{
		if (attributeDeclaration->DefaultDeclarationType.Is(XmlAttributeDeclaration::DECLARATION_FIXED))
		{

			((XmlDtdHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttributeDeclaration - ID attribute \"%s\" must have a declared default of #IMPLIED or #REQUIRED, line %d column %d",attributeDeclaration->Name.Print(),Line(),Column());	
		}

		if (attributeListDeclaration->Forward())
		{

			do
			{
				if (attributeListDeclaration->Pointer()->Type.Is(XmlAttributeDeclaration::TOKEN_ID))
				{
					((XmlDtdHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttribute - Attribute \"%s\" not allowed, ID attribute already specified, line %d column %d",attributeDeclaration->Name.Print(),Line(),Column());
					break;
				}
			}
			while (attributeListDeclaration->Move());
		}	
	}
	else
	if (attributeDeclaration->Type.Equals(XmlAttributeDeclaration::ENUMERATED_NOTATION))
	{

		BnfEnumerator enumerator(&attributeDeclaration->EnumeratedType);

		while (enumerator.Has())
		{
			if (enumerator.Object->Type.Is(BnfToken::TERMINAL|BnfToken::LITERAL|BnfToken::LABEL))
			{
				if (! Dtd->NotationTable.Select(&enumerator().Value))
				{
					((XmlDtdHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttributeDeclaration - Notation attribute declaration name \"%s\" must be declared, line %d column %d",enumerator().Value.Print(),Line(),Column());
				}
			}	

			enumerator.Move();
		}

		if (attributeListDeclaration->FindDeclaration(XmlAttributeDeclaration::ENUMERATED_NOTATION) != 0)
		{
			((XmlDtdHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttributeDeclaration - No element type may have mroe than one NOTATION attribute sepcified \"%s\", line %d column %d",attributeDeclaration->Name.Print(), Line(),Column());
		}

		XmlElementDeclaration * elementDeclaration = (XmlElementDeclaration *) Dtd->ElementTable.Select(&attributeListDeclaration->Name);

		if (elementDeclaration != 0 && elementDeclaration->ContentspecType.Is(XmlElementDeclaration::CONTENTSPEC_EMPTY))
		{
			((XmlDtdHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttributeDeclaration - NOTATION must not be declared on an element declared EMPTY \"%s\", line %d column %d",elementDeclaration->Name.Print(), Line(),Column());
		}

	}

	attributeListDeclaration->Append(attributeDeclaration);

	return true;
}

bool XmlDtdReader::ProcessAttributeListDeclaration(XmlAttributeListDeclaration *attributeListDeclaration,Substring &prefix)
{

	if (!prefix.IsEmpty())
	{

		XmlNamespace *ns = (XmlNamespace *) Dtd->Namespaces.Select(&prefix);
		if (ns == 0 || ns->DeclarationElement != attributeListDeclaration)
		{
			((XmlDtdHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttributeDeclaration - Namespace prefix \"%s\" is undeclared, line %d column %d",prefix.Print(),Line(),Column());
		}
	}

	XmlElementDeclaration * elementDeclaration = (XmlElementDeclaration *) Dtd->ElementTable.Select(&attributeListDeclaration->Name);

	if (elementDeclaration == 0)
	{
		((XmlDtdHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessAttributeListDeclaration - No matching element declaration for attribute list \"%s\", line %d column %d",attributeListDeclaration->Name.Print(),Line(),Column());	
	}

	Dtd->AttributeListTable.Insert(attributeListDeclaration);
	return Assembler->Assemble(attributeListDeclaration);
}

bool XmlDtdReader::ProcessNotationDeclaration(XmlNotationDeclaration *notationDeclaration)
{

	XmlNotationDeclaration * nd = (XmlNotationDeclaration *) Dtd->NotationTable.Select(notationDeclaration);
	if (nd != 0)
	{

		((XmlDtdHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessNotationDeclaration - Notation name \"\" is already declared, line %d column %d",notationDeclaration->Name.Print(),Line(),Column());
	}

	Dtd->NotationTable.Insert(notationDeclaration);
	return Assembler->Assemble(notationDeclaration);
}

bool XmlDtdReader::ProcessTextDeclaration(XmlTextDeclaration *textDeclaration)
{
	return Assembler->Assemble(textDeclaration); 
}

bool XmlDtdReader::ProcessIncludeSection(XmlIncludeSection *includeSection)
{
	return Assembler->Assemble(includeSection);
}

bool XmlDtdReader::ProcessIgnoreSection(XmlIgnoreSection *ignoreSection)
{
	return Assembler->Assemble(ignoreSection);
}

XmlEntityDeclaration * XmlDtdReader::ProcessParameterEntityReference(Substring &reference)
{

	XmlEntityDeclaration * entityDeclaration = (XmlEntityDeclaration *) Dtd->EntityTable.Select(&reference);

	if (entityDeclaration != 0 && entityDeclaration->EntityType.Is(XmlEntityDeclaration::ENTITY_PARAMETER))
	{
		if (entityDeclaration->ReplacementText.IsEmpty())
		{

			Reason::System::String value;
			if (entityDeclaration->IsExternalEntity() && entityDeclaration->IsParsedEntity())
			{

				Url url;
				url.Construct(entityDeclaration->ExternalId.SystemLiteral);

				if (!url.Download(value))
				{
					((XmlDtdHandler*)Handled)->Warning("ProcessParameterEntityReference - Could not load system literal URL \"%s\", line %d column %d",url.Print(),Line(),Column());
				}
			}
			else
			if (entityDeclaration->IsInternalEntity())
			{
				value.Construct(entityDeclaration->EntityValue);
			}
			else
			{
				((XmlDtdHandler*)Handled)->Warning("ProcessParameterEntityReference - Unhandled external entity type, line %d column %d",Line(),Column());
			}

			value.Trim();
			entityDeclaration->ReplacementText.Construct(value);
		}

		return entityDeclaration;
	}
	else
	{

		((XmlDtdHandler*)Handled)->Throw(XML_EXCEPTION_VALIDITY,"ProcessParameterEntityReference - Undeclared parameter entity reference \"%s\", line %d column %d",reference.Print(),Line(),Column());
		return 0;
	}

}

 void XmlDtdReader::Catch(const unsigned int type,const char * message,va_list args)
{
	OutputMessage("XmlDtdReader::Catch - %s: ",Description(type));

	switch (type & Handler::TypeMask)
	{
		case Handler::HANDLER_ERROR:
			++Errors;
		break;
		case Handler::HANDLER_FAILURE:
			++Failures;
		break;
		case Handler::HANDLER_WARNING:
			++Warnings;
		break;
	};

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

bool XmlDtdReader::ProcessCdataSection(XmlCdata *cdataSection)
{

	return Assembler->Assemble(cdataSection);
}

bool XmlDtdReader::ProcessComment(Substring &comment)
{

	return false;
}

bool XmlDtdReader::ProcessReference(Substring &reference)
{

	return false;
}

bool XmlDtdReader::ProcessExternalId(XmlExternalId &externalId)
{

	return false;
}

bool XmlDtdReader::ProcessProcessingInstruction(XmlProcessingInstruction *processingInstruction)
{

	return Assembler->Assemble(processingInstruction);
}

void XmlDtdReader::Report()
{

	OutputMessage("XmlDtdReader::Report - %d Failure(s), %d Error(s), %d Warning(s)",Failures,Errors,Warnings);
	OutputMessage(" - %d Validity Error(s), %d Well-formedness Error(s)\n",ValidityErrors, WellFormednessErrors);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

