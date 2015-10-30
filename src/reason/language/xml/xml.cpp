
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

#include "reason/network/uri.h"
#include "reason/network/http/http.h"
#include "reason/system/string.h"
#include "reason/language/xml/xml.h"
#include "reason/language/xml/dtd.h"
#include "reason/language/xml/enumerator.h"

#include <string.h>
#include <assert.h>
#include <stdio.h>		
#include <ctype.h>
#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::Language::Xml;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlElement::XmlElement():
	XmlObject(XML_NONE),Namespace(0)
{

}

XmlElement::XmlElement(unsigned int type):
	XmlObject(type),Namespace(0)
{

}

XmlElement::XmlElement(unsigned int type, Substring &token):
	XmlObject(type,token),Namespace(0)
{

}

XmlElement::~XmlElement()
{

	Attributes.Destroy();
}

void XmlElement::Print(String & string)
{
	string << "<";

	if (Type.Is(XML_END))
		string << "/";

	string << Token;

	XmlElement *element = (XmlElement*)this;
	for(element->Attributes.Forward();element->Attributes.Has();element->Attributes.Move())
	{
		XmlAttribute * attribute = element->Attributes();
		if (attribute->Value.IsNull())
		{
			string << " " << attribute->Name;
		}
		else
		{
			string << " " << attribute->Name << "=\"";
			attribute->Normalise(string);
			string << "\"";
		}
	}

	if (Type.Is(XML_EMPTY))	
		string << 	"/";

	string << ">";

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Reason::Structure::Objects::Array XmlElementAllocator::Allocator;

void * XmlElementAllocator::Allocate(unsigned int size)
{
	if (Allocator.Length()==0)
	{
		return ::operator new(size);
	}
	else
	{
		Reason::System::Object *object = Allocator[Allocator.Length()-1];
		Allocator.Remove(object);
		return object;
	}	
}

void XmlElementAllocator::Deallocate(void *object)
{
	Allocator.Append((Reason::System::Object*)object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void XmlDeclaration::Print(String & string)
{
	string << "<?xml ";

	if (!Version.IsEmpty())
		string << "version=\"" << Version << "\" ";

	if (!Encoding.IsEmpty())
		string << "encoding=\"" << Encoding << "\" ";

	if (!Standalone.IsEmpty())
		string << "standalone=\"" << Standalone << "\" ";

	string << "?>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void XmlProcessingInstruction::Print(String & string)
{
	string << "<?" << Token << " ";

	if (!Token.IsEmpty())
		string << Token;

	string << " ?>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void XmlStack::Debug()
{

	OutputMessage("\n------------------------------------------------------\n");
	OutputMessage("XmlStack:Debug - Dumping XML tags:\n\n");

	if (Top() == 0)
	{
		OutputMessage("XmlStack:Debug - Stack is empty...\n");
		return;
	}

	Reason::Structure::Objects::Entry * P = Top();
	while ( P != 0)
	{
		OutputMessage("Reason::System::Object [%08lX] - %s\n",P->Object,P->Object->Print());
		P = P->Prev;
	}

	OutputMessage("------------------------------------------------------\n");

}

XmlObject * XmlStack::Select(Sequence & id, unsigned int type)
{
	Reason::Structure::Objects::Entry * entry = SelectEntry(id,type);
	if (entry)
		return (XmlObject*)entry->Object;

	return 0;
}

XmlEntry * XmlStack::SelectEntry(Sequence &id,unsigned int type)
{

	XmlEntry * entry = (XmlEntry *) Top();
	while (entry != 0) 
	{
		if ( ((XmlObject *) entry->Object)->Type.Is(type) && id.Is(((XmlObject *)entry->Object)->Token,true) )
			break;

		entry = (XmlEntry *)entry->Prev;
	}

	return entry;
}

XmlEntry * XmlStack::SelectEntry(Sequence &id,unsigned int type, int depth, int count)
{

	XmlEntry *pointer = (XmlEntry*) Top();
	XmlEntry *current=0;

	while (pointer != 0) 
	{
		if ( ((XmlObject *) pointer->Object)->Type.Is(type) && id.Is(((XmlObject *)pointer->Object)->Token,true) )
		{

			if ( depth - (pointer->Depth) >= 0)
			{
				return pointer;
			}

		}

		pointer = (XmlEntry *)pointer->Prev;
	}

	return current;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity XmlObject::Instance;

void XmlObject::Initialise()
{
	Before	=0;
	After	=0;
	Child	=0;
	Parent	=0;
}

XmlObject::XmlObject():
	Before(0),After(0),Child(0),Parent(0),Type(0)
{

}

XmlObject::XmlObject(unsigned int type):
	Before(0),After(0),Child(0),Parent(0),Type(type)
{

}

XmlObject::XmlObject(unsigned int type, Substring &token):
	Before(0),After(0),Child(0),Parent(0),Type(type),Token(token)
{

}

XmlObject::~XmlObject()
{

}

void XmlObject::Evaluate(String & string, bool padded)
{
	if (Type.Is(XML_ELEMENT))
	{

		XmlNavigator e;
		e.NavigateDescendants(this);
		if (e.Forward())
		{
			while(true)
			{
				if (e.Pointer()->Type.Is(XML_TEXT))
				{
					e.Pointer()->Evaluate(string,padded);
					if (!e.Move()) break;
					if (padded)string.Append(" ");
				}
				else
				{
					if (!e.Move()) break;
				}
			}
		}
	}
}

void XmlObject::Normalise(String & string, bool padded)
{
	XmlNavigator e;
	e.NavigateDescendants(this);
	if (e.Forward())
	{
		while(true)
		{
			if (e.Pointer()->Type.Is(XML_TEXT))
			{
				e.Pointer()->Normalise(string,padded);
				if (!e.Move()) break;
				if (padded)string.Append(" ");
			}
			else
			{
				if (!e.Move()) break;
			}

		}
	}
}

void XmlObject::ExpandedName(String & name, String & uri)
{

	if (Type.Is(XML_ELEMENT))
	{
		XmlElement *element = (XmlElement*)this;

		name << element->Token;

		if (element->Namespace)
		{
			uri << element->Namespace->Resource;
		}
	}
	else
	if(Type.Is(XML_PROCESSING_INSTRUCTION))
	{

		name << Token;
	}
}

bool XmlObject::IsAncestorOf(XmlObject * object)
{

	if (object)
	{
		int ordinal = object->Ordinal();

		if (Type.Is(XML_START))
			return Ordinal() < ordinal && After->Ordinal() > ordinal;
		else
		if (Type.Is(XML_MARKUP))
			return ((XmlMarkup*)this)->First->Ordinal() < ordinal && ((XmlMarkup*)this)->Last->Ordinal() > ordinal;
	}

	return false;
}

bool XmlObject::IsDescendantOf(XmlObject * object)		
{

	if (object)
	{
		int ordinal = Ordinal();

		if(object->Type.Is(XML_START))
			return ordinal > object->Ordinal() && ordinal < object->After->Ordinal();
		else
		if (object->Type.Is(XML_MARKUP))
			return ordinal > ((XmlMarkup*)object)->First->Ordinal() && ordinal < ((XmlMarkup*)object)->Last->Ordinal();
	}

	return false;
}

int XmlObject::Ordinal()
{	

	if (Type.Is(XML_VIRTUAL))
	{
		if (Type.Is(XmlObject::XML_END))
			if (Child) return Child->Ordinal()+1;

		int ordinal=1;
		XmlObject * object = Preceding();
		while (object && object->Type.Is(XmlObject::XML_VIRTUAL))
		{
			++ordinal;
			object = object->Preceding();
		}

		if (object)
		{
			return ordinal + object->Ordinal();
		}
		else
		{
			OutputError("XmlObject::Ordinal - Ordinal value for specified object type could not be found.\n");
			return -1;
		}
	}
	else
	if (Type.Is(XML_MARKUP))
	{

		return ((XmlMarkup*)this)->First->Ordinal()-1;

	}
	else
	if (Type.Is(XML_TEXT) || Type.Is(XML_START) || Type.Is(XML_END) || Type.Is(XML_EMPTY) || Type.Is(XML_COMMENT) || Type.Is(XML_DECLARATION) || Type.Is(XML_CDATA) || Type.Is(XML_PROCESSING_INSTRUCTION) || Type.Is(XML_DTD_DOCTYPE_DECLARATION))
	{
		return (int)(long)Token.Data;
	}
	else
	{
		OutputError("XmlObject::Ordinal - Ordinal value for specified object type could not be found.\n");
		return -1;
	}

}

int XmlObject::Depth()
{
	int depth=0;
	XmlObject * parent = this;
	while (parent && parent->Parent)
	{
		++depth;
		parent = parent->Parent;
	}
	return depth;
}

int XmlObject::Compare(Reason::System::Object *object, int comparitor)
{
	if (this == object) return 0;

	if (object->InstanceOf(this))
	{
		if (comparitor == Comparable::COMPARE_INSTANCE)
		{
			return (int)((long)this - (long)object);
		}
		else
		{			
			XmlObject *xmlObject = (XmlObject *) object;
			int result = Token.Compare(&xmlObject->Token,comparitor);

			if (result == 0)
			{
				if (Type - xmlObject->Type == 0)
					return 0;
				else 
					return Type - xmlObject->Type;
			}
			else
			{
				return result;
			}
		}
	}
	else
	if (object->InstanceOf(Sequence::Instance))
	{
		return Token.Compare(object,comparitor);
	}
	else
	{
		OutputError("XmlObject::Compare - ERROR: Invalid object type for comparison.\n");
		return Identity::Error;
	}
}

int XmlObject::Hash()
{

	return Sequence::HashCaseless(Token.Data,Token.Size);
}

bool XmlObject::Contains(XmlObject *object)
{

	if (Type.Is(XML_START))
	{
		return object->Ordinal() > Ordinal() && object->Ordinal() < After->Ordinal();
	}
	else
	if (Type.Is(XML_END))
	{
		return object->Ordinal() < Ordinal() && object->Ordinal() > Before->Ordinal();
	}
	else
	{
		return false;
	}

}

void XmlObject::Print(String & string)
{

	string << "<" << Token << "/>";

}

void XmlObject::Debug()
{	
	OutputMessage("\n-------------------------------------------------------------------------\n");
	OutputMessage("XmlObject::Debug\n\n");

	OutputMessage("[%08lX] %s\n",this,Print());

	XmlNavigator e;
	e.NavigateDescendants(this);
	for (e.Forward();e.Pointer();e.Move())
	{
		OutputMessage("[%08lX] ",e.Pointer());

		for (int i=0;i<e.Depth()+1;++i)OutputMessage("   ");

		String print;

		OutputMessage("%s\n",e.Pointer()->Print());	

	}

	if (Type.Is(XML_START) && After)
	{
		OutputMessage("[%08lX] %s\n",After, After->Print());
	}

	OutputMessage("-------------------------------------------------------------------------\n");
}

void XmlObject::Release()
{
	if (((XmlObject*)this)->Type.Is(XmlObject::XML_START))
	{
		XmlObject *garbage = 0; 

		while (Child != 0)
		{
			garbage = Child;
			Child = Child->After;
			garbage->Release();
		}
	}

	Initialise();
}

void XmlObject::Destroy()
{

	if (((XmlObject*)this)->Type.Is(XmlObject::XML_START))
	{
		XmlObject * object = 0; 

		while (Child != 0)
		{
			object = Child;
			Child = Child->After;

			if (object->Child)
				object->Destroy();

			delete object;
		}
	}

}

void XmlObject::Copy(XmlObject *model)
{
	Before		= model->Before;
	After		= model->After;
	Parent		= model->Parent;
	Child		= model->Child;
}

void XmlObject::Validate()
{	
	#ifdef _DEBUG

		assert(After!=this);
		assert(Before!=this);
		assert(Parent!=this);
		assert(Child!=this);

	#endif

	if (Type.Is(XmlObject::XML_START))
	{
		if (After == 0)
		{
			OutputError("XmlObject::Validate - Start object [%08lX] is missing end object.\n",this);	
		}
		else
		if (After->Type.Is(XmlObject::XML_END))
		{
			if (!After->Token.Is(this->Token,true))
			{
				OutputError("XmlObject::Validate - Start object [%08lX] has different id to end object [%08lX].\n",this,After);	
			}

			if (After->Parent != Parent)
			{
				OutputError("XmlObject::Validate - Start object [%08lX] has different parent to end object [%08lX].\n",this,After);	
			}

			if (Child)
			{
				if (After->Child != LastChild())
				{
					OutputError("XmlObject::Validate - Start object last child [%08lX] does not match reverse child [%08lX] of end object .\n",LastChild(),After->Child);	
				}

				XmlObject *object = Child;
				while (object !=0)
				{
					if (object->Parent != this)
					{
						OutputError("XmlObject::Validate - Start object [%08lX] is not the parent of one of immediate descendant [%08lX].\n",this,object);						
					}

					object = object->After;
				}
			}
		}
		else
		{
			OutputError("XmlObject::Validate - Start object [%08lX] is incorrectly bound to empty object [%08lX].\n",this,After);	
		}
	}
	else
	if (Type.Is(XmlObject::XML_END))
	{
		if (Before == 0)
		{
			OutputError("XmlObject::Validate - End object [%08lX] is missing start object.\n",this);
		}
		else
		if (Before->Type.Is(XmlObject::XML_START))
		{
			if (!Before->Token.Is(this->Token,true))
			{
				OutputError("XmlObject::Validate - End object [%08lX] has different id to start object [%08lX].\n",this,Before);	
			}

			if (Before->Parent != Parent)
			{
				OutputError("XmlObject::Validate - End object [%08lX] has different parent to start object [%08lX].\n",this,Before);	
			}

			if (Child)
			{
				if (Before->LastChild() != Child)
				{
					OutputError("XmlObject::Validate - End object reverse child [%08lX] does not match last child [%08lX] of start object.\n",Child,Before->LastChild());	
				}
			}
		}
		else
		{
			OutputError("XmlObject::Validate - End object [%08lX] is incorrectly bound to empty object [%08lX].\n",this,Before);	
		}
	}
	else
	{
		if (Before && Before->Parent != Parent)
		{
			OutputError("XmlObject::Validate - Empty object [%08lX] has different parent to object before [%08lX]\n",this,Before);
		}

		if (Child != 0)
		{
			OutputError("XmlObject::Validate - Empty object [%08lX] has child.\n",this);
		}
	}
}

void XmlObject::InsertParent(XmlObject * object)
{

	OutputAssert(object->Child == 0);
	OutputAssert(object->Parent == 0);

	if (Before)
	{
		XmlObject * before = Before;
		XmlObject * after = before->DetachAfter();
		object->AttachChild(after);
		before->AttachAfter(object);
	}
	else
	if (Parent)
	{	
		XmlObject * parent = Parent;
		XmlObject * child = parent->DetachChild();		
		object->AttachChild(child);
		parent->AttachChild(object);
	}
	else
	{
		AttachParent(object);
	}
}

void XmlObject::AttachParent(XmlObject * object)
{
	if (Parent)
	{
		OutputError("XmlObject::AttachParent - Attempt to attach parent, but parent already exists.\n");
		return;
	}

	if (Before)
	{
		OutputError("XmlObject::AttachParent - Cannot attach parent to non first sibling.\n");
		return;
	}

	if (!Type.Is(XmlObject::XML_END))
	{
		OutputAssert(object->Type.Is(XmlObject::XML_START)==true);
		OutputAssert(object->Child==0);

		Parent = object;
		Parent->Child = this;

		if (Parent->After)
			Parent->After->Child = LastSibling();

		object = After;
		while (object)
		{
			object->Parent = Parent;
			object = object->After;
		}
	}
	else
	{
		OutputError("XmlObject::AttachParent - Cannot attach parent to end object.\n");
	}
}

XmlObject * XmlObject::DetachParent()
{
	if (Before)
	{
		OutputError("XmlObject::DetachParent - Cannot detach parent from non first sibling.\n");
		return 0;
	}

	if (Parent)
	{
		XmlObject * parent = Parent;
		Parent->Child = 0;

		if (Parent->After)
			Parent->After->Child = 0;

		Parent = 0;

		XmlObject *object = After;
		while (object != 0)
		{	
			object->Parent = 0;
			object = object->After;
		}

		return parent;
	}
	else
	{
		return 0;
	}
}

void XmlObject::InsertBefore(XmlObject *object)
{
	if (Before)
	{
		XmlObject *before = DetachBefore();
		object->FirstSibling()->AttachBefore(before);
	}

	AttachBefore(object);
}

void XmlObject::AttachBefore(XmlObject *object)
{
	if (Before)
	{
		OutputError("XmlObject::AttachBefore - Attempt to attach object before, but before already exists.\n");
		return;
	}

	if (Type.Is(XmlObject::XML_END))
	{
		OutputAssert(object->Type.Is(XmlObject::XML_START)==true);
		OutputAssert(Child == 0);

		if (object->Child)
		{
			Child = object->Child->LastSibling();
		}

	}

	Before = object;
	object->After = this;

	while (object)
	{
		object->Parent = Parent;
		object = object->Before;
	}

	if (Parent)
	{
		Parent->Child = FirstSibling();
	}
}

XmlObject * XmlObject::DetachBefore()
{
	if (Before)
	{
		XmlObject *before = Before;
		Before->After = 0;
		Before = 0;

		if (Parent)
		{
			Parent->Child = this;
		}

		XmlObject *object = before;
		while (object)
		{
			object->Parent=0;
			object = object->Before;
		}

		return before;
	}
	else
	{
		return 0;
	}
}

void XmlObject::InsertAfter(XmlObject *object)
{
	if (After)
	{
		XmlObject *after = DetachAfter();
		object->LastSibling()->AttachAfter(after);
	}

	AttachAfter(object);
}

void XmlObject::Attach()
{

	if (Type.Is(XmlObject::XML_START))
	{
		if (After)
		{
			if (!Child)
				Child = After->FirstChild();
		}
	}
	else
	if (Type.Is(XmlObject::XML_END))
	{
		if (Before)
		{
			if (!Child)
				Child = Before->LastChild();
		}
	}

	if (!Parent)
	{
		if (Before)
			Parent = Before->Parent;
		else
		if (After)
			Parent = After->Parent;
	}
}

void XmlObject::AttachAfter(XmlObject *object)
{
	if (After)
	{
		OutputError("XmlObject::AttachAfter - Attempt to attach object after, but after already exists.\n");
		return;
	}

	if ( Type.Is(XmlObject::XML_START) )
	{
		OutputAssert(object->Type.Is(XmlObject::XML_END)==true);
		OutputAssert(object->Child == 0);

		if (Child)
		{
			object->Child = Child->LastSibling();
		}
	}

	After = object;
	object->Before = this;

	while (object != 0)
	{	
		object->Parent = Parent;
		object = object->After;
	}

	if (Parent && Parent->After)
	{

		Parent->After->Child = LastSibling();
	}
}

XmlObject * XmlObject::Detach()
{

	return 0;
}

XmlObject * XmlObject::DetachAfter()
{
	if (After)
	{
		XmlObject *after = After;

		After->Before = 0;
		After = 0;

		if (Parent && Parent->After)
		{
			Parent->After->Child = this;
		}

		XmlObject *object = after;
		while (object)
		{
			object->Parent=0;
			object = object->After;
		}

		return after;
	}
	else
	{
		return 0;
	}
}

void XmlObject::InsertChild(XmlObject *object)
{
	if (Child)
	{
		XmlObject *child = DetachChild();
		object->LastSibling()->AttachAfter(child);
	}

	AttachChild(object);
}

void XmlObject::AttachChild(XmlObject *object)
{
	if ( ! Type.Is(XmlObject::XML_START))
	{
		OutputError("XmlObject::AttachChild - Cannot attach child to non start object.\n");
		return;
	}

	if (Child)
	{
		OutputError("XmlObject::AttachChild - Child already exists.\n");
		return;
	}

	OutputAssert(object->Parent == 0);

	Child = object;
	while (object != 0)
	{	
		object->Parent = this;
		object = object->After;
	}	

	if (After)
		After->Child = Child->LastSibling();
}

XmlObject * XmlObject::DetachChild()
{
	if (Type.Is(XmlObject::XML_START))
	{
		XmlObject *child = Child;
		if (child)
		{	
			Child = 0;

			XmlObject *object = child;
			while (object != 0)
			{	
				object->Parent = 0;
				object = object->After;
			}		

			if (After)
				After->Child = 0;
		}

		return child;
	}
	else
	{
		OutputError("XmlObject::DetachChild - Cannot detach child from non start object.\n");
		return 0;
	}
}

XmlObject * XmlObject::Root()
{
	XmlObject * root = this;
	while (root->Parent != 0)
		root = root->Parent;
	return root;
}

#ifdef REASON_USING_XML_STYLE
XmlStyle * XmlObject::Style()
{

	XmlStyle * style = 0;

	if (this->Root()->InstanceOf(XmlDocument::Instance))
	{
		XmlDocument * document = (XmlDocument*)this->Root();		

		if (!document->Stylesheet)
		{

			document->Stylesheet = new XmlStylesheet(document->Resource.Resolver);
		}

		style = document->Stylesheet->Map[this];

		if (!style)
		{
			style = new XmlStyle(this);

			Iterand< Mapped<XmlObject *,XmlStyle*> > iterand = document->Stylesheet->Map.Insert(this,style);
			OutputAssert(iterand != 0);
		}
	}

	return style;
}
#endif

XmlObject *XmlObject::FirstSibling()
{
	XmlObject * before = 0; 
	XmlObject * object = (XmlObject*)this; 

	if (object)
	{
		do
		{
			before = object;
		}
		while ((object = object->Before) != 0);
	}

	return before;
}

XmlObject *XmlObject::LastSibling()
{
	XmlObject *after = 0; 
	XmlObject *object = (XmlObject*)this; 

	if (object)
	{
		do
		{
			after = object;
		}
		while ((object = object->After) != 0);
	}

	return after;
}

XmlObject *XmlObject::PrecedingSibling()
{
	return Before;
}

XmlObject * XmlObject::Preceding()
{
	if (Type.Is(XML_END) && Child)
	{
		return Child;
	}
	else
	if (Before)
	{
		return Before;
	}
	else
	{
		return Parent;
	}
}

XmlObject *XmlObject::FollowingSibling()
{
	return After;
}

XmlObject * XmlObject::Following()
{
	if (Type.Is(XML_START) && Child)
	{
		return Child;
	}
	else
	if (After)
	{
		return After;
	}
	else
	{
		#ifdef _DEBUG

		XmlObject *parent = Parent;
		while (parent && parent->After==0)
		{
			parent = parent->Parent;
		}

		if (parent)
			return parent->After;

		#else

        return Parent->After;

		#endif		
	}

	return 0;
}

XmlObject *XmlObject::FirstChild()
{

	if (Child)
	{

		return Child->FirstSibling();
	}
	return 0;
}

XmlObject *XmlObject::LastChild()
{
	if (Child)
	{

		return Child->LastSibling();
	}

	return 0;

}

Substring XmlObject::Resource()
{
	char * from=0;
	char * to=0;

	if (Type.Is(XmlObject::XML_BINARY))
	{

		if (Type.Is(XmlObject::XML_START) && After)
		{
			from = Token.Data-1;
			if (After->Token.Data != Token.Data)
			{
				to = After->Token.Data+After->Token.Size+1;	
			}
			else
			if (After->After)
			{
				to = After->After->Token.Data-1;
			}
		}
		else
		if (Type.Is(XmlObject::XML_END) && Before)
		{
			from = Before->Token.Data-1;

			if (Before->Token.Data != Token.Data)
			{
				to = Token.Data+Token.Size+1;
			}
			else
			if (After)
			{
				to = After->Token.Data-1;
			}
		}
	}
	else
	if (Type.Is(XmlObject::XML_UNARY))
	{

		from = Token.Data-1;
		to = Token.Data+Token.Size+2;
	}
	else
	{

		from = Token.Data;
		to = Token.Data+Token.Size;
	}

	if (from && to)
	{
		return Substring(from,to-from);
	}
	else
	{

		OutputWarning("XmlObject::Substring - Cannot construct substring from xml.\n");

		return Substring();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 XmlObject *XmlFactory::CreateElement()
{

	return 0;
}

 XmlObject *XmlFactory::CreateStartElement()
{
	XmlElement *element = new XmlElement();
	element->Type.Assign(XmlObject::XML_START);
	element->Type.On(XmlObject::XML_VIRTUAL);
	return element;

}

 XmlObject *XmlFactory::CreateEndElement()
{
	XmlElement *element = new XmlElement();
	element->Type.Assign(XmlObject::XML_END);
	element->Type.On(XmlObject::XML_VIRTUAL);
	return element;

}

 XmlObject *XmlFactory::CreateEmptyElement()
{
	XmlElement *element = new XmlElement();
	element->Type.Assign(XmlObject::XML_EMPTY);
	element->Type.On(XmlObject::XML_VIRTUAL);
	return element;
}

 XmlObject *XmlFactory::CreateComment()
{
	XmlComment * comment = new XmlComment();
	comment->Type.On(XmlObject::XML_VIRTUAL);
	return comment;
}

 XmlObject *XmlFactory::CreateText()
{
	XmlText * text = new XmlText();
	text->Type.On(XmlObject::XML_VIRTUAL);
	return text;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Reason::Structure::Objects::Entry * XmlCollection::Select(Sequence &name,unsigned int type)
{
	if (name.IsEmpty())
		return 0;

	Reason::Structure::Objects::Entry * P = Last;

	while (P != 0 ) 
	{
		if (((XmlObject *)P->Object)->Token.Is(name,true) && (((XmlObject *)P->Object)->Type.Is(type)))
			break;

		P = P->Prev;
	}

	return P;
}

Reason::Structure::Objects::Entry * XmlCollection::Select(XmlObject *object)
{

	Reason::Structure::Objects::Entry * entry = First;
	while(entry != 0)
	{
		if ( ((XmlObject *)entry->Object) == object)
			break;

		entry = entry->Next;
	}
	return entry;
}

void XmlCollection::ExtractText(Path &output)
{

	Reason::Structure::Objects::Entry *entry = First;
	while (entry != 0)
	{
		if (((XmlObject *)entry->Object)->Type.Is(XmlObject::XML_TEXT))
		{
			output.Append(new Path(((XmlText *)(entry->Object))->Token));
		}

		entry = entry->Next;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity XmlAttribute::Instance;

XmlAttribute::XmlAttribute():
	Type(ATTRIBUTE),Next(0),Prev(0),Namespace(0),Parent(0)
{

}

XmlAttribute::XmlAttribute(XmlObject * parent):
	Type(ATTRIBUTE),Next(0),Prev(0),Namespace(0),Parent(parent)
{

}

XmlAttribute::~XmlAttribute() 
{

}	

bool XmlAttribute::IsNamespaceDefaultDeclaration()
{
	return Type.Is(NAMESPACE_DEFAULT_DECLARATION);
}

bool XmlAttribute::IsNamespaceDeclaration()
{
	return Type.Is(NAMESPACE_DECLARATION);
}

bool XmlAttribute::IsNamespaceQualified()
{
	return Namespace != 0;
}

void XmlAttribute::Evaluate(String & string)
{

	Normalise(string);
}

void XmlAttribute::Normalise(String & string)
{

	String normal;
	normal << Value;

	normal.Trim();
	normal.Replace('\r',' ');
	normal.Replace('\n',' ');
	normal.Replace('\t',' ');
	normal.Replace("  "," "); 

	XmlEntity::Decode(normal);

	string << normal;
}

void XmlAttribute::ExpandedName(String &name,String &uri)
{

	name <<Name;

	if (Namespace)
	{
		uri << Namespace->Resource;
	}
}

void XmlAttribute::Print(String & string)
{
	string << " " << Name << "=\"" << Value << "\"";
}

int XmlAttribute::Hash()
{
	return Name.Hash();
}

int XmlAttribute::Compare(Reason::System::Object *object, int comparitor)
{

	if (object->InstanceOf(this))
	{
		int dif = Name.Compare(&((XmlAttribute*)object)->Name,comparitor);
		if (dif == 0)
			return Value.Compare(&((XmlAttribute*)object)->Value,comparitor);
		else
			return dif;
	}
	else
	if (object->InstanceOf(String::Instance))
	{
		return Name.Compare(object,comparitor);
	}
	else
	{
		OutputMessage("XmlAttribute::Compare - ERROR: Invalid object type for comparison.\n");
		return Identity::Error;
	}
}

void XmlAttribute::Release()
{
	Next =0;
	Prev =0;
	Name.Release();
	Value.Release();
}

void XmlAttribute::Destroy()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Reason::Structure::Objects::Array XmlAttributeAllocator::Allocator;

void * XmlAttributeAllocator::Allocate(unsigned int size)
{
	if (Allocator.Length()==0)
	{
		return ::operator new(size);
	}
	else
	{
		Reason::System::Object *object = Allocator[Allocator.Length()-1];
		Allocator.Remove(object);
		return object;
	}	
}

void XmlAttributeAllocator::Deallocate(void *object)
{
	Allocator.Append((Reason::System::Object*)object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlAttributeCollection::XmlAttributeCollection()
{
}

XmlAttributeCollection::~XmlAttributeCollection()
{
}

void XmlAttributeCollection::Print(String & string)
{

	if (string.Remaining() < 64)
		string.Reserve(64);

	Reason::Structure::Objects::Entry *P = First;
	while (P != 0)
	{	
		string << " " << ((XmlAttribute*)P->Object)->Name << "=\"" << ((XmlAttribute*)P->Object)->Value << "\"";
		P = P->Next;
	}

}

Reason::System::Object *XmlAttributeCollection::Select(const char *name, bool caseless)
{
	if (name == 0)
		return 0;

	Reason::Structure::Objects::Entry *P = First;
	while ( P != 0 )
	{
		if ( ((XmlAttribute*)P->Object)->Name.Is(name,caseless) )
			return P->Object;

		P = P->Next;
	}

	return 0; 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlAttributeList::XmlAttributeList():
Count(0),First(0),Last(0),EnumeratorIndex(0),Enumerator(0),EnumeratorDirection(0)
{

}

XmlAttributeList::~XmlAttributeList()
{

	Release();
}

void XmlAttributeList::Print(String & string)
{

	if (string.Remaining() < 64)
		string.Reserve(64);

	XmlAttribute *P = First;
	while (P != 0)
	{	
		string << " " << P->Name << "=\"" << P->Value << "\"";
		P = P->Next;
	}

}

void XmlAttributeList::Prepend(XmlAttribute *attribute)
{
	if (First == 0)
	{
		First = Last = attribute;
	}
	else
	{
		attribute->Next = First;
		First->Prev = attribute;
		First = attribute;
	}

	++Count;
}

void XmlAttributeList::Append(XmlAttribute *attribute)
{
	if (First == 0)
	{
		First = Last = attribute;
	}
	else
	{
		attribute->Prev	= Last;
		Last->Next		= attribute;
		Last = attribute;
	}

	++Count;
}

void XmlAttributeList::Remove(XmlAttribute *attribute)
{
	if (Count == 1)
	{
		First	=0;
		Last	=0;
	}
	else
	if (attribute == First)
	{
		First->Next->Prev = 0;
		First = First->Next;
	}
	else
	if (attribute == Last)
	{
		Last->Prev->Next = 0;
		Last = Last->Prev;
	}
	else
	{
		attribute->Next->Prev = attribute->Prev;
		attribute->Prev->Next = attribute->Next;
	}

	attribute->Prev = 0;
	attribute->Next = 0;

	--Count;
}

void XmlAttributeList::Delete(XmlAttribute *attribute)
{
	if (Count == 1)
	{
		First	=0;
		Last	=0;
	}
	else
	if (attribute == First)
	{
		if (First->Next)
			First->Next->Prev = 0;

		First = First->Next;
	}
	else
	if (attribute == Last)
	{
		Last->Prev->Next = 0;
		Last = Last->Prev;
	}
	else
	{
		attribute->Next->Prev = attribute->Prev;
		attribute->Prev->Next = attribute->Next;
	}

	delete attribute;

	--Count;
}

XmlAttribute *XmlAttributeList::Select(Reason::System::Object *name, Reason::System::Object *value, int comparitor)
{
	XmlAttribute *attribute = First;
	while (attribute != 0)
	{
		if (attribute->Name.Compare(name,comparitor)==0 && attribute->Value.Compare(value)==0)
			return attribute;

		attribute = attribute->Next;
	}

	return 0;
}

XmlAttribute *XmlAttributeList::Select(Reason::System::Object *name, int comparitor)
{
	XmlAttribute *attribute = First;
	while (attribute != 0)
	{
		if (attribute->Compare(name,comparitor)==0)
			return attribute;

		attribute = attribute->Next;
	}

	return 0;
}

XmlAttribute * XmlAttributeList::Select(const char *name, bool caseless)
{
	if (name == 0)
		return 0;

	XmlAttribute *attribute = First;
	while ( attribute != 0 )
	{
		if (attribute->Name.Is(name,caseless))
			return attribute;

		attribute = attribute->Next;
	}

	return 0; 
}

XmlAttribute * XmlAttributeList::Select(const char *name, const char * value, bool caseless)
{
	if (name == 0)
		return 0;

	XmlAttribute *attribute = First;
	while ( attribute != 0 )
	{
		if (attribute->Name.Is(name,caseless) && attribute->Value.Is(value,caseless))
			return attribute;

		attribute = attribute->Next;
	}

	return 0; 
}

void XmlAttributeList::Union(XmlAttributeList &list)
{

	XmlAttribute *attribute = list.First;

	while (attribute != 0)
	{
		if (Select(attribute,COMPARE_INSTANCE)==0)
		{
			Append(attribute);
		}

		attribute = attribute->Next;
	}
}

void XmlAttributeList::Release()
{
	XmlAttribute *attribute = First;
	while (attribute != 0)
	{
		attribute = attribute->Next;
		First->Release();
		delete First;
		First = attribute;
	}

	Count = 0;
	First = 0;
	Last  = 0;
}

void XmlAttributeList::Destroy()
{
	XmlAttribute *attribute = First;
	while (attribute != 0)
	{
		attribute = attribute->Next;
		First->Destroy();
		delete First;
		First = attribute;
	}

	Count = 0;
	First = 0;
	Last  = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity XmlId::Instance;

XmlId::XmlId(XmlElement *element, XmlAttribute *attribute)
{

	Element	= element;
	Attribute	= attribute;
}

XmlId::XmlId()
{

	Element	=0;
	Attribute	=0;
}

XmlId::~XmlId()
{

}

int XmlId::Compare(Reason::System::Object *object, int comparitor)
{

	if (object->InstanceOf(this))
	{
		return Id().Compare(&((XmlId*)object)->Id(),comparitor);
	}
	else
	if (object->InstanceOf(String::Instance))
	{
		return Id().Compare(object,comparitor);
	}
	else
	{
		OutputError("XmlId::Compare - Invalid object type for comparison.\n");
		return Identity::Error;
	}
}

int XmlId::Hash()
{

	return Id().Hash();
}

Sequence & XmlId::Id()
{
	return Attribute->Value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity XmlResource::Instance;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity XmlNamespace::Instance;

XmlNamespace::XmlNamespace(const char *prefix, const char *resource)
{
	Prefix.Assign((char*)prefix);
	Resource.Assign((char*)resource);

	DeclarationAttribute = 0;
	DeclarationElement = 0;
}

XmlNamespace::XmlNamespace()
{
	DeclarationAttribute = 0;
	DeclarationElement = 0;
}

XmlNamespace::~XmlNamespace()
{
	DeclarationAttribute = 0;
	DeclarationElement = 0;
}

 int XmlNamespace::Compare(Reason::System::Object *object, int comparitor)
{

	if (object->InstanceOf(this))
	{
		return Prefix.Compare(&((XmlNamespace*)object)->Prefix,comparitor);
	}
	else
	if (object->InstanceOf(String::Instance))
	{
		return Prefix.Compare(object,comparitor);
	}
	else
	{
		OutputMessage("XmlId::Compare - ERROR: Invalid object type for comparison.\n");
		return Identity::Error;
	}
}

 int XmlNamespace::Hash()
{

	int hash = 0;
	char *c = Prefix.PointerAt(0);
	char *e = Prefix.PointerAt(Prefix.Size-1);
	while (c != e)
	{
		hash += *c & 0x1F;
		++c;
	}

	return hash;
}

void XmlNamespace::Evaluate(String & string)
{

}

void XmlNamespace::ExpandedName(String & name, String & uri)
{

	name << Prefix;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlNamespace XmlNamespaceScope::NamespaceXsi("xsi","http://www.w3.org/1999/XMLSchema-instance");
XmlNamespace XmlNamespaceScope::NamespaceXsd("xsd","http://www.w3.org/1999/XMLSchema");
XmlNamespace XmlNamespaceScope::NamespaceXmlns("xmlns","http://www.w3.org/TR/REC-xml");

XmlNamespaceScope::XmlNamespaceScope()
{

	Push(&NamespaceXsi);
	Push(&NamespaceXsd);
	Push(&NamespaceXmlns);
}

XmlNamespaceScope::~XmlNamespaceScope()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlExternalId::XmlExternalId():
	Type(NONE)
{

}

XmlExternalId::~XmlExternalId()
{

}

void XmlExternalId::Print(String & string)
{

	if (Type == PUBLIC)
	{
		string << "PUBLIC \"" << PublicIdLiteral << "\"";
		if (!SystemLiteral.IsEmpty())
			string << " \"" << SystemLiteral << "\" ";
	}
	else
	if (Type == SYSTEM)
	{
		string << "SYSTEM \"" << SystemLiteral << "\"";
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlEntity::XmlEntity(XmlEntityDeclaration *declaration)
{
	Declaration=declaration;
}

XmlEntity::~XmlEntity()
{

}

 int XmlEntity::Hash()
{
	return Declaration->Name.Hash();
}

 int XmlEntity::Compare(Reason::System::Object *object, int comparitor)
{
	return Declaration->Name.Compare(&((XmlEntity*) object)->Declaration->Name,comparitor);
}

void XmlEntity::Encode(String & string)
{

	string.Replace("&","&amp;");
	string.Replace("<","&lt;");
	string.Replace(">","&gt;");
	string.Replace("'","&apos;");
	string.Replace("\"","&quot;");
}

void XmlEntity::Decode(String & string)
{

	string.Replace("&amp;","&");
	string.Replace("&lt;","<");
	string.Replace("&gt;",">");
	string.Replace("&apos;","'");
	string.Replace("&quot;","\"");

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlComment::XmlComment():
	XmlObject(XML_COMMENT)
{
}

XmlComment::XmlComment(Substring &comment):
	XmlObject(XML_COMMENT,comment)
{
}

int XmlComment::Compare(Reason::System::Object *object, int comparitor)
{

	if ( !comparitor && object->InstanceOf(Sequence::Instance) && ((Sequence*)object)->Is("#PCDATA") )
	{
		return 0;
	}

	return XmlObject::Compare(object,comparitor);
}

void XmlComment::Normalise(String & string)
{
	String normal;
	normal << Token;
	normal.Trim();
	normal.Replace('\r',' ');
	normal.Replace('\n',' ');
	normal.Replace('\t',' ');
	normal.Replace("  "," "); 
	string << normal;

}

void XmlComment::Abstract(String & string)
{
	String abstract;
	Normalise(abstract);

	if (abstract.Size < 50)
	{
		string.Append(abstract.Data,abstract.Size);
	}
	else
	{
		string.Append(abstract.Data,47);
		string << "...";
	}
}

void XmlComment::Print(String & string)
{
	string << "<!-- ";
	#ifdef _DEBUG
		((XmlComment *)this)->Abstract(string);
	#else
		((XmlComment *)this)->Normalise(string);
	#endif
	string << " -->";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Reason::Structure::Objects::Array XmlCommentAllocator::Allocator;

void * XmlCommentAllocator::Allocate(unsigned int size)
{
	if (Allocator.Length()==0)
	{
		return ::operator new(size);
	}
	else
	{
		Reason::System::Object *object = Allocator[Allocator.Length()-1];
		Allocator.Remove(object);
		return object;
	}	
}

void XmlCommentAllocator::Deallocate(void *object)
{
	Allocator.Append((Reason::System::Object*)object);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void XmlCdata::Print(String & string)
{
	string << "<![CDATA[\n";	
	string << Token;
	string << "\n]]>";
}

void XmlCdata::Normalise(String &string)
{
	String normal;
	normal << Token;

	if (Type.Is(XmlObject::XML_CDATA))
	{
		normal.Replace("&","&amp;");
		normal.Replace("<","&lt;");
	}

	string << normal;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void XmlIncludeSection::Print(String & string)
{
	string << "<![INCLUDE[\n";	
	string << Token;
	string << "\n]]>";

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void XmlIgnoreSection::Print(String & string)
{
	string << "<![IGNORE[\n";	
	string << Token;
	string << "\n]]>";

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlText::XmlText():
	XmlObject(XML_TEXT)
{

}

XmlText::XmlText(unsigned int type):
	XmlObject(type)
{

}

XmlText::XmlText(Substring &text):
	XmlObject(XML_TEXT,text)
{

}

int XmlText::Compare(Reason::System::Object *object, int comparitor)
{

	if ( !comparitor && object->InstanceOf(Sequence::Instance) && ((Sequence*)object)->Is("#PCDATA") )
	{
		return 0;
	}

	return XmlObject::Compare(object,comparitor);
}

void XmlText::Evaluate(String & string, bool padded)
{
	string << Token;
}

void XmlText::Normalise(String &string, bool padded)
{
	String normal;
	normal << Token;
	normal.Trim();
	normal.Replace('\r',' ');
	normal.Replace('\n',' ');
	normal.Replace('\t',' ');

	int size=normal.Size+1;
	while (size > normal.Size)
	{

		size = normal.Size;
		normal.Replace("  "," ");
	}

	normal.Replace("<","&lt;");

	string << normal;
}

void XmlText::Abstract(String &string)
{
	String abstract;
	Normalise(abstract);

	if (abstract.Size < 100)
	{
		string.Append(abstract.Data,abstract.Size);
	}
	else
	{
		string.Append(abstract.Data,77);
		string << "...";
	}
}

void XmlText::Print(String & string)
{

		Normalise(string);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Reason::Structure::Objects::Array XmlTextAllocator::Allocator;

void * XmlTextAllocator::Allocate(unsigned int size)
{
	if (Allocator.Length()==0)
	{
		return ::operator new(size);
	}
	else
	{
		Reason::System::Object *object = Allocator[Allocator.Length()-1];
		Allocator.Remove(object);
		return object;
	}
}

void XmlTextAllocator::Deallocate(void *object)
{
	Allocator.Append((Reason::System::Object*)object);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

XmlVector::XmlVector()
{
	Buffer.Allocate(16);
}

void XmlVector::Append(XmlObject *object)
{
	Model mask;

	if (object->Child != 0)		mask.bit.child = 1;
	if (object->Parent != 0)		mask.bit.parent = 1;
	if (object->After != 0)		mask.bit.after = 1;
	if (object->Before != 0)		mask.bit.before = 1;

	if (((Buffer.ReferenceAt(0)<<4)&0xFF) == 0)
	{
		*Buffer.PointerAt(Buffer.Size-1) |= mask.byte;
	}
	else
	{
		Buffer << (char) (mask.byte<<4);
	}
}

void XmlVector::Create(XmlObject *origin, XmlObject *destination)
{

	XmlNavigator e;
	e.Navigate(origin);

	for (e.Forward();e.Has();e.Move())
	{
		Append(e.Pointer());
		if (e.Pointer() == destination)
			break;
	}

}

XmlObject * XmlVector::Follow(XmlObject *object)
{

	for (int i=0;i<Buffer.Size;++i)
	{

	}

	return 0;
}

bool XmlVector::Match(XmlObject *object, Model mask)
{

	if ( !(object->Child && mask.bit.child == 1) )
		return false;

	if ( !(object->Parent && mask.bit.parent == 1) )
		return false;

	if ( !(object->After && mask.bit.after == 1) )
		return false;

	if ( !(object->Before && mask.bit.before == 1) )
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlTokeniser::XmlTokeniser()
{
}

XmlTokeniser::XmlTokeniser(Sequence & sequence):Substring(sequence)
{
}

void XmlTokeniser::Tokenise()
{
	StringParser parser;
	parser.Assign(*this);

	while (! parser.Eof())
	{

		if (parser.Is('<'))
		{
			parser.Mark();
			while(!parser.Eof() && !parser.Is('>'))
				parser.Next();
			if (!parser.Eof()) parser.Next();
			parser.Trap();
			Objects.Append(parser.Token);
		}
		else
		if (parser.Is('&'))
		{
			parser.Mark();
			while(!parser.Eof() && !parser.Is(';'))
				parser.Next();
			if (!parser.Eof()) parser.Next();
			parser.Trap();

			Entities.Append(parser.Token);
		}
		else
		if (parser.ParseWord())
		{
			Words.Append(parser.Token);
		}
		else
		if (parser.ParseNumber())
		{
			Numbers.Append(parser.Token);
		}
		else
		if (parser.ParseSymbol())
		{
			Symbols.Append(parser.Token);
		}
		else
		{

			if (!parser.Eof())
				parser.Next();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

