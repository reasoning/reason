
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

#include "reason/language/xml/dtd.h"
#include "reason/language/xml/dtdreader.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Language { namespace Xml {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 int XmlDtdObject::Hash()
{

	return Sequences::HashCaseless(Name.Data,Name.Size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity XmlEntityDeclaration::Instance;

XmlEntityDeclaration::XmlEntityDeclaration()
{

}

XmlEntityDeclaration::~XmlEntityDeclaration()
{

}

bool XmlEntityDeclaration::IsInternalEntity()
{

	return !IsExternalEntity();
}

bool XmlEntityDeclaration::IsExternalEntity()
{

	return EntityValue.IsEmpty() && ExternalId.Type != XmlExternalId::NONE;

}

bool XmlEntityDeclaration::IsParsedEntity()
{

	return NotationDataDeclaration.IsEmpty();
}

bool XmlEntityDeclaration::IsUnparsedEntity()
{

	return !NotationDataDeclaration.IsEmpty();
}

 int XmlEntityDeclaration::Hash()
{

	return Name.Hash();
}

 int XmlEntityDeclaration::Compare(Reason::System::Object *object, int comparitor)
{
	if (object->InstanceOf(this))
	{

		XmlEntityDeclaration *entity = (XmlEntityDeclaration *) object;

		if (comparitor == Comparable::COMPARE_INSTANCE)
		{
			return this == object;
		}
		else
		if (comparitor == Comparable::COMPARE_PRECISE)
		{
			return Name.Compare(&entity->Name,comparitor) + (Type - entity->Type);
		}
		else
		if (comparitor == Comparable::COMPARE_INSTANCE)
		{
			return Name.Compare(&entity->Name,comparitor);			
		}
	}
	else
	if (object->InstanceOf(Sequence::Instance))
	{

		return Name.Compare(object,comparitor);
	}

	OutputMessage("XmlEntityDeclaration::Compare - ERROR: Invalid object type for comparison.\n");
	return Identity::Error;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlDoctypeDeclaration::XmlDoctypeDeclaration():
	XmlDtdObject(XML_DTD_DOCTYPE_DECLARATION)
{

}

XmlDoctypeDeclaration::~XmlDoctypeDeclaration()
{

}

void XmlDoctypeDeclaration::Print(String & string)
{
	string << "<!DOCTYPE "<< Name << " ";

	ExternalId.Print(string);

	if (! Dtd.IsEmpty())
	{
		string << " [\n";	
		Dtd.Print(string);
		string << "\n]]";
	}

	string << ">";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlAttributeDeclaration::XmlAttributeDeclaration()
{

	Type = 0;
	DefaultDeclarationType = 0;

}

XmlAttributeDeclaration::~XmlAttributeDeclaration()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity XmlElementDeclaration::Instance;

XmlElementDeclaration::XmlElementDeclaration()
{

}

XmlElementDeclaration::~XmlElementDeclaration()
{

}

 int XmlElementDeclaration::Compare(Reason::System::Object *object, int comparitor)
{
	if (object->InstanceOf(this))
	{
		return Name.Compare(&((XmlElementDeclaration*)object)->Name,comparitor);
	}
	else
	if (object->InstanceOf(XmlObject::Instance))
	{
		return Name.Compare(&((XmlObject*)object)->Token,comparitor);
	}
	else
	if (object->InstanceOf(Sequence::Instance))
	{

		return Name.Compare(((Sequence*)object),comparitor);
	}
	else
	{
		OutputMessage("XmlElementDeclaration::Compare - ERROR: Invalid object type for comparison.\n");
		return Identity::Error;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity XmlNotationDeclaration::Instance;

XmlNotationDeclaration::XmlNotationDeclaration():
	XmlDtdObject(XML_DTD_NOTATION_DECLARATION)
{

}

XmlNotationDeclaration::~XmlNotationDeclaration()
{

}

 int XmlNotationDeclaration::Compare(Reason::System::Object *object, int comparitor)
{
	if (object->InstanceOf(this))
	{
		return Name.Compare(&((XmlNotationDeclaration*)object)->Name, comparitor);
	}
	else
	if (object->InstanceOf(Sequence::Instance))
	{

		return Name.Compare(((Sequence*)object), comparitor);
	}
	else
	{
		OutputMessage("XmlNotationDeclaration::Compare - ERROR: Invalid object type for comparison.\n");
		return Identity::Error;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity XmlAttributeListDeclaration::Instance;

XmlAttributeListDeclaration::XmlAttributeListDeclaration()
{

}

XmlAttributeListDeclaration::~XmlAttributeListDeclaration()
{

	XmlAttributeList::Destroy();
}

 int XmlAttributeListDeclaration::Compare(Reason::System::Object *object, int comparitor)
{
	if (object->InstanceOf(this))
	{
		return Name.Compare(&((XmlAttributeListDeclaration*)object)->Name,comparitor);
	}
	else
	if (object->InstanceOf(XmlObject::Instance))
	{

		return Name.Compare(&((XmlObject*)object)->Token,comparitor);
	}
	else
	if (object->InstanceOf(Sequence::Instance))
	{

		return Name.Compare(((Sequence*)object), comparitor);
	}
	else
	{
		OutputMessage("XmlAttributeListDeclaration::Compare - ERROR: Invalid object type for comparison.\n");
		return Identity::Error;
	}
}

XmlAttributeDeclaration * XmlAttributeListDeclaration::FindDeclaration(unsigned short type,XmlAttributeDeclaration *fromObject )
{
	XmlAttribute *object;

	if (fromObject)
		object = fromObject->Next;
	else
		object = First;

	while (object != 0)
	{
		if ( ((XmlAttributeDeclaration*)object)->Type.Is(type) )
			break;

		object = object->Next;
	}

	return (XmlAttributeDeclaration*) object;
}

XmlAttributeDeclaration * XmlAttributeListDeclaration::FindDefaultDeclaration(unsigned char type, XmlAttributeDeclaration *from)
{
	XmlAttribute*object;

	if (from)
		object = from->Next;
	else
		object = First;

	while (object != 0)
	{
		if ( ((XmlAttributeDeclaration*)object)->DefaultDeclarationType.Is(type) )
			break;

		object = object->Next;
	}

	return (XmlAttributeDeclaration*) object;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlDtd::XmlDtd()
{

}

XmlDtd::~XmlDtd()
{
	XmlMarkup::Destroy();

	AttributeListTable.Release();
	ElementTable.Release();
	EntityTable.Release();
	NotationTable.Release();

	Namespaces.Destroy();
}

bool XmlDtd::Download(const Url & url)
{
	Resource.Url.Construct(url);
	((Url&)url).Download(Resource);
	return Construct();
}

bool XmlDtd::Load(const File & file)
{
	((File&)file).Read(this->Resource);
	return Construct();
}

bool XmlDtd::Create(char * data, int size)
{
	Resource.Construct(data,size);
	return Construct();
}

bool XmlDtd::Construct()
{
	XmlMarkupBuilder builder;
	XmlDtdReader reader(this,&builder);
	reader.Process(Resource);
	reader.Report();
	return reader.Errors == 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

