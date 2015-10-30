
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

#include "reason/language/xml/document.h"
#include "reason/language/xml/parser.h"
#include "reason/language/xml/reader.h"
#include "reason/language/xml/dtd.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Language { namespace Xml {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity XmlDocument::Instance;

XmlDocument::XmlDocument(Resolver * resolver):
#ifdef REASON_USING_XML_STYLE
	Resource(resolver),Index(0),DoctypeDeclaration(0),Declaration(0),Dtd(0),Stylesheet(0)
#else
	Resource(resolver),Index(0),DoctypeDeclaration(0),Declaration(0),Dtd(0)
#endif

{
}

XmlDocument::XmlDocument(): 
#ifdef REASON_USING_XML_STYLE
	Index(0),DoctypeDeclaration(0),Declaration(0),Dtd(0),Stylesheet(0)	
#else
	Index(0),DoctypeDeclaration(0),Declaration(0),Dtd(0)
#endif
{
}

XmlDocument::~XmlDocument()
{

	Destroy();
}

bool XmlDocument::Download(const Url & url)
{
	Resource.Download(url);
	return Construct();
}

bool XmlDocument::Load(const File & file)
{
	Resource.Load(file);
	return Construct();
}

bool XmlDocument::Create(char * data, int size)
{
	Resource.Create(data,size);
	return Construct();
} 

bool XmlDocument::Construct()
{
	if (!IsEmpty())
		Destroy();

	if (Index)
	{	
		XmlMarkupConstructor constructor;
		XmlMarkupIndexer indexer(Index,&constructor);
		XmlReaderFast reader(this,&indexer);
		reader.Recovery = true;
		return Construct(&reader);
	}
	else
	{
		XmlMarkupConstructor constructor;
		XmlReaderFast reader(this,&constructor);
		reader.Recovery = true;
		return Construct(&reader);
	}

}

bool XmlDocument::Construct(XmlReader * reader)
{
	reader->Process(Resource);
	reader->Report();
	return reader->Failures == 0;
}

void XmlDocument::Destroy()
{
#ifdef REASON_USING_XML_STYLE
	if (Stylesheet)
	{
		delete Stylesheet;
		Stylesheet=0;
	}
#endif
	if (Index)
	{
		delete Index;
		Index=0;
	}

	if (Dtd)
	{
		delete Dtd;
		Dtd = 0;
	}

	DoctypeDeclaration = 0;
	Declaration = 0;

	Identifiers.Destroy();
	Namespaces.Destroy();

	XmlMarkup::Destroy();

	Initialise();
}

void XmlDocument::Release()
{
	OutputMessage("XmlDocument::Release - WARNING: Release is being used, XmlObjects will be unbound but not deleted.\n");

	if (Index)
	{
		delete Index;
		Index=0;
	}

	if (Dtd)
	{
		delete Dtd;
		Dtd = 0;
	}

	DoctypeDeclaration = 0;
	Declaration = 0;

	Identifiers.Destroy();
	Namespaces.Destroy();

	XmlMarkup::Release();

	Initialise();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlIndex::XmlIndex()
{

}

XmlIndex::~XmlIndex()
{
	Destroy();
}

void XmlIndex::Destroy()
{
	AttributeNameCatalogue.Destroy();
	AttributeValueCatalogue.Destroy();
	AttributeList.Release();

	ElementCatalogue.Destroy();
	ElementList.Release();

	TextCatalogue.Destroy();
	TextList.Release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity XmlCatalogue::Instance;

XmlCatalogue::XmlCatalogue()
{
	Index = new XmlIndex();
}

XmlCatalogue::~XmlCatalogue()
{

}

bool XmlCatalogue::Construct()
{
	if (!IsEmpty())
	{
		Destroy();
	}

	XmlMarkupConstructor constructor;
	XmlMarkupIndexer indexer(Index,&constructor);

	XmlReaderFast reader(this,&indexer);

	return XmlDocument::Construct(&reader);	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

