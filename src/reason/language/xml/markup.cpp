
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
#include "reason/language/xml/markup.h"
#include "reason/language/xml/enumerator.h"
#include "reason/language/xml/dtd.h"
#include "reason/language/xml/document.h"

#ifdef REASON_PLATFORM_POSIX
#include <stdlib.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Language { namespace Xml {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity XmlMarkup::Instance;

XmlMarkup::XmlMarkup():
	XmlObject(XmlObject::XML_MARKUP),First(0),Last(0),Count(0)
{

}

XmlMarkup::XmlMarkup(XmlObject *object):
	XmlObject(XmlObject::XML_MARKUP),First(0),Last(0),Count(0)
{

	Assign(object);
}

XmlMarkup::XmlMarkup(XmlObject *first,XmlObject *last):
	XmlObject(XmlObject::XML_MARKUP),First(first),Last(last),Count(0)
{

}

XmlMarkup::~XmlMarkup()
{

}

void XmlMarkup::Assign(XmlObject *first,XmlObject *last)
{
	First = first;
	Last = last;
}

void XmlMarkup::Assign(XmlObject * object)
{
	if (!object) return;

	if (object->Type.Is(XmlObject::XML_MARKUP))
	{
		Assign((XmlMarkup*)object);
	}
	else
	if (object->Type.Is(XmlObject::XML_START))
	{
		First = object;
		Last = object->After;
	}
	else
	if (object->Type.Is(XmlObject::XML_END))
	{
		First = object->Before;
		Last = object;
	}
	else
	{
		First = object;
		Last = object;
	}
}

void XmlMarkup::Assign(XmlMarkup * markup)
{
	First = markup->First;
	Last = markup->Last;
}

void XmlMarkup::Index(Reason::Structure::Index<XmlObject*> &index)
{
	XmlNavigator e;
	e.Navigate(this);

	for (e.Forward();e.Has();e.Move())
	{
		index.Attach(e.Pointer(),e()->Token);
	}
}

bool XmlMarkup::Contains(XmlObject * object)
{

	if (object->Type.Is(XmlObject::XML_MARKUP))
	{

		XmlMarkup * markup = (XmlMarkup*) object;
		return (markup->First && markup->Last) && (markup->First->Ordinal() >= First->Ordinal() && markup->Last->Ordinal() <= Last->Ordinal());
	}
	else

	{

		int ordinal = object->Ordinal();
		return ordinal >= First->Ordinal() && ordinal <= Last->Ordinal();
	}

}

void XmlMarkup::Evaluate(String & string, bool padded)
{
	if (Type.Is(XML_MARKUP))
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
					if (padded) string.Append(" ");
				}
				else
				{
					if (!e.Move()) break;
				}
			}
		}
	}	
}

int XmlMarkup::Compare(Reason::System::Object *object, int comparitor)
{
	if (object->InstanceOf(this))
	{

		XmlMarkup *markup = (XmlMarkup*)object;

		if (First == markup->First)
		{
			if (Last == markup->Last)
			{
				return 0;
			}
			else
			{

				return ((Last->Type.Is(XmlObject::XML_END))?Last->Before:Last)->Token.Data -
				((markup->Last->Type.Is(XmlObject::XML_END))?markup->Last->Before:markup->Last)->Token.Data;
			}
		}
		else

		{
			return ((First->Type.Is(XmlObject::XML_END))?First->Before:First)->Token.Data -
			((markup->First->Type.Is(XmlObject::XML_END))?markup->First->Before:markup->First)->Token.Data;			
		}
	}

	else
	{
		OutputError("XmlObject::Compare - Invalid object type for comparison.\n");
		return Identity::Error;
	}
}

void XmlMarkup::Print(String & string)
{
	XmlNavigator enumerator;
	enumerator.Navigate(this);

	for (enumerator.Forward();enumerator.Has();enumerator.Move())
	{
		String padding;
		for (int i=0;i<enumerator.Depth();++i)
			padding << "  ";

		string << padding;

		String markup;
		enumerator()->Print(markup);

		padding >> "\n";
		markup.Replace(String("\n"),padding);
		string << markup;

		string << "\n";
	}
}

void XmlMarkup::Debug()
{

	OutputMessage("\n-------------------------------------------------------------------------\n");

	if (First == 0)
	{
		OutputMessage("XmlMarkup:Debug - Empty\n");
		return;
	}
	else
	{
		XmlObject * root = First->Root();
		if (root->InstanceOf(XmlDocument::Instance))
		{
			if (root != this)
				OutputMessage("XmlMarkup:Debug - Fragment, %s\n",((XmlDocument*)root)->Resource.Url.Print());
			else
				OutputMessage("XmlMarkup:Debug - Markup, %s\n",((XmlDocument*)root)->Resource.Url.Print());
		}
		else
		{
			OutputMessage("XmlMarkup:Debug - Fragment\n");
		}
	}	

	XmlNavigator e;
	e.Navigate(this);

	String print;
	for (e.Forward();e.Has();e.Move())
	{
		OutputMessage("[%08lX] ",e.Pointer());

		for (int i=0;i<e.Depth();++i)OutputMessage("   ");

		e.Pointer()->Print(print);
		OutputMessage("%s\n",print.Print());
		print.Release();

	}

	OutputMessage("-------------------------------------------------------------------------\n");
}

void XmlMarkup::Destroy()
{

	XmlObject * object;

	while (First != 0)
	{
		object = First;
		First = First->After;
		object->Destroy();
		delete object;
	}

	First	=0;
	Last	=0;
	Count	=0;
}

void XmlMarkup::Release()
{

	XmlObject *garbage;

	while (First != 0)
	{
		garbage = First;
		First = First->After;
		garbage->Release();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlMarkupAssembler::XmlMarkupAssembler()
{

	Handler=0;
	Markup=0;
}

XmlMarkupAssembler::~XmlMarkupAssembler()
{

}

bool XmlMarkupAssembler::Initialise(XmlMarkup *markup,Reason::System::Handler *handler)
{
	Markup = markup;
	Handler = handler;
	return true;
}

bool XmlMarkupAssembler::Finalise()
{
	return true;
}

bool XmlMarkupAssembler::Validate(XmlObject *object)
{

	XmlObject *O;

	int error=0;	

	if (object)
	{
		O = object;
	}
	else
	{
		OutputMessage("XmlMarkupAssembler::Validate - Checking document structure in forwards order, any errors will be reported.\n");
		O = Markup->First;
	}

	while (O != 0)
	{
		if (O->Type.Is(XmlObject::XML_START) && O->Child != 0)
		{
			Validate(O->Child);

			if (O->After == 0)
			{
				OutputMessage("XmlMarkupAssembler::Validate - ERROR: XML_START object must have XML_END. %s[%08lX]\n", O->Print(),O);
				++error;
			}
			else
			{
				if (O->After->Parent != O->Parent)
				{
					OutputMessage("XmlMarkupAssembler::Validate - ERROR: XML_START object has XML_END, but does not share the same parent. %s[%08lX]->Parent( %s[%08lX]), %s[%08lX]->After( %s[%08lX]->Parent( ) )\n", O->Print(),O,O->Parent->Print(),O->Parent,O->After->Print(),O->After,O->After->Parent->Print(),O->After->Parent);
					++error;
				}

				XmlObject *C = O->Child;
				if (C)
				{
					while (C->After)
					{
						C = C->After;
					}

					if (O->After->Child == 0)
					{
						OutputMessage("XmlMarkupAssembler::Validate - ERROR: XML_START object has child, but XML_END does not. %s[%08lX]->Child( %s[%08lX] ), %s[%08lX]->After( %s[%08lX] )\n", O->Print(),O,O->Child->Print(),O->Child,O->Print(),O,O->After->Print(),O->After);
						++error;
					}
					else
					if (O->After->Child != C)
					{
						OutputMessage("XmlMarkupAssembler::Validate - ERROR: XML_START object has XML_END, but XML_END does not point to the correct final child. %s[%08lX]->After( %s[%08lX]->Child( %s[%08lX] ) )\n", O->Print(),O,O->After->Print(),O->After,O->After->Child->Print(),O->After->Child);
						++error;
					}
				}
			}
		}
		else
		if (O->Type.Is(XmlObject::XML_END))
		{
			if (O->Before == 0)
			{
				OutputMessage("XmlMarkupAssembler::Validate - ERROR: XML_END object must have XML_START. %s[%08lX]\n", O->Print(),O);
				++error;
			}
		}

		O = O->After;
	}

	OutputMessage("XmlMarkupAssembler::Validate - %d errors reported.\n",error);

	return (error==0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool XmlMarkupBuilder::Finalise()
{

	if (Markup)
	{
		while (Stack.Count > 0)
		{
			XmlObject * start = Stack.Pop();

			Handler->Warning("XmlMarkupBuilder - Deleting XML_START object %s[%08lX] because it has no XML_END object",start->Print(),start);

			XmlObject * child = start->Child;
			if (child)
			{
				if (start->Before)
				{
					child->Before = start->Before;
					start->Before->After = child;
				}
				else
				if (start->Parent == Markup)
				{
					Markup->First = child;
				}
				else
				{
					start->Parent->Child = child;
				}

				child->Parent = start->Parent;		
				while(child->After)
				{
					child = child->After;
					child->Parent = start->Parent;					
				}

				if (start->After)
				{
					child->After = start->After;
					start->After->Before = child;
				}
				else
				if (start->Parent->After)
				{
					start->Parent->After->Child = child;
				}

			}
			else
			{
				if (start->Before)
					start->Before->After = start->After;
				else
				if (start->Parent == Markup)
					Markup->First = start->After;
				else
					start->Parent->Child = start->After;

				if (start->After)
					start->After->Before = start->Before;
				else
				if (start->Parent->After)
					start->Parent->After->Child = start->Before;
			}

			delete start;
		}

		if (Markup->First)
		{
			Markup->Last = Markup->First->LastSibling();
		}
		else
		{
			Markup->First = Markup->Last = 0;
		}
	}

	return true;
}

bool XmlMarkupBuilder::Assemble(XmlObject *object)
{

	if (Markup->IsEmpty())
	{
		if (object->Is(XmlObject::XML_END))
		{
			Handler->Error("XmlMarkupBuilder - Markup cannot begin with XML_END object %s[%08lX]",object->Print(),object);
			delete object;
			return false;		
		}
		else
		if (object->Is(XmlObject::XML_START))
		{
			Stack.Push(object);
		}

		object->Parent = Markup;
		Markup->First = object;
	}
	else
	{
		XmlObject * last = Markup->Last;

		XmlObject * start = (last->Is(XmlObject::XML_START))?last:last->Parent;

		if (object->Is(XmlObject::XML_END))
		{
			if (!object->Token.Is(start->Token,true))
			{		
				if (start == last)
					Handler->Error("XmlMarkupBuilder - The last XML_START object does not match the current XML_END object %s[%08lX]",object->Print(),object);
				else
					Handler->Error("XmlMarkupBuilder - The XML_START parent of the last object does not match the current XML_END object %s[%08lX]",object->Print(),object);

				delete object;
				return false;
			}
			else
			{
				Stack.Remove(start);
				start->AttachAfter(object);
			}
		}
		else
		if (object->Is(XmlObject::XML_START))
		{

			Stack.Push(object);

			if (start == last)
			{

				start->AttachChild(object);
			}
			else
			{

				last->AttachAfter(object);
			}
		}
		else
		{
			if (start == last)
			{

				start->AttachChild(object);
			}
			else
			{

				last->AttachAfter(object);
			}
		}

	}

	Markup->Last = object;
	++ Markup->Count;

	OutputAssert(object->Parent != 0);
	OutputAssert(object->After != object);
	OutputAssert(object->Before != object);
	OutputAssert(object->Parent != object);
	OutputAssert(object->Child != object);
	OutputAssert(Markup->Last != 0);
	OutputAssert(Markup->First != 0);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool XmlMarkupConstructor::Finalise()
{

	if (Markup)
	{
		while (Stack.Count > 0)
		{
			XmlObject * start = Stack.Pop();

			if (start->After == 0)
			{

				XmlObject * end = XmlFactory::CreateEndElement();
				end->Token = start->Token;

				Handler->Warning("XmlMarkupConstructor - Creating XML_END object %s[%08lX] for XML_START object %s[%08lX]",end->Print(),end,start->Print(),start);

				XmlObject * hint = (XmlObject*)Hints.Select((int)(long)start);
				if (hint)
				{
					Handler->Warning("XmlMarkupConstructor - Using XML_START object %s[%08lX] as a hint for XML_END object being created for XML_START object %s[%08lX]",hint->Print(),hint,start->Print(),start);

					if (start->Child == hint)
						start->Child = 0;

					OutputAssert(hint->Parent == start);

					end->Parent = start->Parent;				

					start->After = end;
					end->Before = start;				

					if (hint->Before)
					{
						hint->Before->After = 0;
						end->Child = hint->Before;
					}

					hint->Before = end;
					end->After = hint;

					while(hint)
					{
						hint->Parent = start->Parent;
						hint = hint->After;
					}

				}
				else
				{
					start->AttachAfter(end);
					Markup->Last = end;		
				}

				++ Markup->Count;
			}
		}

		if (Markup->First)
		{
			Markup->Last = Markup->First->LastSibling();
		}
		else
		{
			Markup->First = Markup->Last = 0;
		}
	}

	return true;
}

bool XmlMarkupConstructor::Assemble(XmlObject *object)
{
	if (Markup->IsEmpty())
	{
		if (object->Type.Is(XmlObject::XML_END))
		{
			Handler->Error("Assemble","Markup cannot begin with XML_END object %s[%08lX]", object->Print(),object);
			delete object;
			return false;		
		}
		else
		if (object->Is(XmlObject::XML_START))
		{
			Stack.Push(object);
		}

		object->Parent = Markup;
		Markup->First = object;
	}
	else
	{
		XmlObject * last = Markup->Last;

		if (object->Is(XmlObject::XML_END))
		{

			XmlObject * start = (last->Is(XmlObject::XML_START))?last:last->Parent;

			XmlObject * parent = start;
			while (parent)
			{
				if (parent->Token.Is(object->Token,true))
					break;

				parent = parent->Parent;
			}

			if (parent)
			{
				if (parent != start)
				{

					Handler->Warning("XmlMarkupConstructor - The XML_END object %s[%08lX] does not match the last XML_START object %s[%08lX]", object->Print(),object,start->Print(),start);

					while(start != parent)
					{
						XmlObject * end = XmlFactory::CreateEndElement();
						end->Token = start->Token;

						Handler->Warning("XmlMarkupConstructor - Creating XML_END object %s[%08lX] for XML_START object %s[%08lX]",end->Print(),end,start->Print(),start);

						XmlObject * hint = (XmlObject*)Hints.Select((int)(long)start);
						if (hint)
						{
							Hints.Remove((int)(long)start);

							Handler->Warning("XmlMarkupConstructor - Using XML_START object %s[%08lX] as a hint for XML_END object being created for XML_START object %s[%08lX]",hint->Print(),hint,start->Print(),start);

							if (start->Child == hint)
								start->Child = 0;

							OutputAssert(hint->Parent == start);

							end->Parent = start->Parent;				

							start->After = end;
							end->Before = start;				

							if (hint->Before)
							{
								hint->Before->After = 0;
								end->Child = hint->Before;								
							}
							else
							{
								start->Child = 0;
							}

							hint->Before = end;
							end->After = hint;

							while(hint)
							{
								hint->Parent = start->Parent;
								hint = hint->After;
							}	
						}
						else
						{
							start->AttachAfter(end);
							Markup->Last = end;
						}

						++ Markup->Count;
						Stack.Remove(start);
						start = start->Parent;
					}					
				}

				Stack.Remove(parent);

				parent->AttachAfter(object);
			}
			else
			{
				Handler->Error("XmlMarkupConstructor - The current XML_END object %s[%08lX] does not match the last XML_START object or any of its ancestors", object->Print(),object);
				delete object;
				return false;
			}

		}
		else
		{
			if (object->Is(XmlObject::XML_START))
			{
				XmlObject * start = Stack.Select(object);
				if (start)
				{

					if (last->Parent == start || last == start)
						Hints.Insert(object,(int)(long)start);
				}

				Stack.Push(object);

			}

			if (last->Is(XmlObject::XML_START))
			{
				last->AttachChild(object);
			}
			else
			{
				last->AttachAfter(object);
			}
		}

	}

	Markup->Last = object;
	++ Markup->Count;

	OutputAssert(object->Parent != 0);
	OutputAssert(object->After != object);
	OutputAssert(object->Before != object);
	OutputAssert(object->Parent != object);
	OutputAssert(object->Child != object);
	OutputAssert(Markup->Last != 0);
	OutputAssert(Markup->First != 0);

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlMarkupReconstructor::XmlMarkupReconstructor():
	Strategy(STRATEGY_ADOPTION),Count(0),Depth(0),Scope(0)
{

}

XmlMarkupReconstructor::XmlMarkupReconstructor(char strategy):
	Strategy(strategy),Count(0),Depth(0),Scope(0)
{

}

XmlMarkupReconstructor::~XmlMarkupReconstructor()
{

}

bool XmlMarkupReconstructor::Finalise()
{
	Repair();
	return true;
}

bool XmlMarkupReconstructor::Assemble(XmlObject *object)
{	
	bool result=false;

	if (object->Is(XmlObject::XML_START))
		result=AssembleStart(object);
	else
	if (object->Is(XmlObject::XML_END))
		result=AssembleEnd(object);
	else
	if (object->Is(XmlObject::XML_EMPTY))
		result=AssembleEmpty(object);
	else
	if (object->Is(XmlObject::XML_COMMENT))
		result=AssembleComment(object);
	else
	if (object->Is(XmlObject::XML_TEXT))
		result=AssembleText(object);
	else
		result=AssembleAny(object);

	if (result)
	{

		if (Markup->Last == object)
		{

			if (object->Parent && object->Before == 0)
			{

				++Depth;
			}

			#ifdef _DEBUG
			OutputAssert(object->Depth() == Depth);
			#endif
		}		

		if (object->Is(XmlObject::XML_START))
			Stack.Push(new XmlEntry(object,Depth,Markup->Count));

		return true;
	}
	else
	{
		return false;
	}
}

bool XmlMarkupReconstructor::AssembleAny(XmlObject * object)
{

	if (Markup->IsEmpty())
	{
		if (object->Type.Is(XmlObject::XML_END))
		{
			Handler->Error("XmlMarkupReconstructor::AssembleAny - Markup cannot begin with XML_END object %s[%08lX]",object->Print(),object);
			delete object;
			return false;		
		}

		object->Parent = Markup;
		Markup->First = object;
	}
	else
	{
		XmlObject * last = Markup->Last;

		if (last->Type.Is(XmlObject::XML_START))
		{
			if (object->Type.Is(XmlObject::XML_END))
			{

				if (! object->Token.Is(last->Token,true))
				{
					Handler->Error("XmlMarkupReconstructor::AssembleAny - The last XML_START object %s[%08lX] does not match the current XML_END object %s[%08lX]",last->Print(),last,object->Print(),object);
					delete object;
					return false;
				}

				last->AttachAfter(object);
			}
			else
			{

				last->AttachChild(object);
			}
		}
		else
		if (object->Type.Is(XmlObject::XML_END))
		{

			if (! object->Token.Is(last->Parent->Token,true))
			{
				Handler->Error("XmlMarkupReconstructor::AssembleAny - The parent XML_START object %s[%08lX] of the last object does not match the current XML_END object %s[%08lX]",last->Parent->Print(),last->Parent,object->Print(),object);
				delete object;
				return false;
			}

			last->Parent->AttachAfter(object);
		}
		else
		{

			last->AttachAfter(object);
		}

	}

	Markup->Last = object;
	++ Markup->Count;

	OutputAssert(object->Parent != 0);
	OutputAssert(object->After != object);
	OutputAssert(object->Before != object);
	OutputAssert(object->Parent != object);
	OutputAssert(object->Child != object);
	OutputAssert(Markup->Last != 0);
	OutputAssert(Markup->First != 0);

	return true;
}

bool XmlMarkupReconstructor::AssembleStart(XmlObject *object)
{

	if ( ! Markup->IsEmpty() && (Markup->InstanceOf(XmlDocument::Instance)))
	{

		XmlDocument *document = (XmlDocument*)Markup;
		if (document->Dtd && !document->Dtd->IsEmpty())
		{
			XmlDtd *dtd = document->Dtd;	

			XmlObject * parent = (Markup->Last->Type.Is(XmlObject::XML_START))?Markup->Last:Markup->Last->Parent;

			XmlElementDeclaration *parentDeclaration = (XmlElementDeclaration *) dtd->ElementTable.Select(parent);

			BnfMatcher matcher;
			if (parentDeclaration && !matcher.Contains(object))

			{

				XmlEntry * entry = Stack.SelectEntry(object->Token,XmlObject::XML_START,Depth,Markup->Count);
				if (entry && (Depth - entry->Depth) < 4)
				{

					XmlObject * start = (XmlObject*)entry->Object;

					while(parent && parent != start)
						parent = parent->Parent;

					if (parent)
					{
						XmlElementDeclaration *parentDeclaration = (XmlElementDeclaration *) dtd->ElementTable.Select(start);

						BnfMatcher matcher;
						if (parentDeclaration && ! matcher.Contains(object))

						{

							XmlObject * end = XmlFactory::CreateEndElement();
							end->Token.Assign(start->Token);
							start->AttachAfter(end);

							Handler->Warning("XmlMarkupReconstructor::AssembleStart - Creating XML_END object %s[%08lX] for XML_START object %s[%08lX]",end->Print(),end,start->Print(),start);

							Depth = entry->Depth;
							Stack.DeleteEntry(entry);

							Markup->Last = end;

							OutputAssert(end->Depth() == Depth);
							++Markup->Count;
						}
					}
				}
			}

		}
	}

	return AssembleAny(object);
}

bool XmlMarkupReconstructor::AssembleEnd(XmlObject *object)
{

	XmlEntry * entry = Stack.SelectEntry(object->Token,XmlObject::XML_START,Depth,Markup->Count);

	if (entry && (Scope==0 || (entry->Depth) >= (Depth-(Scope-1))))
	{

		XmlObject *start = (XmlObject *) entry->Object;
		XmlObject *last = Markup->Last;

		if ( (last != start) && ( last->Type.Is(XmlObject::XML_START) || last->Parent != start ) ) 
		{

			switch (Strategy)
			{
			case STRATEGY_ADOPTION:
			{	

				XmlObject * parent = start;
				while(parent && parent->After == 0)
					parent = parent->Parent;

				#ifdef _DEBUG

				if (((entry->Depth) == ((XmlObject*)entry->Object)->Depth()) == false)
				{
					Markup->Debug();

					OutputFlush();

					String output;
					Markup->Print(output);
					OutputMessage("\n\n%s\n\n",output.Print());

					OutputFlush();
				}

				OutputAssert((entry->Depth) == ((XmlObject*)entry->Object)->Depth());
				#endif

				start->AttachAfter(object);

				if (!parent)
				{
					Depth = entry->Depth;
					Markup->Last = object;

					#ifdef _DEBUG

					OutputAssert(object->Depth() == Depth);
					#endif
				}

				Stack.DeleteEntry(entry);
				++ Markup->Count;

			}
			break;
			case STRATEGY_STRUCTURE:
			{

				object->Type.Assign(XmlObject::XML_EMPTY);
				object->Child = start;

				Stack.DeleteEntry(entry);
				Stack.Push(object);				

				last->AttachAfter(object);

				Markup->Last = object;
				++ Markup->Count;

				#ifdef _DEBUG

					if (last->Type.Is(XmlObject::XML_START))
					{
						OutputMessage("XmlMarkupReconstructor::AssembleEnd - XML_END object %s[%08lX] requires fixing because the tail object is a binary object %s[%08lX]\n",object->Print(),object, last->Print(),last);
					}
					else
					{
						if (last->Parent != 0)
							OutputMessage("XmlMarkupReconstructor::AssembleEnd - XML_END object %s[%08lX] requires fixing due to unclosed binary ancestor object %s[%08lX]\n",object->Print(),object, last->Parent->Print(),last->Parent);
						else
							OutputMessage("XmlMarkupReconstructor::AssembleEnd - XML_END object %s[%08lX] requires fixing because the tail object has no parent.\n",object->Print(),object);
					}

				#endif

			}
			break;
			}
		}
		else
		{

			#ifdef _DEBUG

			if (((entry->Depth) == ((XmlObject*)entry->Object)->Depth()) == false)
			{
				Markup->Debug();

				OutputFlush();

				String output;
				Markup->Print(output);
				OutputMessage("\n\n%s\n\n",output.Print());

				OutputFlush();
			}

			OutputAssert((entry->Depth) == ((XmlObject*)entry->Object)->Depth());

			#endif

			Depth = entry->Depth;
			Stack.DeleteEntry(entry);	
			AssembleAny(object);
		}

		#ifdef _DEBUG
		OutputAssert(object->Depth() == start->Depth());
		OutputAssert(object->After != object);
		OutputAssert(object->Before != object);
		OutputAssert(object->Parent != object);
		OutputAssert(object->Child != object);
		#endif	

		return true;
	}
	else
	{

		delete object;
		return false;
	}

}

bool XmlMarkupReconstructor::AssembleEmpty(XmlObject *object)
{

	return AssembleAny(object);
}

bool XmlMarkupReconstructor::AssembleText(XmlObject *object)
{
	return AssembleAny(object);
}

bool XmlMarkupReconstructor::AssembleComment(XmlObject *object)
{
	return AssembleAny(object);
}

void XmlMarkupReconstructor::Repair()
{
	if (Markup->IsEmpty())
		return;

	#ifdef _DEBUG

	#endif

	switch (Strategy)
	{
		case STRATEGY_ADOPTION:
		{

			RepairAdoption();
		}
		break;
		case STRATEGY_STRUCTURE:
		{

			RepairOrphan();
			RepairStructure();
		}
		break;
	}	

	#ifdef _DEBUG

	#endif

	assert(Stack.IsEmpty());
	OutputMessage("XmlMarkupReconstructor::Repair - Repaired %d element(s).\n",Count);
}

void XmlMarkupReconstructor::RepairStructure()
{
	OutputMessage("XmlMarkupReconstructor::RepairStructure - Using structure strategy.\n");

	Reason::Structure::Objects::Entry *stackPointer = Stack.Bottom();

	if (!stackPointer) 
		return;

	XmlObject *targetObject;
	XmlObject *repairObject;
	XmlObject *startObject;

	while (stackPointer != 0)
	{
		repairObject = (XmlObject *) stackPointer->Object;

		if ( repairObject->Type.Is(XmlObject::XML_EMPTY) )
		{

			repairObject->Type.Assign(XmlObject::XML_END);
			startObject = repairObject->Child;
			repairObject->Child = 0;

			targetObject = repairObject->Parent;

			if (targetObject == startObject)
			{
				if (repairObject == startObject->Child)	
				{
					startObject->AttachAfter(startObject->DetachChild());
				}
				else
				{
					startObject->AttachAfter(repairObject->Before->DetachAfter());
				}
			}
			else
			{

				XmlObject *completeObject = 0;
				while (targetObject != 0)
				{
					if (targetObject->Parent == startObject)
					{

						if (targetObject->After == 0 && completeObject != 0) 
							targetObject = completeObject;

						if (targetObject->After != 0)	
						{

							targetObject = targetObject->After;

							if (repairObject->Before != 0)
							{
								repairObject->Before->After = repairObject->After;
							}
							else
							{

								repairObject->Parent->Child = repairObject->After;
							}

							if (repairObject->After != 0)	
							{
								repairObject->After->Before = repairObject->Before;
							}
							else
							{

								if (repairObject->Parent->After != 0)
									repairObject->Parent->After->Child = repairObject->Before;
							}

							repairObject->Before = 0;
							repairObject->After = 0;

							if (targetObject->After != 0)
							{

								repairObject->AttachAfter(targetObject->DetachAfter());
							}

							startObject->AttachAfter(repairObject);
						}
						else	
						{
							///////////////////////////////////////////////////////////////////////////////////////////

							///////////////////////////////////////////////////////////////////////////////////////////

							if (repairObject->Before != 0)
							{

								repairObject->Before->DetachAfter();
								startObject->AttachAfter(repairObject);
							}
							else
							{

								startObject->AttachAfter(repairObject->Parent->DetachChild());
							}

						}

						break;
					} 
					else
					if (targetObject->Type.Is(XmlObject::XML_START) && targetObject->After != 0)
					{

						completeObject = targetObject;
					}

					targetObject = targetObject->Parent;

				}	
			}

			if (targetObject == 0)
			{

				if (repairObject->Before)
				{
					repairObject->Before->After = repairObject->After;
				}
				else
				{

					repairObject->Parent->Child = repairObject->After;
				}

				if (repairObject->After)
				{
					repairObject->After->Before = repairObject->Before;

				}
				else
				{
					if (repairObject->Parent->After != 0)
						repairObject->Parent->After->Child = repairObject->Before;
				}

				repairObject->Before = 0;
				repairObject->After = 0;
				repairObject->Parent = 0;

				startObject->AttachAfter(repairObject);
			}

			++Count;

			#ifdef _DEBUG
				OutputMessage("XmlMarkupReconstructor::RepairStructure - Corrected out of order tag: </%s> [%08lX], counterpart is: <%s> [%08lX]\n",repairObject->Token.Print(),repairObject,startObject->Token.Print(),startObject);
			#endif
		}
		else
		{
			OutputError("XmlMarkupReconstructor::RepairStructure - Unexpected tag type on receptor stack: <%s> [%08lX]\n\t\t- Type is [%d], has someone been lazy and not updated this code ?",repairObject->Token.Print(),repairObject,(int)repairObject->Type);
		}

		#ifdef _DEBUG

			assert(repairObject->After != repairObject);
			assert(repairObject->Before != repairObject);
			assert(repairObject->Parent != repairObject);
			assert(repairObject->Child != repairObject);
		#endif

		stackPointer = stackPointer->Next;

	} 

	targetObject = Markup->First->LastSibling();
	Markup->Last = targetObject;

}

void XmlMarkupReconstructor::Validate()
{

	if (Markup->Last != Markup->First->LastSibling())
	{
		Markup->Last = Markup->First->LastSibling();

		OutputMessage("XmlMarkupReconstructor::Validate - Repairing last object [%08lX] %s\n",Markup->Last,Markup->Last->Print());
	}

	XmlObject *targetObject;
	XmlObject *childObject;
	int count	=0;
	int verify	=0;

	OutputMessage("XmlMarkupReconstructor::Validate - Performing reverse traversal.\n");

	targetObject = Markup->Last;
	while (targetObject != 0)
	{
		++verify;

		if (targetObject->Type.Is(XmlObject::XML_END)) 
		{
			if (targetObject->Before->Child)
			{

				childObject = targetObject->Before->LastChild();
				if (targetObject->Child != childObject)
				{
					OutputError("XmlMarkupReconstructor::Validate - Repairing unattached reverse child [%08lX] %s\n",childObject,childObject->Print());
					assert(targetObject->Child == childObject);
					targetObject->Child = childObject;
					++count;
				}

				targetObject = targetObject->Child;
			}
			else
			{
				targetObject = targetObject->Before;
			}
		}
		else
		{

			if (targetObject->Before == 0)
			{
				if (targetObject->Parent != Markup)
					targetObject = targetObject->Parent;
				else
					targetObject = 0;
			}
			else
			{
				targetObject = targetObject->Before;
			}
		}

	}

	if (verify != Markup->Count)
	{
		OutputError("XmlMarkupReconstructor::Validate - The number of objects traversed does not match the number reported by the document.\n");
		assert(verify == Markup->Count);
	}

	OutputMessage("XmlMarkupReconstructor::Validate - Performing forwards traversal.\n");

	verify =0;
	targetObject = Markup->First;
	while (targetObject != 0)
	{
		++verify;

		if (targetObject->Type.Is(XmlObject::XML_START)) 
		{
			assert(targetObject->After != 0);

			if (targetObject->Child)
			{

				childObject = targetObject->Child;
				while (childObject != 0)
				{
					if (childObject->Parent != targetObject)
					{
						OutputError("XmlMarkupReconstructor::Validate - Child object [%08lX] %s has incorrect parent.\n",childObject,childObject->Print());

						#ifdef _DEBUG
							assert(childObject->Parent == targetObject);
						#endif

						childObject->Parent = targetObject;
						++count;
					}

					if (childObject->After == 0)
						break;

					childObject = childObject->After;
				}

				if (targetObject->After->Child != childObject)
				{
					OutputMessage("XmlMarkupReconstructor::Validate - Repairing unattached reverse child [%08lX] %s\n",childObject,childObject->Print());

					#ifdef _DEBUG
						assert(targetObject->After->Child==childObject);
					#endif

					targetObject->After->Child = childObject;
					++count;
				}

				targetObject = targetObject->Child;
			}
			else
			{
				targetObject = targetObject->After;
			}
		}
		else
		{

			if (targetObject->After == 0)
			{
				targetObject = targetObject->Parent->After;
			}
			else
			{
				targetObject = targetObject->After;
			}
		}

	}

	if (verify != Markup->Count)
	{
		OutputError("XmlMarkupReconstructor::Validate - The number of objects traversed does not match the number reported by the document.\n");
		assert(verify == Markup->Count);
	}

	if (count > 0)
	{
		OutputError("XmlMarkupReconstructor::Validate - %d objects were repaired, markup is not being constructed correctly.\n",count);
		assert(count==0);
	}

}

void XmlMarkupReconstructor::RepairAdoption()
{
	#ifdef _DEBUG
	OutputMessage("XmlMarkupReconstructor::RepairAdoption - Using adoption strategy.\n");
	#endif

	Reason::Structure::Objects::Entry *stackPointer;
	XmlObject *repairObject;
	XmlObject *targetObject;

	XmlDtd *dtd=0;

	if (Markup->InstanceOf(XmlDocument::Instance))
	{	

		XmlDocument *document = (XmlDocument*)Markup;
		if (document->Dtd && !document->Dtd->IsEmpty())
		{
			dtd = document->Dtd;
		}
	}

	stackPointer = Stack.Top();

	while (stackPointer != 0)
	{

		repairObject = (XmlObject *) stackPointer->Object;

		if ( repairObject->Type.Is(XmlObject::XML_START))
		{
			XmlObject * endObject = XmlFactory::CreateEndElement();
			endObject->Token.Assign(repairObject->Token);

			#ifdef _DEBUG
			OutputMessage("XmlMarkupReconstructor::RepairAdoption - Object [%08lX] will adopt virtual object [%08lX] %s\n",repairObject,endObject,endObject->Print());
			#endif	

			targetObject = repairObject->LastChild();
			bool fallback = true;

			if (dtd)
			{
				XmlElementDeclaration *elementDeclaration = (XmlElementDeclaration *) dtd->ElementTable.Select(repairObject);
				if (elementDeclaration)
				{
					while ( targetObject && targetObject->Type.Is(XmlObject::XML_BINARY) )
					{				

						BnfMatcher matcher;
						if (!matcher.Contains(targetObject))

						{
							fallback = false;

							if (targetObject->Type.Is(XmlObject::XML_END))
							{
								targetObject = targetObject->Before;
							}

							if (targetObject == repairObject->FirstChild())
							{

								targetObject = repairObject->DetachChild();
							}
							else
							{
								targetObject = targetObject->Before->DetachAfter();
							}

							endObject->InsertAfter(targetObject);

							targetObject = repairObject->LastChild();
						}
						else
						{
							break;
						}
					}
				}				
			}

			if ( fallback && targetObject && targetObject->Type.Is(XmlObject::XML_START) && targetObject->Child )
			{

				#ifdef _DEBUG 
				OutputMessage("XmlMarkupReconstructor::RepairAdoption - DTD repair unavailable, using fallback truncation policy for children of object [%08lX] %s",targetObject,targetObject->Print());
				#endif

				endObject->AttachAfter(targetObject->DetachChild());
			}

			repairObject->AttachAfter(endObject);

			#ifdef _DEBUG

			#endif		

			Reason::Structure::Objects::Entry *garbage = stackPointer;
			stackPointer = stackPointer->Prev;
			Stack.DeleteEntry(garbage);	

			++Markup->Count;
			++Count;
		}
		else
		{
			stackPointer = stackPointer->Prev;
		}

		#ifdef _DEBUG

		#endif

	}

	if (Markup->Last != Markup->First->LastSibling())
	{
		Markup->Last = Markup->First->LastSibling();
	}
}

void XmlMarkupReconstructor::RepairOrphan()
{
	#ifdef _DEBUG
	OutputMessage("XmlMarkupReconstructor::RepairOrphan - Using orphan strategy.\n");
	#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////////

	Reason::Structure::Objects::Entry *stackGarbage;
	Reason::Structure::Objects::Entry *stackPointer;

	XmlObject *repairObject;

	stackPointer = Stack.Bottom();
	while (stackPointer != 0)
	{
		repairObject = (XmlObject *) stackPointer->Object;

		if ( repairObject->Type.Is(XmlObject::XML_START))
		{
			if(Strategy == STRATEGY_ADOPTION)
			{

				if (	repairObject->Token.Is("font",true) ||
						repairObject->Token.Is("p",true) ||
						repairObject->Token.Is("li",true) ||
						repairObject->Token.Is("center",true) ||	
						repairObject->Token.Is("b",true))
				{
						XmlObject *child = repairObject->DetachChild();
						repairObject->Type = XmlObject::XML_EMPTY;
						if (child) repairObject->AttachAfter(child);

						stackGarbage = stackPointer;
						stackPointer = stackPointer->Next;
						Stack.DeleteEntry(stackGarbage);	
						++Count;

						#ifdef _DEBUG
						OutputMessage("XmlMarkupReconstructor::RepairOrphan - Demoted empty html binary start tag to solitary status: [%08lX] %s\n",repairObject,repairObject->Print());
						#endif
				}
				else
				{
					stackPointer = stackPointer->Next;
				}

			}
			else
			{

				XmlObject *child = repairObject->DetachChild();

				repairObject->Type = XmlObject::XML_EMPTY;
				if (child) repairObject->AttachAfter(child);

				#ifdef _DEBUG
				OutputMessage("XmlMarkupReconstructor::RepairOrphan - Demoted binary start tag with missing end to solitary status: [%08lX] %s\n",repairObject,repairObject->Print());
				#endif

				stackGarbage  = stackPointer;
				stackPointer = stackPointer->Next;
				Stack.DeleteEntry(stackGarbage);	
				++Count;
			}
		}
		else
		{
			stackPointer = stackPointer->Next;
		}

		#ifdef _DEBUG
			assert(repairObject->After != repairObject);
			assert(repairObject->Before != repairObject);
			assert(repairObject->Parent != repairObject);
			assert(repairObject->Child != repairObject);
		#endif	
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlMarkupIndexer::XmlMarkupIndexer(XmlIndex * index, XmlMarkupAssembler * assembler):
	Index(index),Assembler(assembler),Caseless(false)
{
}

XmlMarkupIndexer::XmlMarkupIndexer():
	Index(0),Assembler(0),Caseless(false)
{
}

bool XmlMarkupIndexer::Assemble(XmlObject *object)
{

	if (Assembler->Assemble(object) && Index)
	{
		if (object->Type.Is(XmlObject::XML_ELEMENT))
		{
			Index->ElementCatalogue.Attach(object,object->Token,Caseless);

			Index->ElementList.Append(object);

			XmlElement *element = (XmlElement*)object;

			for(element->Attributes.Forward();element->Attributes.Has();element->Attributes.Move())
			{

				Index->AttributeList.Append(element->Attributes.Pointer());
				Index->AttributeNameCatalogue.Attach(element->Attributes.Pointer(),element->Attributes.Pointer()->Name,Caseless);
				Index->AttributeValueCatalogue.Attach(element->Attributes.Pointer(),element->Attributes.Pointer()->Value,Caseless);
			}
		}
		else
		if (object->Type.Is(XmlObject::XML_TEXT))
		{

			XmlText *text = (XmlText*)object;

			Path sequence;
			sequence.Assign(text->Token);
			sequence.Tokenise("\n\t\r\f ~`!@#$%^&*()_-+={}[]|\\|:;\"'<,>.?/",false);

			Path::Enumerator iterator(sequence);
			for (iterator.Forward();iterator.Has();iterator.Move())
			{
				if (!iterator()->IsEmpty())
				{
					Index->TextCatalogue.Attach(object,iterator.Reference(),Caseless);

				}
			}

			Index->TextList.Append(object);
		}

		return true;
	}
	else
	{
		return false;
	}
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

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

