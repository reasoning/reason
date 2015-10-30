
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

#include <math.h>
#include <stdlib.h>	
#include "reason/reason.h"
#include "reason/language/xml/xml.h"
#ifdef REASON_USING_XML_STYLE
#include "reason/language/xml/style.h"
#endif
#include "reason/language/xml/dtd.h"
#include "reason/language/xml/document.h"
#include "reason/language/xml/enumerator.h"

#include "reason/language/xpath/xpath.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::Language::Xml;
using namespace Reason::System;

namespace Reason { namespace Language { namespace Xpath {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpression::XpathExpression()
	:Type(NONE),Result(0)
{

}

XpathExpression::XpathExpression(unsigned int type)
	:Type(type),Result(0)
{

}

XpathExpression::~XpathExpression()
{
	Release();
}

void XpathExpression::Release()
{
	if (Result)
	{
		delete Result;
		Result = 0;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpressionBinary::XpathExpressionBinary() :
	OperandOne(0),OperandTwo(0)
{

}

XpathExpressionBinary::~XpathExpressionBinary()
{
	if (OperandOne)
		delete OperandOne;

	if (OperandTwo)
		delete OperandTwo;
}

void XpathExpressionBinary::Print(String & string)
{
	OperandOne->Print(string);

	switch (Type)
		{
		case EXPRESSION_BINARY_OR: string << " or ";
			break;
		case EXPRESSION_BINARY_AND: string << " and ";
			break;
		case EXPRESSION_BINARY_EQUAL: string << " = ";
			break;
		case EXPRESSION_BINARY_NOT_EQUAL: string << " != ";
			break;
		case EXPRESSION_BINARY_LESS_THAN: string << " < ";
			break;
		case EXPRESSION_BINARY_LESS_THAN_OR_EQUAL: string << " <= ";
			break;
		case EXPRESSION_BINARY_GREATER_THAN: string << " > ";
			break;
		case EXPRESSION_BINARY_GREATER_THAN_OR_EQUAL: string << " >= ";
			break;
		case EXPRESSION_BINARY_ADD: string << " + ";
			break;
		case EXPRESSION_BINARY_SUBTRACT: string << " - ";
			break;
		case EXPRESSION_BINARY_MULTIPLY: string << " * ";
			break;
		case EXPRESSION_BINARY_DIV: string << " div ";
			break;
		case EXPRESSION_BINARY_MOD: string << " mod ";
			break;
		case EXPRESSION_BINARY_UNION: string << " | ";
			break;
		case EXPRESSION_BINARY_DIFFERENCE: string << "^";
			break;
		case EXPRESSION_BINARY_INTERSECTION: string << "&";
			break;
	};

	OperandTwo->Print(string);
}

XpathObject * XpathExpressionBinary::Evaluate(XpathState & state)
{
	Release();

	if (Type.Is(EXPRESSION))
	{

		if (Type.Is(EXPRESSION_BINARY_OR))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			if (A)
			{
				Result = new XpathBoolean();

				XpathBoolean booleanA;
				booleanA.Boolean(A);

				if (booleanA.Value)
				{
					((XpathBoolean *)Result)->Value = true;
				}
				else
				{
					XpathObject *B = OperandTwo->Evaluate(state);
					if (B)
					{
						XpathBoolean booleanB;
						booleanB.Boolean(B);
						((XpathBoolean *)Result)->Value = booleanB.Value;
					}
				}
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_AND))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			if (A)
			{
				Result = new XpathBoolean();
				XpathBoolean booleanA;
				booleanA.Boolean(A);

				if (!booleanA.Value)
				{
					((XpathBoolean *)Result)->Value = false;
				}
				else
				{
					XpathObject *B = OperandTwo->Evaluate(state);
					if (B)
					{
						XpathBoolean booleanB;
						booleanB.Boolean(B);
						((XpathBoolean *)Result)->Value = booleanB.Value;
					}
				}
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_EQUAL))
		{
			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);

			if (A && B)
			{
				Result = new XpathBoolean();
				if (A->Type.Is(XpathObject::TYPE_NUMBER) || B->Type.Is(XpathObject::TYPE_NUMBER))
				{
					XpathNumber numberA;
					numberA.Number(A);

					XpathNumber numberB;
					numberB.Number(B);

					((XpathBoolean *)Result)->Value = numberA.Value == numberB.Value;
				}
				else
				{
					((XpathBoolean *)Result)->Value = A->Compare(B) == 0;
				}
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_NOT_EQUAL))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);

			if (A && B)
			{
				Result = new XpathBoolean();
				if (A->Type.Is(XpathObject::TYPE_NUMBER) || B->Type.Is(XpathObject::TYPE_NUMBER))
				{
					XpathNumber numberA;
					numberA.Number(A);

					XpathNumber numberB;
					numberB.Number(B);

					((XpathBoolean *)Result)->Value = ! (numberA.Value == numberB.Value);
				}
				else
				{
					((XpathBoolean *)Result)->Value = A->Compare(B) != 0;
				}
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_LESS_THAN))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);

			if (A && B)
			{
				Result = new XpathBoolean();
				if (A->Type.Is(XpathObject::TYPE_NUMBER) || B->Type.Is(XpathObject::TYPE_NUMBER))
				{
					XpathNumber numberA;
					numberA.Number(A);

					XpathNumber numberB;
					numberB.Number(B);

					((XpathBoolean *)Result)->Value = numberA.Value < numberB.Value;

				}
				else
				{
					((XpathBoolean *)Result)->Value = A->Compare(B) < 0;
				}
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_LESS_THAN_OR_EQUAL))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);

			if (A && B)
			{
				Result = new XpathBoolean();
				if (A->Type.Is(XpathObject::TYPE_NUMBER) || B->Type.Is(XpathObject::TYPE_NUMBER))
				{
					XpathNumber numberA;
					numberA.Number(A);

					XpathNumber numberB;
					numberB.Number(B);

					((XpathBoolean *)Result)->Value = numberA.Value <= numberB.Value;

				}
				else
				{
					((XpathBoolean *)Result)->Value = A->Compare(B) <= 0;
				}				
			}
		}
		else 
		if (Type.Is(EXPRESSION_BINARY_GREATER_THAN))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);

			if (A && B)
			{
				Result = new XpathBoolean();
				if (A->Type.Is(XpathObject::TYPE_NUMBER) || B->Type.Is(XpathObject::TYPE_NUMBER))
				{
					XpathNumber numberA;
					numberA.Number(A);

					XpathNumber numberB;
					numberB.Number(B);

					((XpathBoolean *)Result)->Value = numberA.Value > numberB.Value;
				}
				else
				{
					((XpathBoolean *)Result)->Value = A->Compare(B) > 0;
				}				
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_GREATER_THAN_OR_EQUAL))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);

			if (A && B)
			{
				Result = new XpathBoolean();
				if (A->Type.Is(XpathObject::TYPE_NUMBER) || B->Type.Is(XpathObject::TYPE_NUMBER))
				{
					XpathNumber numberA;
					numberA.Number(A);

					XpathNumber numberB;
					numberB.Number(B);

					((XpathBoolean *)Result)->Value = numberA.Value >= numberB.Value;
				}
				else
				{
					((XpathBoolean *)Result)->Value = A->Compare(B) >= 0;
				}				
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_ADD))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);

			if (A && B)
			{
				XpathNumber numberA;
				numberA.Number(A);

				XpathNumber numberB;
				numberB.Number(B);

				Result = new XpathNumber();
				((XpathNumber*)Result)->Value = numberA.Value + numberB.Value;
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_SUBTRACT))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);

			if (A && B)
			{
				XpathNumber numberA;
				numberA.Number(A);

				XpathNumber numberB;
				numberB.Number(B);

				Result = new XpathNumber();
				((XpathNumber*)Result)->Value = numberA.Value - numberB.Value;
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_MULTIPLY))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);
			if (A && B)
			{
				XpathNumber numberA;
				numberA.Number(A);

				XpathNumber numberB;
				numberB.Number(B);

				Result = new XpathNumber();
				((XpathNumber*)Result)->Value = numberA.Value * numberB.Value;
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_DIV))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);
			if (A && B)
			{
				XpathNumber numberA;
				numberA.Number(A);

				XpathNumber numberB;
				numberB.Number(B);

				Result = new XpathNumber();
				((XpathNumber*)Result)->Value = numberA.Value / numberB.Value;
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_MOD))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);
			if (A && B)
			{
				XpathNumber numberA;
				numberA.Number(A);

				XpathNumber numberB;
				numberB.Number(B);

				Result = new XpathNumber();
				((XpathNumber*)Result)->Value = (int) numberA.Value % (int) numberB.Value;
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_UNION))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);

			if (A && B)
			{
				if (A->Type.Is(XpathObject::TYPE_SEQUENCE) && B->Type.Is(XpathObject::TYPE_SEQUENCE))
				{
					XpathSequence *sequenceA = (XpathSequence *)A;
					XpathSequence *sequenceB = (XpathSequence *)B;

					Result = new XpathSequence();
					((XpathSequence*)Result)->Union(*sequenceA);
					((XpathSequence*)Result)->Union(*sequenceB);
					((XpathSequence*)Result)->OrdinalSort();
				}
				else
				{
					OutputError("XpathExpression::Evaluate - The union expression must have two sequences as operands.\n");
				}
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_INTERSECTION))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);

			if (A && B)
			{
				if (A->Type.Is(XpathObject::TYPE_SEQUENCE) && B->Type.Is(XpathObject::TYPE_SEQUENCE))
				{
					XpathSequence *sequenceA = (XpathSequence *)A;
					XpathSequence *sequenceB = (XpathSequence *)B;

					Result = new XpathSequence();
					((XpathSequence*)Result)->Union(*sequenceA);
					((XpathSequence*)Result)->Intersection(*sequenceB);
					((XpathSequence*)Result)->OrdinalSort();
				}
				else
				{
					OutputError("XpathExpression::Evaluate - The intersection expression must have two sequences as operands.\n");
				}
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_DIFFERENCE))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);

			if (A && B)
			{
				if (A->Type.Is(XpathObject::TYPE_SEQUENCE) && B->Type.Is(XpathObject::TYPE_SEQUENCE))
				{
					XpathSequence *sequenceA = (XpathSequence *)A;
					XpathSequence *sequenceB = (XpathSequence *)B;

					Result = new XpathSequence();
					((XpathSequence*)Result)->Union(*sequenceA);
					((XpathSequence*)Result)->Difference(*sequenceB);
					((XpathSequence*)Result)->OrdinalSort();
				}
				else
				{
					OutputError("XpathExpression::Evaluate - The intersection expression must have two sequences as operands.\n");
				}
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_PRECEDING))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);

			if (A && B)
			{
				if (A->Type.Is(XpathObject::TYPE_SEQUENCE) && B->Type.Is(XpathObject::TYPE_SEQUENCE))
				{
					XpathSequence *sequenceA = (XpathSequence *)A;
					XpathSequence *sequenceB = (XpathSequence *)B;

					Result = new XpathBoolean();
					if (sequenceA->IsEmpty() || sequenceB->IsEmpty() || !sequenceA->IsObjects() || !sequenceB->IsObjects())
					{
						((XpathBoolean *)Result)->Value = false;
					}
					else
					{
						XmlObject * objectA = (XmlObject*)sequenceA->First->Object;
						XmlObject * objectB = (XmlObject*)sequenceB->First->Object;
						((XpathBoolean *)Result)->Value = objectA->Ordinal() < objectB->Ordinal();
					}
				}
				else
				{
					OutputError("XpathExpression::Evaluate - The preceding expression must have two sequences as operands.\n");
				}
			}
		}
		else
		if (Type.Is(EXPRESSION_BINARY_FOLLOWING))
		{

			XpathObject *A = OperandOne->Evaluate(state);
			XpathObject *B = OperandTwo->Evaluate(state);

			if (A && B)
			{
				if (A->Type.Is(XpathObject::TYPE_SEQUENCE) && B->Type.Is(XpathObject::TYPE_SEQUENCE))
				{
					XpathSequence *sequenceA = (XpathSequence *)A;
					XpathSequence *sequenceB = (XpathSequence *)B;

					Result = new XpathBoolean();
					if (sequenceA->IsEmpty() || sequenceB->IsEmpty() || !sequenceA->IsObjects() || !sequenceB->IsObjects())
					{
						((XpathBoolean *)Result)->Value = false;
					}
					else
					{
						XmlObject * objectA = (XmlObject*)sequenceA->First->Object;
						XmlObject * objectB = (XmlObject*)sequenceB->First->Object;
						((XpathBoolean *)Result)->Value = objectA->Ordinal() > objectB->Ordinal();
					}
				}
				else
				{
					OutputError("XpathExpression::Evaluate - The following expression must have two sequences as operands.\n");
				}
			}
		}
		else
		{
			OutputError("XpathExpression::Evaluate - Unrecognised expression type.\n");
		}
	}

	return Result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpressionParenthesised::XpathExpressionParenthesised():
	XpathExpressionPrimary(EXPRESSION_PRIMARY_PARENTHESISED),Expression(0)
{

}

XpathExpressionParenthesised::~XpathExpressionParenthesised()
{

	if (Expression)
		delete Expression;
}

void XpathExpressionParenthesised::Print(String & string)
{
	string << "(";
	Expression->Print(string);
	string << ")";
}

XpathObject * XpathExpressionParenthesised::Evaluate(XpathState & state)
{

	if (Expression)
	{
		XpathObject * object = Expression->Evaluate(state);

		if (Expression->Type.Is(XpathExpression::EXPRESSION_PATH) && ((XpathExpressionStep*)Expression)->IsAxis())
		{

			((XpathSequence*)object)->OrdinalSort();

		}

		return object;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpressionSigned::XpathExpressionSigned():
	XpathExpression(EXPRESSION_SIGNED),Expression(0)
{

}

XpathExpressionSigned::~XpathExpressionSigned()
{

	if (Expression)
		delete Expression;
}

void XpathExpressionSigned::Print(String & string)
{
	string << "-";
	Expression->Print(string);
}

XpathObject * XpathExpressionSigned::Evaluate(XpathState & state)
{
	XpathObject * object =	Expression->Evaluate(state);

	if (object->Type.Is(XpathObject::TYPE_NUMBER))
	{
		if (IsNegative())
		{
			((XpathNumber *)object)->Value *= -1;
		}

		return object;
	}

	return 0;
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathSequence::XpathSequence(Reason::System::Object * object):
	XpathObject(TYPE_SEQUENCE)
{
	Append(object);
}

XpathSequence::XpathSequence(const XpathSequence & sequence):
	XpathObject(TYPE_SEQUENCE)
{
	XpathSequence & seq = (XpathSequence&)sequence;
	for(seq.Forward();seq.Has();seq.Move())
	{
		Append(seq());
	}
}

XpathSequence::XpathSequence():
	XpathObject(TYPE_SEQUENCE)
{

}

XpathSequence::~XpathSequence()
{

	Release();
}

bool XpathSequence::IsHeterogenous()
{

	return Composition.Count() > 1;
}

bool XpathSequence::IsHomogenous()
{

	return Composition.Count() == 1;
}

void XpathSequence::Release()
{

	Table.Release();

	Reason::Structure::Objects::Entry *entry = First;
	while (entry != 0)
	{

		if (entry->Key == 1)
			((XpathObject*)entry->Object)->Decrement();

		entry = entry->Next;
		delete First;
		First = entry;
	}

	Composition = 0;
	Count = 0;
	First = 0;
	Last  = 0;
}

void XpathSequence::Destroy()
{

	Release();
}

void XpathSequence::InsertEntry(Reason::Structure::Objects::Entry *entry,Reason::Structure::Objects::Entry *sentinel, int placement)
{

	Reason::System::Object * object = entry->Object;

	if (object->InstanceOf(XmlObject::Instance))
	{
		XmlObject * xmlObject = (XmlObject*)object;
		if (xmlObject->Type.Is(XmlObject::XML_TEXT))
		{
			Composition.On(COMPOSITION_TEXT);
		}
		else
		if (xmlObject->Type.Is(XmlObject::XML_ELEMENT))
		{
			Composition.On(COMPOSITION_ELEMENT);
		}
		else
		if (xmlObject->Type.Is(XmlObject::XML_MARKUP))
		{
			Composition.On(COMPOSITION_MARKUP);
		}
	}
	else
	if (object->InstanceOf(XmlAttribute::Instance))
	{
		Composition.On(COMPOSITION_ATTRIBUTE);
	}
	else
	if (object->InstanceOf(XmlNamespace::Instance))
	{
		Composition.On(COMPOSITION_NAMESPACE);
	}
	else
	#ifdef REASON_USING_XML_STYLE	
	if (object->InstanceOf(XmlStyle::Instance))
	{
		Composition.On(COMPOSITION_STYLE);
	}
	else
	#endif
	if (object->InstanceOf(XpathObject::Instance))
	{		

		entry->Key = 1;

		XpathObject * xpathObject = (XpathObject*)object;		

		xpathObject->Increment();

		if (xpathObject->Type.Is(XpathObject::TYPE_STRING))
		{
			Composition.On(COMPOSITION_STRING);
		}
		else
		if (xpathObject->Type.Is(XpathObject::TYPE_NUMBER))
		{
			Composition.On(COMPOSITION_NUMBER);
		}
		else
		if (xpathObject->Type.Is(XpathObject::TYPE_BOOLEAN))
		{
			Composition.On(COMPOSITION_BOOLEAN);
		}
		else
		if (xpathObject->Type.Is(XpathObject::TYPE_SEQUENCE))
		{
			Composition.On(COMPOSITION_SEQUENCE);
		}
	}

	List::InsertEntry(entry,sentinel,placement);
}

void XpathSequence::OrdinalSort(int order)
{
	if (!First || !Last)
		return;

	Reason::Structure::Objects::Entry * root = 0;
	Reason::Structure::Objects::Entry * node = 0;

	Reason::Structure::Objects::Entry * min = 0;
	Reason::Structure::Objects::Entry * max = 0;
	Reason::Structure::Objects::Entry * entry = 0;

	root = First;
	First = First->Next;
	root->Prev = 0;
	root->Next = 0;
	min = max = root;

	while (First)
	{
		entry = First;
		First = First->Next;

		entry->Prev = 0;
		entry->Next = 0;

		if (min && OrdinalCompare(entry->Object,min->Object) <= 0)
		{
			min->Prev = entry;
			min = entry;
			continue;
		}

		if (max && OrdinalCompare(entry->Object,max->Object) > 0)
		{
			max->Next = entry;
			max = entry;
			continue;
		}

		node = root;
		while (node)
		{

			int compare = OrdinalCompare(entry->Object,node->Object);
			if (compare <= 0)
			{

				if (node->Prev == 0)
				{
					node->Prev = entry;
					if (node == min)
						min = entry;
					break;
				}

				node = node->Prev;
			}
			else
			{

				if (node->Next == 0)
				{
					node->Next = entry;
					if (node == max)
						max = entry;
					break;
				}

				node = node->Next;
			}
		}
	}

	First = Last = 0;

	Reason::Structure::Array<Reason::Structure::Objects::Entry *> stack;

	stack.Allocate(8);

	node = root;

	if (order == ORDER_ASCENDING)
	{

		while (node->Prev) 
		{
			stack.Append(node);
			node = node->Prev;
		}

		while (node)
		{
			entry = node;

			if (node->Next)
			{
				node = node->Next;
				while (node->Prev) 
				{
					stack.Append(node);
					node = node->Prev;				
				}
			}
			else
			{
				if (stack.Size > 0)
				{
					node = stack.PointerAt(stack.Size-1);
					stack.RemoveAt(stack.Size-1);
				}
				else
				{
					node = 0;
				}
			}

			if (!First)
			{
				First = Last = entry;
				entry->Prev = 0;
				entry->Next = 0;
			}
			else
			{			
				entry->Prev = Last;
				entry->Next = 0;
				Last->Next = entry;
				Last = entry;
			}
		}

	}
	else
	{

		while (node->Next) 
		{
			stack.Append(node);
			node = node->Next;
		}

		while (node)
		{
			entry = node;

			if (node->Prev)
			{
				node = node->Prev;
				while (node->Next) 
				{
					stack.Append(node);
					node = node->Next;				
				}
			}
			else
			{
				if (stack.Size > 0)
				{
					node = stack.PointerAt(stack.Size-1);
					stack.RemoveAt(stack.Size-1);
				}
				else
				{
					node = 0;
				}
			}

			if (!First)
			{
				First = Last = entry;
				entry->Prev = 0;
				entry->Next = 0;
			}
			else
			{			
				entry->Prev = Last;
				entry->Next = 0;
				Last->Next = entry;
				Last = entry;
			}
		}
	}
}

int XpathSequence::OrdinalCompare(Reason::System::Object * left, Reason::System::Object * right)
{
	return Ordinal(left)-Ordinal(right);
}

int XpathSequence::Ordinal(Reason::System::Object * object)
{
	if (object->InstanceOf(XmlObject::Instance))
	{
		return ((XmlObject*)object)->Ordinal();
	}
	else
	if (object->InstanceOf(XmlAttribute::Instance))
	{
		return ((XmlObject*)((XmlAttribute*)object)->Parent)->Ordinal();
	}
	else
	if (object->InstanceOf(XmlNamespace::Instance))
	{
		return ((XmlObject*)((XmlNamespace*)object)->DeclarationElement)->Ordinal();
	}

	return 0;
}

void XpathSequence::Print(String & string)
{
	if (IsEmpty())
	{
		string << "<hv:xpath-sequence count=\"" << Count << "\"/>\n";
	}
	else
	{
		string << "<hv:xpath-sequence count=\"" << Count << "\">\n";

		Reason::Structure::Objects::Entry::Enumerator iterator(*this);

		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			if (iterator()->InstanceOf(XmlMarkup::Instance))
			{
				string << "<hv:xpath-object>\n";
				iterator()->Print(string);
				string << "\n</hv:xpath-object>";
			}
			else
			if (iterator()->InstanceOf(XmlObject::Instance))
			{
				string << "  <hv:xpath-object>\n";
				string << "    ";

				XmlObject *object = (XmlObject*)iterator.Pointer();

				if (object->Type.Is(XmlObject::XML_CDATA))
				{
					((XmlCdata*)iterator.Pointer())->XmlText::Print(string);
				}
				else
				{
					object->Print(string);
				}

				if (object->Type.Is(XmlObject::XML_START))
				{
					string << "\n";

					XmlNavigator enumerator;
					enumerator.NavigateDescendants(object);

					for(enumerator.Forward();enumerator.Has();enumerator.Move())
					{
						String padding;
						for (int i=-3;i<enumerator.Depth();++i)
							padding << "  ";

						string << padding;

						if (enumerator()->Type.Is(XmlObject::XML_CDATA))
						{
							String cdata;

							((XmlCdata*)enumerator.Pointer())->XmlText::Print(cdata);
							padding >> "\n";
							cdata.Replace(String("\n"),padding);
							string << cdata;
						}
						else
						{
							enumerator()->Print(string);
						}

						string << "\n";
					}

					string << "    ";
					object->After->Print(string);
					string << "\n";
				}

				string << "\n  </hv:xpath-object>";
			}
			else
			if (iterator()->InstanceOf(XmlAttribute::Instance))
			{
				string << "<hv:xpath-attribute ";
				((XmlAttribute*)iterator.Pointer())->Print(string);
				string << "/>";
			}
			else
			if (iterator()->InstanceOf(XmlNamespace::Instance))
			{
				string << "<hv:xpath-namespaces ";
				((XmlNamespace*)iterator.Pointer())->Print(string);
				string << "/>";
			}

			string << "\n";
		}

		string << "</hv:xpath-sequence>\n";
	}

}

void XpathSequence::Evaluate(String &result, bool padded)
{

	if (!IsEmpty())
	{
		Reason::Structure::Objects::Entry::Enumerator iterator(*this);
		if (iterator.Forward())
		{
			while (iterator.Has())
			{
				Reason::System::Object * object = iterator.Pointer();

				if (object->InstanceOf(XmlObject::Instance))
				{
					((XmlObject*)object)->Evaluate(result,padded);
					if (!iterator.Move()) break;
					if (padded) result.Append(" ");
				}
				else
				if (object->InstanceOf(XmlAttribute::Instance))
				{
					((XmlAttribute*)object)->Evaluate(result);
					if (!iterator.Move()) break;
					if (padded) result.Append(" ");
				}
				else
				if (object->InstanceOf(XmlNamespace::Instance))
				{

					((XmlNamespace*)object)->Evaluate(result);
					if (!iterator.Move()) break;
					if (padded) result.Append(" ");
				}
				else
				#ifdef REASON_USING_XML_STYLE
				if (object->InstanceOf(XmlStyle::Instance))
				{
					((XmlStyle*)object)->Evaluate(result);
					if (!iterator.Move()) break;
					if (padded) result.Append(" ");
				}
				else
				#endif
				if (object->InstanceOf(XpathObject::Instance))
				{

					XpathString string((XpathObject*)object);
					result.Append(string.Value);
					if (!iterator.Move()) break;
					if (padded) result.Append(" ");
				}
			}
		}
	}

	if (result.Size==0)
		result << "";
}

int XpathSequence::Compare(XpathObject *object, int comparitor)
{
	if (object->Type.Is(XpathObject::TYPE_SEQUENCE))
	{

		XpathSequence *sequence = (XpathSequence *) object;

		if (!IsEmpty())
		{
			if (!sequence->IsEmpty())
			{
				String A;
				Evaluate(A);

				String B;
				sequence->Evaluate(B);

				return A.Compare(&B);

			}
			else
			{
				return 1;
			}
		}
		else
		{
			return -1;
		}
	}
	else
	if (object->Type.Is(XpathObject::TYPE_NUMBER))
	{

		XpathNumber converted;
		return converted.Number(this).Compare(object);

	}
	else
	if (object->Type.Is(XpathObject::TYPE_STRING))
	{

		XpathString string;
		return string.String(this).Compare(object);

	}
	else
	if (object->Type.Is(XpathObject::TYPE_BOOLEAN))
	{

		XpathBoolean boolean;
		return boolean.Boolean(this).Compare(object);
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
XpathBoolean::XpathBoolean(bool value):Value(value)
{
	Type = TYPE_BOOLEAN;
}

XpathBoolean::XpathBoolean(XpathObject *object)
{
	Type = TYPE_BOOLEAN;
	Boolean(object);
}

XpathBoolean::XpathBoolean()
{
	Type = TYPE_BOOLEAN;
}

XpathBoolean::~XpathBoolean()
{

}

void XpathBoolean::Print(String & string)
{
	if (Value)
		string << "true";
	else
		string << "false";
}

XpathBoolean & XpathBoolean::Boolean(XpathObject *object)
{
	if (object->Type.Is(XpathObject::TYPE_NUMBER))
	{

		if ( ((XpathNumber*)object)->Value.IsNan()  || ((XpathNumber*)object)->Value == 0)
		{
			Value = false;
		}
		else
		{
			Value = true;
		}
	}
	else
	if (object->Type.Is(XpathObject::TYPE_BOOLEAN))
	{
		Value = ((XpathBoolean *)object)->Value;
	}
	else
	if (object->Type.Is(XpathObject::TYPE_SEQUENCE))
	{

		if (!((XpathSequence *)object)->IsEmpty())
		{
			Value = true;
		}
		else
		{
			Value = false;
		}
	}
	else
	if (object->Type.Is(XpathObject::TYPE_STRING))
	{

		Value = ((XpathString *)object)->Value.Size > 0;
	}

	return *this;
}

XpathBoolean & XpathBoolean::Not(XpathBoolean *boolean)
{
	Value = !boolean->Value;
	return *this;
}

XpathBoolean & XpathBoolean::True()
{
	Value = true;
	return *this;
}

XpathBoolean & XpathBoolean::False()
{
	Value = false;
	return *this;
}

int XpathBoolean::Compare(XpathObject *object, int comparitor)
{

	if (object->Type.Is(TYPE_BOOLEAN))
	{

		XpathBoolean *booleanObject = (XpathBoolean *)object;

		if (Value == booleanObject->Value)
		{
			return 0;
		}
		else
		{
			return Value - booleanObject->Value;
		}
	}
	else
	if (object->Type.Is(XpathObject::TYPE_STRING))
	{

		XpathBoolean boolean;
		return Compare(&boolean.Boolean(object));
	}
	else
	if (object->Type.Is(XpathObject::TYPE_NUMBER))
	{
		XpathBoolean boolean;
		return Compare(&boolean.Boolean(object));
	}
	else
	if (object->Type.Is(XpathObject::TYPE_SEQUENCE))
	{

		XpathBoolean boolean;
		return Compare(&boolean.Boolean(object));
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathString::XpathString(const Sequence & sequence):
	XpathObject(XpathObject::TYPE_STRING),Value(sequence)
{
}

XpathString::XpathString(XpathObject *object):
	XpathObject(XpathObject::TYPE_STRING)
{
	String(object);
}

XpathString::XpathString()
{
	Type = XpathObject::TYPE_STRING;
}

XpathString::~XpathString()
{

}

XpathString & XpathString::String(XpathObject *object)
{

	if (object->Type.Is(XpathObject::TYPE_NUMBER))
	{

		XpathNumber *number = (XpathNumber *)object;
		if (number->Value.IsNan())
		{
			Value << "NaN";
		}
		else
		if (number->Value.IsNegativeInfinity())
		{
			Value << "-Infinity";
		}
		else
		if (number->Value.IsPositiveInfinity())
		{
			Value << "Infinity";
		}
		else
		{

			Value.Allocate(24);

			if ( number->Value - (float)((int)number->Value) == 0)	
			{

				sprintf(Value.Data,"%d",number->Value.SignedInt());
			}
			else
			{

				sprintf(Value.Data,"%lf",number->Value.Double());
			}
		}
	}
	else
	if (object->Type.Is(TYPE_BOOLEAN))
	{

		if (((XpathBoolean *)object)->Value)
		{
			Value << "true";
		}
		else
		{
			Value << "false";
		}
	}
	else
	if (object->Type.Is(XpathObject::TYPE_SEQUENCE))
	{

		((XpathSequence *)object)->Evaluate(Value);
	}
	else
	if (object->Type.Is(XpathObject::TYPE_STRING))
	{

		Value.Construct(((XpathString*)object)->Value);
	}

	return *this;
}

XpathString & XpathString::Concat(XpathString *first,XpathString *second )
{
	Value << first->Value;

	if (second)
		Value << second->Value;

	return *this;
}

XpathString & XpathString::SubstringBefore(XpathString *target,XpathString *string)
{
	Value.Data = target->Value.Data;
	Value.Size = target->Value.IndexOf(string->Value)+1;

	return *this;
}

XpathString & XpathString::SubstringAfter(XpathString *target,XpathString *string)
{
	Value.Data = target->Value.Data + target->Value.IndexOf(string->Value) + string->Value.Size;
	Value.Size = target->Value.Size - (Value.Data+1 - target->Value.Data);

	return *this;
}

XpathString & XpathString::Substring(XpathString *string,XpathNumber *position,XpathNumber *length )
{
	if (!position->Value.IsNan() && position->Value < string->Value.Size)
	{
		Value.Data = string->Value.Data + (int)position->Value;

		if (length != 0 && !length->Value.IsNan() && length->Value < (int)(string->Value.PointerAt(string->Value.Size) - Value.Data))
		{
			Value.Size = (Value.Data + (int)length->Value)+1 - Value.Data;
		}
		else
		{
			Value.Size = string->Value.PointerAt(string->Value.Size) - Value.Data;
		}
	}

	return *this;
}

XpathString & XpathString::NormaliseSpace(XpathString *string )
{

	if (string)
		Value.Construct(string->Value);

	Value.Trim();
	Value.Replace(' ',0);
	Value.Replace('\t',0);
	Value.Replace('\n',0);
	Value.Replace('\r',0);

	return *this;
}

XpathString & XpathString::Translate(XpathString *string,XpathString *match,XpathString *replacement)
{

	Value.Construct(string->Value);

	for (int i=0;i<match->Value.Size;++i)
	{
		if (i < replacement->Value.Size)
		{

			Value.Replace(match->Value.CharAt(i),replacement->Value.CharAt(i));
		}
		else
		{

			Value.Replace(match->Value.CharAt(i),0);
		}
	}

	return *this;
}

bool XpathString::StartsWith(XpathString *string)
{
	return Value.StartsWith(string->Value);
}

bool XpathString::StartsWith(XpathString *target,XpathString *string)
{
	return target->Value.StartsWith(string->Value);
}

bool XpathString::Contains(XpathString *string)
{
	return Value.Contains(string->Value);
}

bool XpathString::Contains(XpathString *target,XpathString *string)
{
	return target->Value.Contains(string->Value);
}

int XpathString::StringLength()
{
	return Value.Size;
}

int XpathString::StringLength(XpathString *string)
{
	return string->Value.Size;
}

void XpathString::Print(class String & string)
{
	string << "\"" << Value << "\"";
}

int XpathString::Compare(XpathObject *object, int comparitor)
{
	if (object->Type.Is(XpathObject::TYPE_STRING))
	{

		return Value.Compare(&((XpathString*)object)->Value);
	}
	else
	if (object->Type.Is(XpathObject::TYPE_SEQUENCE))
	{

		XpathString string;
		return Compare(&string.String(object));
	}
	else

	if (object->Type.Is(TYPE_BOOLEAN))
	{

		XpathBoolean boolean;
		return boolean.Boolean(this).Compare(object);
	}
	else
	if (object->Type.Is(XpathObject::TYPE_NUMBER))
	{

		XpathNumber number;
		return number.Number(this).Compare(object);
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathNumber::XpathNumber(int number):Value(number)
{
	Type = XpathObject::TYPE_NUMBER;
}

XpathNumber::XpathNumber(double number):Value(number)
{
	Type = XpathObject::TYPE_NUMBER;
}

XpathNumber::XpathNumber(XpathObject *object)
:Value(0.0)
{

	Type = XpathObject::TYPE_NUMBER;
	Number(object);
}

XpathNumber::XpathNumber()
:Value(0.0)
{

	Type = XpathObject::TYPE_NUMBER;
}

XpathNumber::~XpathNumber()
{

}

void XpathNumber::Evaluate(XmlObject *contextObject,int contextPosition,int contextSize,XpathObject *result)
{

	result = new XpathBoolean();
	if (contextPosition == Value)
	{
		((XpathBoolean *)result)->Value = true;
	}
	else
	{
		((XpathBoolean *)result)->Value = false;
	}

}

void XpathNumber::Evaluate(XmlObject *contextObjectParent,XmlNamespace *contextObject,int contextPosition,int contextSize,XpathObject *result)
{

	result = new XpathBoolean();
	if (contextPosition == Value)
	{
		((XpathBoolean *)result)->Value = true;
	}
	else
	{
		((XpathBoolean *)result)->Value = false;
	}

}

void XpathNumber::Evaluate(XmlObject *contextObjectParent,XmlAttribute *contextObject,int contextPosition,int contextSize,XpathObject *result)
{

	result = new XpathBoolean();
	if (contextPosition == Value)
	{
		((XpathBoolean *)result)->Value = true;
	}
	else
	{
		((XpathBoolean *)result)->Value = false;
	}
}

void XpathNumber::Print(String & string)
{

	if ((Value - (int) Value) > 0)
		string << (double)Value;
	else
		string << (int)Value;
}

int XpathNumber::Compare(XpathObject *object, int comparitor)
{
	if (object->Type.Is(XpathObject::TYPE_NUMBER))
	{
		XpathNumber *numberObject = (XpathNumber *)object;

		double dif = (Value - numberObject->Value);

		if (dif == 0)
			return 0;

		if (dif < 0)
			return -1;

		return 1;
	}
	else
	if (object->Type.Is(XpathObject::TYPE_SEQUENCE))
	{

		XpathNumber number;
		return Compare(&number.Number(object));
	}
	else

	if (object->Type.Is(TYPE_BOOLEAN))
	{

		XpathBoolean boolean;
		return boolean.Boolean(this).Compare(object);
	}
	else
	if (object->Type.Is(XpathObject::TYPE_STRING))
	{

		XpathNumber number;
		return Compare(&number.Number(object));
	}
	else
	{
		OutputMessage("XpathNumber::Compare - ERROR: Unrecognised XPath object.\n");
		return false;
	}
}

XpathNumber & XpathNumber::Number(XpathObject *object)
{
	if (object->Type.Is(XpathObject::TYPE_NUMBER))
	{
		Value = ((XpathNumber *)object)->Value;
	}
	else
	if (object->Type.Is(TYPE_BOOLEAN))
	{

		Value = (((XpathBoolean *)object)->Value)?1:0;
	}
	else
	{
		String string;

		if (object->Type.Is(XpathObject::TYPE_SEQUENCE))
		{

			((XpathSequence *)object)->Evaluate(string);
		}
		else
		if (object->Type.Is(XpathObject::TYPE_STRING))
		{

			string.Construct(((XpathString*)object)->Value);
		}

		if (!string.IsEmpty())
		{
			string.Trim();
			if (string.EndsWith('%'))
			{

				Value = Number::Nan();
			}
			else
			{

				Value = string.Real();
			}
		}
		else
		{
			Value = Number::Nan();
		}
	}

	return *this;
}

XpathNumber & XpathNumber::Sum(XpathSequence *sequence)
{

	XpathString string;
	XpathNumber number;

	Reason::Structure::Objects::Entry::Enumerator iterator(*sequence);

	for(iterator.Forward();iterator.Has();iterator.Move())
	{
		if (iterator()->InstanceOf(XmlAttribute::Instance))
		{
			string.Value.Destroy();
			((XmlAttribute*)iterator.Pointer())->Evaluate(string.Value);
			number.Number(&string);

			Value += number.Value;
		}
		else
		if (iterator()->InstanceOf(XmlNamespace::Instance))
		{
			string.Value.Destroy();
			((XmlNamespace*)iterator.Pointer())->Evaluate(string.Value);
			number.Number(&string);

			Value += number.Value;
		}
		else
		if (iterator()->InstanceOf(XmlObject::Instance))
		{
			string.Value.Destroy();
			((XmlObject*)iterator.Pointer())->Evaluate(string.Value);
			number.Number(&string);

			Value += number.Value;
		}
	}

	return *this;
}

XpathNumber & XpathNumber::Floor(XpathNumber *number)
{

	Value = floor(number->Value);

	return *this;
}

XpathNumber & XpathNumber::Ceiling(XpathNumber *number)
{

	Value = ceil(number->Value);

	return *this;
}
XpathNumber & XpathNumber::Round(XpathNumber *number)
{

	Value = Number::Round((double)number->Value);

	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpressionPrimary::XpathExpressionPrimary(unsigned int type):
	XpathExpression(type)
{
}

XpathExpressionPrimary::~XpathExpressionPrimary()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpressionFunction::XpathExpressionFunction():
	XpathExpressionPrimary(EXPRESSION_PRIMARY_FUNCTION)
{

}

XpathExpressionFunction::~XpathExpressionFunction()
{
	Arguments.Destroy();
}

void XpathExpressionFunction::Print(String & string)
{
	string << Token << "(";
	for (int i=0;i<Arguments.Length();++i)
	{
		Arguments[i]->Print(string);
		if (i < Arguments.Length()-1)
			string << ", ";
	}
	string << ")";
}

XpathObject * XpathExpressionFunction::Evaluate(XpathState & state)
{
	if (state() == 0) return 0;

	Release();

	if (Token.Is("trace"))
	{
		if (Arguments.Count == 0)
		{

			OutputMessage("XpathExpressionFunction::Evaluate - trace() printing stack trace.\n");

			int count = state.Count();

			for(int index=count-1;index >= 0;--index)
			{
				if (state[index]->InstanceOf(XmlMarkup::Instance))
				{
					OutputMessage("Stack (%d) - [%08lX] .\n",count,state[index]);
				}
				else
				{					
					String string;
					state[index]->Print(string);
					OutputMessage("Stack (%d) - [%08lX] %s\n",count,state[index],string.Print());
				}

				--count;
			}

			Result = new XpathBoolean(true);
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - trace() function does not take any arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("context"))
	{
		if (Arguments.Count == 0)
		{
			XpathSequence * sequence = new XpathSequence();
			for(int index=state.Count()-1;index >= 0;--index)
			{

				sequence->List::Insert(state[index],sequence->Last);
			}

			Result = sequence;
		}
		else
		if (Arguments.Count == 1)
		{
			XpathObject *argument = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (argument->Type.Is(XpathObject::TYPE_NUMBER))
			{
				XpathSequence * sequence = new XpathSequence();

				int index=((XpathNumber*)argument)->Value.SignedInt();
				sequence->Append(state[state.Count()-index]);

				Result = sequence;
			}
			else
			{
				OutputError("XpathExpressionFunction::Evaluate - context(number) function, argument was not a number.\n");
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - state() function takes 0 or 1 arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("sort"))
	{

		if (Arguments.Count == 1)
		{
			XpathObject *result = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (result && result->Type.Is(XpathObject::TYPE_SEQUENCE) && ((XpathSequence*)result)->IsObjects())
			{

				XpathSequence * sequence = (XpathSequence*)result;
				XpathSequence * sorted = new XpathSequence();

				sorted->Take(*sequence);
				sorted->OrdinalSort();

				Result = sorted;

			}
			else
			{
				OutputError("XpathExpressionFunction::Evaluate - sort(sequence) function, argument was not a valid sequence.\n");
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - sort(sequence) function requires a single argument, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("common-ancestor"))
	{

		if (Arguments.Count == 1)
		{
			XpathObject *result = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (result && result->Type.Is(XpathObject::TYPE_SEQUENCE) && ((XpathSequence*)result)->IsObjects())
			{
				XpathSequence * sequence = (XpathSequence*)result;
				XmlObject *ancestor = (XmlObject*)sequence->First->Object;
				bool found = false;
				while ( ancestor && !found )
				{
					ancestor = ancestor->Parent;
					found = true;
					for (sequence->Forward();sequence->Has();sequence->Move())
					{
						if (!ancestor->Contains((XmlObject*)sequence->Pointer()))
						{
							found = false;
							break;
						}
					}
				}

				if (found)
				{
					Result = new XpathSequence();
					((XpathSequence*)Result)->Append(ancestor);
				}
			}
			else
			{
				OutputError("XpathExpressionFunction::Evaluate - common-ancestor(sequence) function, argument was not a valid homogenous sequence.\n");
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - common-ancestor(sequence) function requires a single argument, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("range"))
	{
		if (Arguments.Count == 1)
		{
			assert(state()->InstanceOf(XmlObject::Instance));

			XpathObject *result = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (result)
			{
				if (result->Type.Is(XpathObject::TYPE_SEQUENCE))
				{

					if ( ((XpathSequence*)result)->IsObjects())
					{
						 XpathSequence *sequence = new XpathSequence();

						XmlObject *range = (XmlObject*)((XpathSequence*)result)->First->Object;
						XmlObject *object = (XmlObject*)state();

						XmlNavigator enumerator;
						if (object->Ordinal() > range->Ordinal())
						{

							enumerator.Navigate(object);
							enumerator.Mode.Off(XmlNavigator::MODE_END);
							enumerator.First(range);

							for(enumerator.Reverse();enumerator.Has();enumerator.Move())
							{
								sequence->Append(enumerator.Pointer());
							}
						}
						else
						{

							enumerator.Navigate(object);
							enumerator.Mode.Off(XmlNavigator::MODE_END);
							enumerator.Last(range);

							for (enumerator.Forward();enumerator.Has();enumerator.Move())
							{
								sequence->Append(enumerator.Pointer());
							}
						}

						Result = sequence;
					}
					else
					{
						OutputError("XpathExpressionFunction::Evaluate - range(sequence) function, argument must resolve to a single atomic sequence.\n");
					}
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - range(sequence) function, argument was not a sequence.\n");
				}
			}
		}
		else
		if (Arguments.Count == 2)
		{
			XpathObject *argumentOne = Arguments[0]->Evaluate(state);

			if (argumentOne && argumentOne->Type.Is(XpathObject::TYPE_SEQUENCE))
			{
				Result = new XpathSequence();

				XpathSequence * sequenceOne = (XpathSequence*) argumentOne;
				XmlMarkup * markup=0;
				XmlMarkup * markupPrevious=0;

				for (sequenceOne->Forward();sequenceOne->Has();sequenceOne->Move())
				{
					if (markupPrevious == 0 || !markupPrevious->Contains((XmlObject*)sequenceOne->Pointer()))
					{					
						XpathContextAuto automatic(state,sequenceOne->Pointer(),state.Position(),state.Size());
						XpathObject *argumentTwo = Arguments[1]->Evaluate(state);

						if (argumentTwo && argumentTwo->Type.Is(XpathObject::TYPE_SEQUENCE))
						{
							if (((XpathSequence*)argumentTwo)->IsObjects())
							{
								markup = new XmlMarkup();
								markup->First = (XmlObject*)sequenceOne->Pointer();
								markup->Last = (XmlObject*)((XpathSequence*)argumentTwo)->First->Object;

								((XpathSequence*)Result)->AppendEntry(new Reason::Structure::Objects::Entry::Auto(markup));
								markupPrevious = markup;
							}
						}
						else
						{
							OutputError("XpathExpressionFunction::Evaluate - range(sequence,sequence) function, one or more arguments was not a sequence.\n");
						}
					}
				}
			}
			else
			{
				OutputError("XpathExpressionFunction::Evaluate - range(sequence,sequence) function, one or more arguments was not a sequence.\n");
			}

		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - range(sequence) function requires a single argument or two arguments, found %d\n",Arguments.Count);
		}
	}
	else

	if (Token.Is("boolean"))
	{
		if (Arguments.Count == 1)
		{
			XpathObject *result = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (result)
			{
				Result = new XpathBoolean();
				((XpathBoolean *)Result)->Boolean(result);
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - boolean(object) function requires a single argument, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("not"))
	{
		if (Arguments.Count == 1)
		{
			XpathExpression * expression = (XpathExpression *)Arguments.First->Pointer();
			if (expression->Type.Is(XpathExpression::EXPRESSION_PATH))
			{

				bool result = ((XpathExpressionStep *)expression)->Match(state);
				Result = new XpathBoolean(!result);
			}
			else

			{
				XpathObject * result = expression->Evaluate(state);
				if (result)
				{
					XpathBoolean boolean;
					boolean.Boolean(result);

					Result = new XpathBoolean();
					((XpathBoolean *)Result)->Not(&boolean);
				}			
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - not(boolean) function requires a single argument, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("true"))
	{
		if (Arguments.Count == 0)
		{
			Result = new XpathBoolean();
			((XpathBoolean *)Result)->Value = true;
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - true() function does not take any arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("false"))
	{
		if (Arguments.Count == 0)
		{
			Result = new XpathBoolean();
			((XpathBoolean *)Result)->Value = false;
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - false() function does not take any arguments, found %d\n",Arguments.Count);
		}

	}
	else
	if (Token.Is("lang"))
	{

		if (Arguments.Count == 1 && state()->InstanceOf(XmlObject::Instance))
		{
			assert(((XmlObject*)state())->Type.Is(XmlObject::XML_ELEMENT));

			XpathObject *argument = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);

			if (argument->Type.Is(XpathObject::TYPE_STRING))
			{

				XmlElement *element = (XmlElement*) state();

				XmlAttribute *attribute;
				String lang;
				lang << "lang";

				while (	element != 0 && (attribute = (XmlAttribute*) element->Attributes.Select(&lang)) == 0 )
				{
					element = (XmlElement*) element->Parent;
				}

				Result = new XpathBoolean();

				if (attribute)
				{
					((XpathBoolean*)Result)->Value = attribute->Value.StartsWith(((XpathString*)argument)->Value);

				}
				else
				{
					((XpathBoolean*)Result)->Value = false;
				}
			}
			else
			{
				OutputError("XpathExpressionFunction::Evaluate - lang(string) function, argument was not a string.\n");
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - lang(string) function takes 1 argument, found %d\n",Arguments.Count);
		}

	}
	else

	if (Token.Is("number"))
	{
		if (Arguments.Count == 1)
		{
			XpathObject *result = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (result)
			{
				Result = new XpathNumber();
				((XpathNumber *)Result)->Number(result);
			}
		}
		else
		if (Arguments.Count == 0)
		{

			XpathSequence resultSet;
			resultSet.Append(state());

			Result = new XpathNumber();
			((XpathNumber *)Result)->Number(&resultSet);
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - number(object) function takes 0 or 1 arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("sum"))
	{
		if (Arguments.Count == 1)
		{
			XpathObject *result = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (result)
			{
				if (result->Type.Is(XpathObject::TYPE_SEQUENCE))
				{
					Result = new XpathNumber();
					((XpathNumber *)Result)->Sum((XpathSequence*) result);
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - sum(sequence) function, argument was not a sequence.\n");
				}
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - sum(sequence) function takes one argument, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("floor"))
	{
		if (Arguments.Count == 1)
		{
			XpathObject *result = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (result)
			{
				if (result->Type.Is(XpathObject::TYPE_NUMBER))
				{
					Result = new XpathNumber();
					((XpathNumber *)Result)->Floor((XpathNumber*)result);
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - floor(number) function, argument was not a number.\n");
				}
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - floor(number) function takes one argument, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("ceiling"))
	{
		if (Arguments.Count == 1)
		{
			XpathObject *result = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (result)
			{
				if (result->Type.Is(XpathObject::TYPE_NUMBER))
				{
					Result = new XpathNumber();
					((XpathNumber *)Result)->Ceiling((XpathNumber*)result);
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - ceiling(number) function, argument was not a number.\n");
				}
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - ceiling(number) function takes one argument, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("round"))
	{
		if (Arguments.Count == 1)
		{
			XpathObject *result = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (result)
			{
				if (result->Type.Is(XpathObject::TYPE_NUMBER))
				{
					Result = new XpathNumber();
					((XpathNumber *)Result)->Round((XpathNumber*)result);
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - round(number) function, argument was not a number.\n");
				}
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - round(number) function takes one argument, found %d\n",Arguments.Count);
		}

	}
	else

	if (Token.Is("translate"))
	{
		if (Arguments.Count == 3)
		{
			XpathObject *argumentOne = Arguments[0]->Evaluate(state);
			XpathObject *argumentTwo = Arguments[1]->Evaluate(state);
			XpathObject *argumentThree = Arguments[2]->Evaluate(state);

			if ( argumentOne && argumentTwo && argumentThree )
			{
				if (argumentOne->Type.Is(XpathObject::TYPE_STRING) && argumentTwo->Type.Is(XpathObject::TYPE_STRING) && argumentThree->Type.Is(XpathObject::TYPE_STRING) )
				{
					Result = new XpathString();
					((XpathString *)Result)->Translate((XpathString*)argumentOne,(XpathString*)argumentTwo,(XpathString*)argumentThree);
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - translate(string,string,string) function, one or more arguments was not a string.\n");
				}
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - translate(string,string,string) function takes three arguments, found %d\n",Arguments.Count);
		}

	}
	else
	if (Token.Is("normalise-space"))
	{
		if (Arguments.Count == 1)
		{
			XpathObject *result = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (result)
			{
				if (result->Type.Is(XpathObject::TYPE_STRING))
				{
					Result = new XpathString();
					((XpathString *)Result)->NormaliseSpace((XpathString*)result);
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - normalise-space(string?) function, argument was not a string.\n");
				}
			}
		}
		else
		if (Arguments.Count == 0)
		{

			Result = new XpathString();
			if (state()->InstanceOf(XmlObject::Instance))
			{
				((XmlObject*)state())->Evaluate(((XpathString *)Result)->Value);
			}
			else
			if (state()->InstanceOf(XmlAttribute::Instance))
			{
				((XmlAttribute*)state())->Evaluate(((XpathString *)Result)->Value);
			}

			((XpathString *)Result)->NormaliseSpace();

		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - normalise-space(string?) function takes 0 or 1 arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("string-length"))
	{
		if (Arguments.Count == 1)
		{
			XpathObject *result = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (result)
			{
				if (result->Type.Is(XpathObject::TYPE_STRING))
				{
					Result = new XpathNumber();
					((XpathNumber *)Result)->Value = ((XpathString *)result)->StringLength();
				}
				else
				{
					Result = new XpathNumber();

					XpathString string;
					string.String(result);

					((XpathNumber *)Result)->Value = string.StringLength();

				}
			}
		}
		else
		if (Arguments.Count == 0)
		{

			if (state())
			{
				XpathString string;

				if (state()->InstanceOf(XmlObject::Instance))
				{
					((XmlObject*)state())->Evaluate(string.Value);
				}
				else
				if (state()->InstanceOf(XmlAttribute::Instance))
				{
					((XmlAttribute*)state())->Evaluate(string.Value);
				}

				Result = new XpathNumber();
				((XpathNumber *)Result)->Value = string.StringLength();
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - string-length(string?) function takes 0 or 1 arguments, found %d\n",Arguments.Count);
		}

	}
	else
	if (Token.Is("substring"))
	{
		if (Arguments.Count == 3)
		{
			XpathObject *argumentOne = Arguments[0]->Evaluate(state);
			XpathObject *argumentTwo = Arguments[1]->Evaluate(state);
			XpathObject *argumentThree = Arguments[2]->Evaluate(state);

			if ( argumentOne && argumentTwo && argumentThree )
			{
				if (argumentOne->Type.Is(XpathObject::TYPE_STRING) && argumentTwo->Type.Is(XpathObject::TYPE_NUMBER) && argumentThree->Type.Is(XpathObject::TYPE_NUMBER) )
				{
					Result = new XpathString();
					((XpathString *)Result)->Substring((XpathString*)argumentOne,(XpathNumber*)argumentTwo,(XpathNumber*)argumentThree);
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - substring(string, number, number?) function, one of the three arguments was not of the correct type.\n");
				}
			}

		}
		else
		if (Arguments.Count == 2)
		{
			XpathObject *argumentOne = Arguments[0]->Evaluate(state);
			XpathObject *argumentTwo = Arguments[1]->Evaluate(state);

			if ( argumentOne && argumentTwo )
			{
				if (argumentOne->Type.Is(XpathObject::TYPE_STRING) && argumentTwo->Type.Is(XpathObject::TYPE_NUMBER))
				{
					Result = new XpathString();
					((XpathString *)Result)->Substring((XpathString*)argumentOne,(XpathNumber*)argumentTwo);
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - substring(string, number, number?) function, one of the two arguments was not of the correct type.\n");
				}
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - substring(string, number, number?)  function takes 2 or 3 arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("substring-after"))
	{
		if (Arguments.Count == 2)
		{
			XpathObject *argumentOne = Arguments[0]->Evaluate(state);
			XpathObject *argumentTwo = Arguments[1]->Evaluate(state);

			if ( argumentOne && argumentTwo )
			{
				if (argumentOne->Type.Is(XpathObject::TYPE_STRING) && argumentTwo->Type.Is(XpathObject::TYPE_STRING))
				{
					Result = new XpathString();
					((XpathString *)Result)->SubstringAfter((XpathString*)argumentOne,(XpathString*)argumentTwo);
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - substring-after(string, string) function, one of the two arguments was not of the correct type.\n");
				}
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - substring-after(string, string) function takes 2 arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("substring-before"))
	{
		if (Arguments.Count == 2)
		{
			XpathObject *argumentOne = Arguments[0]->Evaluate(state);
			XpathObject *argumentTwo = Arguments[1]->Evaluate(state);

			if ( argumentOne && argumentTwo )
			{
				if (argumentOne->Type.Is(XpathObject::TYPE_STRING) && argumentTwo->Type.Is(XpathObject::TYPE_STRING))
				{
					Result = new XpathString();
					((XpathString *)Result)->SubstringBefore((XpathString*)argumentOne,(XpathString*)argumentTwo);
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - substring-before(string, string) function, one of the two arguments was not of the correct type.\n");
				}
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - substring-before(string, string) function takes 2 arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("contains"))
	{
		if (Arguments.Count == 1)
		{
			XpathString string;
			if (state()->InstanceOf(XmlObject::Instance))
			{
				((XmlObject*)state())->Evaluate(string.Value);
			}
			else
			if (state()->InstanceOf(XmlAttribute::Instance))
			{
				((XmlAttribute*)state())->Evaluate(string.Value);
			}

			XpathObject *argumentOne = Arguments[0]->Evaluate(state);
			if (argumentOne)
			{
				XpathString stringOne(argumentOne);
				Result = new XpathBoolean(string.Contains(&stringOne));
			}
		}
		else
		if (Arguments.Count == 2)
		{
			XpathObject *argumentOne = Arguments[0]->Evaluate(state);
			XpathObject *argumentTwo = Arguments[1]->Evaluate(state);

			if ( argumentOne && argumentTwo )
			{
				XpathString stringOne(argumentOne);
				XpathString stringTwo(argumentTwo);
				Result = new XpathBoolean(stringOne.Contains(&stringTwo));

			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - contains(string, string) function takes 1 or 2 arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("lower") || Token.Is("lower-case") || Token.Is("upper") || Token.Is("upper-case"))
	{

		if (Arguments.Count == 0)
		{
			XpathString string;
			if (state()->InstanceOf(XmlObject::Instance))
			{
				((XmlObject*)state())->Evaluate(string.Value);
			}
			else
			if (state()->InstanceOf(XmlAttribute::Instance))
			{
				((XmlAttribute*)state())->Evaluate(string.Value);
			}

			if (Token.StartsWith("lower"))
				string.Value.Lowercase();
			else
				string.Value.Uppercase();

			Result = new XpathString(&string);

		}
		else
		if (Arguments.Count == 1)
		{
			XpathObject *argumentOne = Arguments[0]->Evaluate(state);

			if ( argumentOne )
			{
				XpathString string(argumentOne);				
				if (Token.StartsWith("lower"))
					string.Value.Lowercase();
				else
					string.Value.Uppercase();

				Result = new XpathString(&string);
			}
		}
		else
		{
			if (Token.StartsWith("lower"))
				OutputError("XpathExpressionFunction::Evaluate - lower-case(string) function takes 0 or 1 arguments, found %d\n",Arguments.Count);
			else
				OutputError("XpathExpressionFunction::Evaluate - upper-case(string) function takes 0 or 1 arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("starts-with") || Token.Is("ends-with"))
	{
		if (Arguments.Count == 1)
		{
			XpathString string;
			if (state()->InstanceOf(XmlObject::Instance))
			{
				((XmlObject*)state())->Evaluate(string.Value);
			}
			else
			if (state()->InstanceOf(XmlAttribute::Instance))
			{
				((XmlAttribute*)state())->Evaluate(string.Value);
			}

			XpathObject *argumentOne = Arguments[0]->Evaluate(state);
			if (argumentOne)
			{
				XpathString stringOne(argumentOne);
				if (Token.Is("starts-with"))
					Result = new XpathBoolean(string.Value.StartsWith(stringOne.Value));
				else
					Result = new XpathBoolean(string.Value.EndsWith(stringOne.Value));

			}
		}
		else
		if (Arguments.Count == 2)
		{
			XpathObject *argumentOne = Arguments[0]->Evaluate(state);
			XpathObject *argumentTwo = Arguments[1]->Evaluate(state);

			if ( argumentOne && argumentTwo )
			{
				XpathString stringOne(argumentOne);
				XpathString stringTwo(argumentTwo);
				if (Token.Is("starts-with"))
					Result = new XpathBoolean(stringOne.Value.StartsWith(stringTwo.Value));
				else
					Result = new XpathBoolean(stringOne.Value.EndsWith(stringTwo.Value));

			}
		}
		else
		{
			if (Token.Is("starts-with"))
				OutputError("XpathExpressionFunction::Evaluate - starts-with(string, string) function takes 1 or 2 arguments, found %d\n",Arguments.Count);
			else
				OutputError("XpathExpressionFunction::Evaluate - ends-with(string, string) function takes 1 or 2 arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("concat"))
	{
		if (Arguments.Count > 2)
		{

			Result = new XpathString();

			for (int i=0;i<Arguments.Count;++i)
			{
				XpathObject *argument = Arguments[i]->Evaluate(state);

				if (argument && argument->Type.Is(XpathObject::TYPE_STRING))
				{
					((XpathString *)Result)->Concat((XpathString*)argument);
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - concat(string, string, string*) function, argument %d was not a string.\n",i);
					Release();
					break;
				}
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - concat(string, string, string*) function takes at least 2 arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("string"))
	{
		if (Arguments.Count == 1)
		{
			XpathObject *result = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (result)
			{
				Result = new XpathString();
				((XpathString *)Result)->String(result);
			}
		}
		else
		if (Arguments.Count == 0)
		{

			XpathSequence resultSet;
			resultSet.Append(state());

			Result = new XpathString();
			((XpathString *)Result)->String(&resultSet);
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - string(object?)  function takes 0 or 1 arguments, found %d\n",Arguments.Count);
		}
	}
	else

	if (Token.Is("last"))
	{
		if (Arguments.Count == 0)
		{

			Result = new XpathNumber();
			((XpathNumber *)Result)->Value = state.Size();
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - last() function takes 0 arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("position"))
	{
		if (Arguments.Count == 0)
		{

			Result = new XpathNumber();
			((XpathNumber *)Result)->Value = state.Position();
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - position() function takes 0 arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("count"))
	{

		if (Arguments.Count == 1)
		{
			XpathObject *result = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (result)
			{
				if (result->Type.Is(XpathObject::TYPE_SEQUENCE))
				{
					Result = new XpathNumber();
					((XpathNumber *)Result)->Value += ((XpathSequence *)result)->Count;

				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - count(sequence) function, argument was not a sequence.\n");
				}
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - count(sequence) function takes 1 arguments, found %d.\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("id"))
	{

		if (Arguments.Count == 1)
		{
			XpathObject *argument = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (argument)
			{

				XmlObject *xmlObject;

				if (state()->InstanceOf(XmlObject::Instance))
				{
					xmlObject = ((XmlObject*)state())->Parent;
				}
				else
				if (state()->InstanceOf(XmlAttribute::Instance))
				{
					xmlObject = ((XmlAttribute*)state())->Parent;
				}

				while (xmlObject->Parent != 0)
					xmlObject = xmlObject->Parent;

				XmlDocument *document = (XmlDocument*)xmlObject;

				XpathSequence *resultSet = new XpathSequence();

				if (document->DoctypeDeclaration != 0 && document->DoctypeDeclaration->Dtd.IsEmpty())
				{

					if (argument->Type.Is(XpathObject::TYPE_SEQUENCE))
					{
						Path sequence;

						Reason::Structure::Objects::Entry::Enumerator iterator(*(XpathSequence*)argument);

						for (iterator.Forward();iterator.Has();iterator.Move())
						{

							if (iterator()->InstanceOf(XmlObject::Instance))
							{
								((XmlObject*)iterator.Pointer())->Evaluate(sequence);
							}
							else
							if (iterator()->InstanceOf(XmlAttribute::Instance))
							{
								((XmlAttribute*)iterator.Pointer())->Evaluate(sequence);
							}
						}

						sequence.Tokenise("\n\r\t ");	

						Path::Enumerator stringIterator(sequence);
						for (stringIterator.Forward();stringIterator.Has();stringIterator.Move())
						{
							XmlId *id = (XmlId*) document->Identifiers.Select(stringIterator.Pointer());
							if (id)
							{
								resultSet->Append(id->Element);
							}
						}

					}
					else
					{
						XpathString string;
						string.String(argument);

						Path sequence;
						sequence.Construct(string.Value);
						sequence.Tokenise("\n\r\t ");	

						Path::Enumerator iterator(sequence);
						for (iterator.Forward();iterator.Has();iterator.Move())
						{
							XmlId *id = (XmlId*) document->Identifiers.Select(iterator.Pointer());
							if (id)
							{
								resultSet->Append(id->Element);
							}
						}

					}

				}

				Result = resultSet;

			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - count(sequence) function takes 0 arguments, found %d\n",Arguments.Count);
		}
	}
	else
	if (Token.Is("local-name"))
	{

		String uri; 

		if (Arguments.Count == 1)
		{
			XpathObject *argument = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (argument)
			{
				if (argument->Type.Is(XpathObject::TYPE_SEQUENCE))
				{
					Result = new XpathString();

					if (((XpathSequence*)argument)->IsObjects())
					{
						((XmlObject*)((XpathSequence*)argument)->First->Object)->ExpandedName(((XpathString*)Result)->Value,uri);
					}
					else
					if (((XpathSequence*)argument)->IsAttributes())
					{
						((XmlAttribute*)((XpathSequence*)argument)->First->Object)->ExpandedName(((XpathString*)Result)->Value,uri);
					}
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - local-name(sequence?) function, argument was not a sequence.\n");
				}
			}
		}
		else
		if (Arguments.Count == 0)
		{
			Result = new XpathString();

			if (state()->InstanceOf(XmlObject::Instance))
			{
				((XmlObject*)state())->ExpandedName(((XpathString*)Result)->Value,uri);
			}
			else
			if (state()->InstanceOf(XmlAttribute::Instance))
			{
				((XmlAttribute*)state())->ExpandedName(((XpathString*)Result)->Value,uri);
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - local-name(sequence?) function takes 0 or 1 argument, found %d\n",Arguments.Count);
		}

	}
	else
	if (Token.Is("namespace-uri"))
	{

		String name; 

		if (Arguments.Count == 1)
		{
			XpathObject *argument = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (argument)
			{
				if (argument->Type.Is(XpathObject::TYPE_SEQUENCE))
				{
					Result = new XpathString();

					if (!((XpathSequence*)argument)->IsObjects())
					{
						((XmlObject*)((XpathSequence*)argument)->First->Object)->ExpandedName(name,((XpathString*)Result)->Value);
					}
					else
					if (!((XpathSequence*)argument)->IsAttributes())
					{

						((XmlObject*)((XpathSequence*)argument)->First->Object)->ExpandedName(name,((XpathString*)Result)->Value);
					}
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - namespace-uri(sequence?) function, argument was not a sequence.\n");
				}
			}
		}
		else
		if (Arguments.Count == 0)
		{
			Result = new XpathString();

			if (state()->InstanceOf(XmlObject::Instance))
			{
				((XmlObject*)state())->ExpandedName(name,((XpathString*)Result)->Value);
			}
			else
			if (state()->InstanceOf(XmlAttribute::Instance))
			{
				((XmlAttribute*)state())->ExpandedName(name,((XpathString*)Result)->Value);
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - namespace-uri(sequence?) function takes 0 or 1 argument, found %d\n",Arguments.Count);
		}

	}
	else
	if (Token.Is("name"))
	{

		if (Arguments.Count == 1)
		{
			XpathObject *argument = ((XpathExpression *)Arguments.First->Pointer())->Evaluate(state);
			if (argument)
			{
				if (argument->Type.Is(XpathObject::TYPE_SEQUENCE))
				{
					Result = new XpathString();

					if (((XpathSequence*)argument)->IsObjects())
					{
						XmlObject * object = (XmlObject*)((XpathSequence*)argument)->First->Object;
						if (object->Type.Is(XmlObject::XML_ELEMENT))
						{
							XmlElement * element = (XmlElement *)object;

							if ( element->Namespace != 0)
							{
								((XpathString*)Result)->Value << element->Namespace->Prefix << ":";
							}
							((XpathString*)Result)->Value << element->Token;
						}
					}
					else
					if (((XpathSequence*)argument)->IsAttributes())
					{
						XmlAttribute *attribute = (XmlAttribute*)((XpathSequence*)argument)->First->Object;

						if ( attribute->Namespace != 0)
						{
							((XpathString*)Result)->Value << attribute->Namespace->Prefix << ":";
						}
						((XpathString*)Result)->Value << attribute->Name;
					}
				}
				else
				{
					OutputError("XpathExpressionFunction::Evaluate - name(sequence?) function, argument was not a sequence.\n");
				}
			}
		}
		else
		if (Arguments.Count == 0)
		{
			Result = new XpathString();

			if (state()->InstanceOf(XmlObject::Instance))
			{
				if (((XmlObject*)state())->Type.Is(XmlObject::XML_ELEMENT))
				{
					XmlElement * element = (XmlElement *)state();
					if ( element->Namespace != 0)
					{
						((XpathString*)Result)->Value << element->Namespace->Prefix << ":";
					}
					((XpathString*)Result)->Value << element->Token;
				}
			}
			else
			if (state()->InstanceOf(XmlAttribute::Instance))
			{
				XmlAttribute * attribute = (XmlAttribute *)state();
				if ( attribute->Namespace != 0 )
				{
					((XpathString*)Result)->Value << attribute->Namespace->Prefix << ":";
				}
				((XpathString*)Result)->Value << attribute->Name;
			}
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - name(sequence?) function takes 0 or 1 argument, found %d\n",Arguments.Count);
		}
	}
	else
	if (state.Path->Extension)
	{
		XpathExtensionHandlerHandler * handler = state.Path->Extension->Lookup(XpathExtension::EXTENSION_FUNCTION,Token);
		if (handler)
		{
			return handler->ProcessFunction(this,state);
		}
		else
		{
			OutputError("XpathExpressionFunction::Evaluate - Unrecognised function name.\n");
		}

	}
	else
	{
		OutputError("XpathExpressionFunction::Evaluate - Unrecognised function name.\n");
	}

	return Result;

}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpressionStepFilter::XpathExpressionStepFilter():
	XpathExpressionStep(STEP_FILTER),Expression(0)
{

}

XpathExpressionStepFilter::XpathExpressionStepFilter(XpathExpression * expression):
	XpathExpressionStep(STEP_FILTER),Expression(expression)
{

}

XpathExpressionStepFilter::~XpathExpressionStepFilter()
{
	if (Expression)
		delete Expression;
}

void XpathExpressionStepFilter::Print(String & string)
{
	Expression->Print(string);

	for (int i=0;i<Predicates.Length();++i)
		Predicates[i]->Print(string);

	if (Next)
	{
		string << "/";
		Next->Print(string);
	}
}

void XpathExpressionStepFilter::Select(XpathSequence & sequence, XpathState & state, bool match)
{

	if (state() == 0)
		return;

	XpathObject * object = Expression->Evaluate(state);
	if (object == 0) return;

	if (object->Type.Is(XpathObject::TYPE_SEQUENCE))
	{

		Reason::Structure::Objects::Entry::Enumerator iterator(*(XpathSequence*)object);
		for (iterator.Forward();iterator.Has();iterator.Move())
		{
			XpathContextAuto automatic(state,iterator.Pointer(),iterator.Index()+1,iterator.Length());
			if (Predicates.Match(state))
			{
				if (Next)
				{
					Next->Select(sequence,state,match);
				}
				else

				{
					sequence.Append(iterator.Pointer());
				}
			}

			if (state.Position() == Predicates.ContextPosition || state.Position() == Predicates.ContextPositionMax)
				break;
		}

	}
	else
	if (object->Type.Is(XpathObject::TYPE_NUMBER))
	{

		if (state()->InstanceOf(XmlObject::Instance))
		{
			int index = (int)((XpathNumber*)object)->Value;
			int position=0;
			XmlNavigator enumerator;
			enumerator.NavigateChildren((XmlObject*)state());
			for(enumerator.Forward();enumerator.Has();enumerator.Move())
			{
				++position;
				if (position == index)
				{
					XpathContextAuto automatic(state,enumerator.Pointer(),position,0);
					if (Predicates.Match(state))
					{
						if (Next)
						{
							Next->Select(sequence,state,match);
						}
						else

						{
							sequence.Append(enumerator.Pointer());
						}
					}
					break;
				}
			}
		}
		else
		{
			OutputError("XpathExpressionStepFilter::Select - Type coercion not available with this state.\n");
		}
	}
	else
	if (object->Type.Is(XpathObject::TYPE_STRING))
	{
		if (state()->InstanceOf(XmlObject::Instance))
		{
			String *name = &((XpathString*)object)->Value;
			int position=0;

			XmlNavigator enumerator;
			enumerator.NavigateChildren((XmlObject*)state());
			for(enumerator.Forward();enumerator.Has();enumerator.Move())
			{
				if (enumerator.Pointer()->Token.Is(*name,true))
				{
					++position;
					XpathContextAuto automatic(state,enumerator.Pointer(),position,0);
					if (Predicates.Match(state))
					{
						if (Next)
						{
							Next->Select(sequence,state,match);
						}
						else

						{
							sequence.Append(enumerator.Pointer());
						}
					}
				}
			}

		}
		else
		{
			OutputError("XpathExpressionStepFilter::Select - Type coercion not available with this state.\n");
		}

	}
	else
	{
		OutputError("XpathExpressionStepFilter::Select - The filter expression returned an invalid object type, boolean is not supported as a step.\n");
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpressionStep::XpathExpressionStep():
	Next(0),Prev(0),Step(0),XpathExpression(EXPRESSION_PATH)
{

}

XpathExpressionStep::XpathExpressionStep(unsigned char step):
	Next(0),Prev(0),Step(step),XpathExpression(EXPRESSION_PATH)
{

}

XpathExpressionStep::~XpathExpressionStep()
{
	if (Next)
		delete Next;
}

XpathExpressionStep * XpathExpressionStep::Append(XpathExpressionStep * step)
{
	if (Next)
	{
		XpathExpressionStep *last = step;
		while (last->Next)
			last = last->Next;

		Next->Prev = last;
		last->Next = Next;
	}

	step->Prev = this;
    Next = step;
	return Next;
}

XpathExpressionStep * XpathExpressionStep::Prepend(XpathExpressionStep * step)
{
	if (Prev)
	{
		Prev->Next = step;
		step->Prev = Prev;
	}

	XpathExpressionStep *last = step;
	while (last->Next)
		last = last->Next;

	last->Next = this;
	Prev = last;
	return Prev;
}

XpathObject * XpathExpressionStep::Evaluate(XpathState & state)
{

	Release();
	Result = new XpathSequence();
	Select(*(XpathSequence*)Result,state);

	XpathExpressionStep * step = this;
	while (step->Next) step = step->Next;
	if (step->Step == XpathExpressionStep::STEP_AXIS)
	{

		((XpathSequence*)Result)->OrdinalSort();

	}
	else
	if (step->Step == XpathExpressionStep::STEP_FILTER)
	{

		((XpathSequence*)Result)->OrdinalSort();
	}

	return Result;
}

bool XpathExpressionStep::Match(XpathState & state)
{
	XpathSequence sequence;
	Select(sequence,state,true);
	return ! sequence.IsEmpty();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpressionStepScope::XpathExpressionStepScope():
	XpathExpressionStep(STEP_SCOPE) 
{
}

XpathExpressionStepScope::~XpathExpressionStepScope() 
{
}

void XpathExpressionStepScope::Select(XpathSequence &sequence, XpathState & state, bool match)
{
	if (Next)
	{

		Object * object = state();

		if (object->InstanceOf(XmlObject::Instance))
		{

			state.Scopes.Append(XpathScope((XmlObject*)object));
			Next->Select(sequence,state,match);
			state.Scopes.RemoveAt(state.Scopes.Size-1);

		}
		else
		{
			Next->Select(sequence,state,match);
		}
	}
	else

	{
		sequence.Append(state());
	}
}

void XpathExpressionStepScope::Print(String & string)
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpressionStepAxis::XpathExpressionStepAxis():
	XpathExpressionStep(STEP_AXIS), Axis(0),Test(0)
{

}

XpathExpressionStepAxis::XpathExpressionStepAxis(unsigned short axis, unsigned char test):
	XpathExpressionStep(STEP_AXIS), Axis(axis),Test(test)
{

}

XpathExpressionStepAxis::~XpathExpressionStepAxis()
{
	Arguments.Destroy();	
}

void XpathExpressionStepAxis::Print(String & string)
{

	if (Axis.Type.Is(AXIS_ATTRIBUTE))
	{
		string << "attribute::";
	}
	else
	if (Axis.Type.Is(AXIS_NAMESPACE))
	{
		string << "namespace::";
	}
	else
	if (Axis.Type.Is(AXIS_CHILD))
	{
		string << "child::";
	}
	else
	if (Axis.Type.Is(AXIS_ANCESTOR))
	{
		string << "ancestor::";
	}
	else
	if (Axis.Type.Is(AXIS_ANCESTOR_OR_SELF))
	{
		string << "ancestor-or-self::";
	}
	else
	if (Axis.Type.Is(AXIS_DESCENDANT))
	{
		string << "descendant::";
	}
	else
	if (Axis.Type.Is(AXIS_DESCENDANT_OR_SELF))
	{
		string << "descentant-or-self::";
	}
	else
	if (Axis.Type.Is(AXIS_FOLLOWING))
	{
		string << "following::";
	}
	else
	if (Axis.Type.Is(AXIS_FOLLOWING_SIBLING))
	{
		string << "following-sibling::";
	}
	else
	if (Axis.Type.Is(AXIS_PARENT))
	{
		string << "parent::";
	}
	else
	if (Axis.Type.Is(AXIS_PRECEDING))
	{
		string << "preceding::";
	}
	else
	if (Axis.Type.Is(AXIS_PRECEDING_SIBLING))
	{
		string << "preceding-sibling::";
	}
	else
	if (Axis.Type.Is(AXIS_SELF))
	{
		string << "self::";
	}

	if (Test.Type.Is(TEST_NAME_ALL))
	{
		string << "*";
	}
	else
	if (Test.Type.Is(TEST_NAME_NAMESPACE))
	{

		string << Test;
	}
	else
	if (Test.Type.Is(TEST_NAME_QUALIFIED))
	{
		string << Test;
	}
	else
	if (Test.Type.Is(TEST_NODE_COMMENT))
	{
		string << "comment()";
	}
	else
	if (Test.Type.Is(TEST_NODE_TEXT))
	{
		string << "text()";
	}
	else
	if (Test.Type.Is(TEST_NODE_PROCESSING))
	{

		string << "instruction()";
	}
	else
	if (Test.Type.Is(TEST_NODE_ALL))
	{

		string << "node()";
	}

	for (int i=0;i<Predicates.Length();++i)
		Predicates[i]->Print(string);

	if (Next)
	{
		string << "/";
		Next->Print(string);
	}
}

void XpathExpressionStepAxis::Select(XpathSequence & sequence, XpathState & state, bool match)
{

	if (state() == 0) return;

	if (state.Path->Extension)
	{

		XpathExtensionHandlerHandler * handler = state.Path->Extension->Lookup(XpathExtension::EXTENSION_STEP_AXIS,"");
		if (handler)
		{
			if (!handler->ProcessAxis(this,sequence,state,match))
				return;
		}

		if (!Axis.IsEmpty())
		{
			handler = state.Path->Extension->Lookup(XpathExtension::EXTENSION_STEP_TEST,Axis);
			if (handler)
			{
				handler->ProcessAxis(this,sequence,state,match);
				return;
			}
		}
	}

	switch(state.Type())
	{
	case XpathContext::TYPE_OBJECT:

		if (((XmlObject*)state())->Type.Is(XmlObject::XML_TEXT) || ((XmlObject*)state())->Type.Is(XmlObject::XML_COMMENT))
			switch(Axis.Type)
			{
			case XpathExpressionStepAxis::AXIS_CHILD:
			case XpathExpressionStepAxis::AXIS_DESCENDANT:
			case XpathExpressionStepAxis::AXIS_TYPE_ATTRIBUTE:

				return;
			default:
				break;
			}
		else
		if (((XmlObject*)state())->Type.Is(XmlObject::XML_EMPTY))
			switch(Axis.Type)
			{
			case XpathExpressionStepAxis::AXIS_CHILD:
			case XpathExpressionStepAxis::AXIS_DESCENDANT:

				return;
			default:
				break;
			}

	case XpathContext::TYPE_MARKUP:
		SelectObject(sequence,state,match);
		break;
	case XpathContext::TYPE_ATTRIBUTE:
		SelectAttribute(sequence,state,match);
		break;
	case XpathContext::TYPE_STYLE:
		SelectStyle(sequence,state,match);
		break;
	case XpathContext::TYPE_NAMESPACE:
		SelectNamespace(sequence,state,match);
		break;
	}
}

bool XpathExpressionStepAxis::CanSelectIndexed(XpathState & state)
{

	if (Next && Next->Step == XpathExpressionStep::STEP_AXIS && 
		state()->InstanceOf(XmlDocument::Instance) && ((XmlDocument*)state())->Index && 
		Predicates.Length() == 0)
	{
		XpathExpressionStepAxis * axis = (XpathExpressionStepAxis *)Next;

		if (!axis->Predicates.ContextDimensions && axis->Axis.Type.Is(XpathExpressionStepAxis::AXIS_TYPE_ELEMENT))
		{
			switch(axis->Test.Type)
			{
			case TEST_NAME_ALL: TEST_NAME_NAMESPACE: TEST_NAME_QUALIFIED: TEST_NODE_TEXT:
				return true;
			}
		}
	}

	return false;
}

void XpathExpressionStepAxis::SelectIndexed(XpathSequence &sequence, XpathState & state, bool match)
{

	int position = 0;

	XmlIndex * xmlIndex = ((XmlDocument*)state())->Index;
	XpathExpressionStepAxis * axis = (XpathExpressionStepAxis *)Next;

	switch (axis->Test.Type)
	{
	case TEST_NAME_ALL: TEST_NAME_NAMESPACE:
		{

			Reason::Structure::Objects::Entry::Enumerator iterator(xmlIndex->ElementList);
			for(iterator.Forward();iterator.Has();iterator.Move())
			{
				if (axis->Match(state, (XmlObject*)iterator()))
				{
					++position;
					XpathContextAuto automatic(state,iterator(),position,0);
					if (axis->Next)
					{
						if (axis->Predicates.Match(state))
							axis->Next->Select(sequence,state,match);
					}
					else
					{
						if (sequence.Select(iterator())==0 && axis->Predicates.Match(state))
							sequence.Append(iterator());
					}

					if (match && !sequence.IsEmpty() || position == axis->Predicates.ContextPosition || position == axis->Predicates.ContextPositionMax)
						break;
				}
			}
		}
		break;
	case TEST_NAME_QUALIFIED:
		{

			int i = axis->Test.IndexOf(":");
			if (i != -1)
			{
				Substring ns;
				ns.Assign(axis->Test.Data,axis->Test.Data+i-1);
				Substring name;
				name.Assign(axis->Test.Data+i+1,axis->Test.Data+axis->Test.Size-1);

				Index<Object*> * index = xmlIndex->ElementCatalogue.Select(name,true);
				if (index)
				{

					Iterator<Object*> iterator(index->List);
					for(iterator.Forward();iterator.Has();iterator.Move())
					{

						if (axis->Match(state, (XmlObject*)iterator()) && ((XmlElement*)iterator())->Namespace->Prefix.Is(ns,true) )
						{
							++position;
							XpathContextAuto automatic(state,iterator(),position,0);
							if (axis->Next)
							{
								if (axis->Predicates.Match(state))
									axis->Next->Select(sequence,state,match);
							}
							else
							{
								if (sequence.Select(iterator())==0 && axis->Predicates.Match(state))
									sequence.Append(iterator());
							}
						}

						if (match && !sequence.IsEmpty() || position == axis->Predicates.ContextPosition || position == axis->Predicates.ContextPositionMax)
							break;
					}
				}
			}
			else
			{

				Index<Object*> * index = xmlIndex->ElementCatalogue.Select(axis->Test,true);
				if (index)
				{
					Iterator<Object*> iterator(index->List);
					for(iterator.Forward();iterator.Has();iterator.Move())
					{
						if (axis->Match(state, (XmlObject*)iterator()))
						{
							++position;

							XpathContextAuto automatic(state,iterator(),position,0);
							if (axis->Next)
							{
								if (axis->Predicates.Match(state))
									axis->Next->Select(sequence,state,match);
							}
							else
							{
								if (sequence.Select(iterator())==0 && axis->Predicates.Match(state))
									sequence.Append(iterator());
							}
						}

						if (match && !sequence.IsEmpty() || position == axis->Predicates.ContextPosition || position == axis->Predicates.ContextPositionMax)
							break;
					}
				}
			}
		}
		break;
	case TEST_NODE_ALL:
		{

		}
		break;
	case TEST_NODE_COMMENT:
		{

		}
		break;
	case TEST_NODE_PROCESSING:
		{

		};
		break;
	case TEST_NODE_TEXT:
		{
			Reason::Structure::Objects::Entry::Enumerator iterator(xmlIndex->TextList);
			for(iterator.Forward();iterator.Has();iterator.Move())
			{
				if (axis->Match(state, (XmlObject*)iterator()))
				{
					++position;
					if (! axis->Predicates.ContextDimensions)
					{
						XpathContextAuto automatic(state,iterator(),position,0);
						if (axis->Next)
						{
							if (axis->Predicates.Match(state))
								axis->Next->Select(sequence,state,match);
						}
						else
						if (sequence.Select(iterator())==0 && axis->Predicates.Match(state))
						{					
							sequence.Append(iterator());
						}
					}

					if (match && !sequence.IsEmpty() || position == axis->Predicates.ContextPosition || position == axis->Predicates.ContextPositionMax)
						break;
				}
			}
		};
		break;
	}
}

void XpathExpressionStepAxis::SelectObject(XpathSequence &sequence, XpathState & state, bool match)
{

	if(Axis.Type.Is(AXIS_TYPE_ELEMENT))
	{
		XmlCollection *collection=0;

		if (Predicates.ContextDimensions )
			collection = new XmlCollection();

		int position=0;

		switch (Axis.Type)
		{
		case XpathExpressionStepAxis::AXIS_ROOT:
			{

				XmlObject *object = state.Root();

				if (Next)
				{
					XpathContextAuto automatic(state,object,1,1);
					Next->Select(sequence,state,match);
				}
				else

				{
					sequence.Append(object);
				}

			}
			break;
		case XpathExpressionStepAxis::AXIS_SELF:
			{

				if (Match(state, ((XmlObject*)state())))
				{
					XpathContextAuto automatic(state,state(),1,1);
					if (Predicates.Match(state))
					{	
						if (Next)
						{
							Next->Select(sequence,state,match);
						}
						else

						{
							sequence.Append(state());
						}

						if(match && !sequence.IsEmpty())
							return;
					}
				}
			}
			break;
		case XpathExpressionStepAxis::AXIS_CHILD:
			{

				XmlNavigator enumerator;
				enumerator.NavigateChildren(((XmlObject*)state()));

				for(enumerator.Forward();enumerator.Has();enumerator.Move())
				{
					if (state.Scope() && !((XmlObject*)state.Scope()->Object)->Contains(enumerator()))
						break;

					if (Match(state, enumerator()))
					{
						++position;
						if (! Predicates.ContextDimensions)
						{							
							XpathContextAuto automatic(state,enumerator(),position,0);
							if (Predicates.Match(state))
							{
								if (Next)
								{
									Next->Select(sequence,state,match);
								}
								else

								{
									sequence.Append(enumerator());
								}
							}		

							if(match && !sequence.IsEmpty())
								return;							
						}
						else
						{
							collection->Append(enumerator());
						}

						if (position == Predicates.ContextPosition || position == Predicates.ContextPositionMax)
							break;
					}
				}
			}
			break;
		case XpathExpressionStepAxis::AXIS_ANCESTOR_OR_SELF:
			{

				if (Match(state, ((XmlObject*)state())))
				{
					++position;

					if (! Predicates.ContextDimensions)
					{			
						XpathContextAuto automatic(state,state(),position,0);
						if (Predicates.Match(state))
						{
							if (Next)
							{
								Next->Select(sequence,state,match);
							}
							else

							{
								sequence.Append(state());
							}

							if(match && !sequence.IsEmpty())
								return;
						}
					}
					else
					{
						collection->Append(((XmlObject*)state()));
					}
				}

			}
		case XpathExpressionStepAxis::AXIS_ANCESTOR:
			{

				XmlObject * ancestor = ((XmlObject*)state())->Parent;
				while (ancestor != 0)
				{
					if (state.Scope() && !((XmlObject*)state.Scope()->Object)->Contains(ancestor))
						break;

					if (Match(state, ancestor))
					{
						++position;
						if (! Predicates.ContextDimensions)
						{
							XpathContextAuto automatic(state,ancestor,position,0);
							if (Next)
							{
								if (Predicates.Match(state))
									Next->Select(sequence,state,match);
							}
							else
							if (sequence.Select(ancestor) == 0 && Predicates.Match(state))
							{
								sequence.Append(ancestor);
							}

							if(match && !sequence.IsEmpty())
								return;
						}
						else
						{
							collection->Append(ancestor);
						}

						if (position == Predicates.ContextPosition || position == Predicates.ContextPositionMax)
							break;
					}

					ancestor = ancestor->Parent;
				}
			}
			break;
		case XpathExpressionStepAxis::AXIS_DESCENDANT_OR_SELF:
			{

				if (Match(state, ((XmlObject*)state())))
				{
					++position;

					if (! Predicates.ContextDimensions)
					{
						XpathContextAuto automatic(state,state(),position,0);
						if (Predicates.Match(state))
						{
							if (Next)
							{
								Next->Select(sequence,state,match);
							}
							else

							{
								sequence.Append(state());
							}
						}

						if (match && !sequence.IsEmpty())
							return;
					}
					else
					{
						collection->Append((XmlObject*)state());
					}

					if (position == Predicates.ContextPosition || position == Predicates.ContextPositionMax)
						break;
				}

			}
		case XpathExpressionStepAxis::AXIS_DESCENDANT:
			{

				if (CanSelectIndexed(state))
				{
					SelectIndexed(sequence,state,match);
				}
				else
				{

					XmlNavigator enumerator;
					enumerator.NavigateDescendants(((XmlObject*)state()));
					for(enumerator.Forward();enumerator.Has();enumerator.Move())
					{
						if (state.Scope() && !((XmlObject*)state.Scope()->Object)->Contains(enumerator()))
							break;

						if (Match(state, enumerator()))
						{
							++position;					

							if (! Predicates.ContextDimensions)
							{
								XpathContextAuto automatic(state,enumerator(),position,0);
								if (Next )
								{
									if (Predicates.Match(state))
										Next->Select(sequence,state,match);
								}
								else
								if (sequence.Select(enumerator()) == 0 && Predicates.Match(state))
								{
									sequence.Append(enumerator());
								}

								if(match && !sequence.IsEmpty())
									return;
							}
							else
							{
								collection->Append(enumerator());
							}

							if (position == Predicates.ContextPosition || position == Predicates.ContextPositionMax)
								break;
						}
					}
				}
			}
			break;
		case XpathExpressionStepAxis::AXIS_FOLLOWING_SIBLING:
			{

				XmlObject * sibling = ((XmlObject*)state())->FollowingSibling();
				while (sibling != 0)
				{
					if (state.Scope() && !((XmlObject*)state.Scope()->Object)->Contains(sibling))
						break;

					if (Match(state, sibling))
					{

						++position;
						if (! Predicates.ContextDimensions)
						{
							XpathContextAuto automatic(state,sibling,position,0);
							if (Next)
							{
								if (Predicates.Match(state))
									Next->Select(sequence,state,match);
							}
							else
							if (sequence.Select(sibling) == 0 && Predicates.Match(state))
							{
								sequence.Append(sibling);
							}

							if(match && !sequence.IsEmpty())
								return;														
						}
						else
						{
							collection->Append(sibling);
						}

						if (position == Predicates.ContextPosition || position == Predicates.ContextPositionMax)
							break;
					}

					sibling = sibling->FollowingSibling();
				}
			}
			break;
		case XpathExpressionStepAxis::AXIS_FOLLOWING:
			{

				XmlNavigator enumerator;
				enumerator.NavigateFollowing((XmlObject*)state());
				for(enumerator.Forward();enumerator.Has();enumerator.Move())
				{
					if (state.Scope() && !((XmlObject*)state.Scope()->Object)->Contains(enumerator()))
						break;

					if (Match(state, enumerator()))
					{
						++position;
						if (! Predicates.ContextDimensions)
						{
							XpathContextAuto automatic(state,enumerator(),position,0);
							if (Next)
							{
								if (Predicates.Match(state))
									Next->Select(sequence,state,match);
							}
							else
							if (sequence.Select(enumerator()) == 0 && Predicates.Match(state))
							{
								sequence.Append(enumerator());
							}

							if(match && !sequence.IsEmpty())
								return;
						}
						else
						{
							collection->Append(enumerator());
						}

						if (position == Predicates.ContextPosition || position == Predicates.ContextPositionMax)
							break;
					}
				}
			}
		break;
		case XpathExpressionStepAxis::AXIS_PARENT:
			{

				XmlObject *parent = ((XmlObject*)state())->Parent;
				if (parent)
				{
					if (state.Scope() && !((XmlObject*)state.Scope()->Object)->Contains(parent))
						break;

					if (Match(state, parent))
					{
						++position;
						if (! Predicates.ContextDimensions)
						{
							XpathContextAuto automatic(state,parent,position,0);
							if (Next)
							{
								if (Predicates.Match(state))
									Next->Select(sequence,state,match);
							}
							else
							if (sequence.Select(parent) == 0 && Predicates.Match(state))
							{
								sequence.Append(parent);
							}

							if(match && !sequence.IsEmpty())
								return;
						}
						else
						{
							collection->Append(parent);
						}
					}
				}
			}
			break;
		case XpathExpressionStepAxis::AXIS_PRECEDING:
			{

				XmlNavigator enumerator;
				enumerator.NavigatePreceding((XmlObject*)state());
				for(enumerator.Reverse();enumerator.Has();enumerator.Move())
				{
					if (state.Scope() && !((XmlObject*)state.Scope()->Object)->Contains(enumerator()))
						break;

					if (Match(state, enumerator()))
					{
						++position;
						if (! Predicates.ContextDimensions)
						{
							XpathContextAuto automatic(state,enumerator(),position,0);
							if (Next)
							{
								if (Predicates.Match(state))
									Next->Select(sequence,state,match);
							}
							else
							if (sequence.Select(enumerator()) == 0 && Predicates.Match(state))
							{
								sequence.Append(enumerator());
							}

							if(match && !sequence.IsEmpty())
								return;
						}
						else
						{
							collection->Append(enumerator());
						}

						if (position == Predicates.ContextPosition || position == Predicates.ContextPositionMax)
							break;
					}
				}
			}
			break;
		case XpathExpressionStepAxis::AXIS_PRECEDING_SIBLING:
			{

				XmlObject *sibling = ((XmlObject*)state())->PrecedingSibling();
				while (sibling != 0)
				{
					if (state.Scope() && !((XmlObject*)state.Scope()->Object)->Contains(sibling))
						break;

					if (Match(state, sibling))
					{
						++position;
						if (! Predicates.ContextDimensions)
						{				
							XpathContextAuto automatic(state,sibling,position,0);
							if (Next)
							{
								if (Predicates.Match(state))
									Next->Select(sequence,state,match);
							}
							else
							if (sequence.Select(sibling) == 0 && Predicates.Match(state))
							{
								sequence.Append(sibling);
							}

							if(match && !sequence.IsEmpty())
								return;
						}
						else
						{
							collection->Append(sibling);
						}

						if (position == Predicates.ContextPosition || position == Predicates.ContextPositionMax)
							break;
					}

					sibling = sibling->PrecedingSibling();
				}
			}
			break;
		}

		if (collection)
		{
			position=0;
			for(collection->Forward();collection->Has();collection->Move())
			{
				++position;

				XpathContextAuto automatic(state,collection->Pointer(),collection->Index()+1,collection->Length());
				if (Next)
				{
					if (Predicates.Match(state))
						Next->Select(sequence,state,match);
				}
				else
				if (sequence.Select(collection->Pointer()) == 0 && Predicates.Match(state))
				{
					sequence.Append(collection->Pointer());
				}

				if(match && !sequence.IsEmpty())
					return;

				if (position == Predicates.ContextPosition || position == Predicates.ContextPositionMax)
					break;
			}

			delete collection;
		}
	}
	else
	if (Axis.Type.Is(AXIS_TYPE_ATTRIBUTE))
	{

		if ( ! ((XmlObject*)state())->Type.Is(XmlObject::XML_ELEMENT) )
		{

			return;
		}

		XmlElement *element = (XmlElement *)state();
		XmlAttributeCollection collection;
		XmlAttribute * attribute = element->Attributes.First;

		while (attribute != 0)
		{
			if (Match(state, attribute))
				collection.Append(attribute);

			attribute = attribute->Next;
		}

		for(collection.Forward();collection.Has();collection.Move())
		{
			XpathContextAuto automatic(state,collection.Pointer(),collection.Index()+1,collection.Length());
			if (Predicates.Match(state))
			{
				if (Next)
				{
					Next->Select(sequence,state,match);
				}
				else

				{
					sequence.Append(collection.Pointer());
				}

				if (match && !sequence.IsEmpty())
					return;
			}
		}
	}
	else
	if (Axis.Type.Is(AXIS_TYPE_NAMESPACE))
	{

		OutputError("XpathExpressionLocation::Select - Namespace axis is not fully implemented.\n");
		return;
	}
	#ifdef REASON_USING_XML_STYLE
	else
	if (Axis.Type.Is(AXIS_TYPE_STYLE))
	{
		XmlObject * object = ((XmlObject*)state());
		XmlObject * root = object->Root();

		if (root && root->InstanceOf(XmlDocument::Instance))
		{

			bool insert=false;
			XmlStyle * style=0;

			if (((XmlDocument*)root)->Stylesheet)
				style = ((XmlDocument*)root)->Stylesheet->Map[object];

			if (!style) 
			{
				insert = true;
				style = new XmlStyle(object);
			}

			if (Match(state, style))
			{
				XpathContextAuto automatic(state,style,1,1);
				if (Next)
				{
					if (Predicates.Match(state))
						Next->Select(sequence,state,match);

					if (insert)
						delete style;
				}	
				else
				if ( Predicates.Match(state))
				{
					sequence.Append(style);

					if (insert)
						((XmlDocument*)root)->Stylesheet->Map.Insert(object,style);
				}
			}
			else
			{
				if (insert)
					delete style;
			}
		}
	}
	#endif

}

void XpathExpressionStepAxis::SelectAttribute(XpathSequence & sequence, XpathState & state, bool match)
{

	int position=0;

	if(Axis.Type.Is(AXIS_TYPE_ELEMENT))
	{
		XmlCollection collection;

		switch (Axis.Type)
		{
		case XpathExpressionStepAxis::AXIS_ROOT:
			{

				XmlObject *root = ((XmlObject*)state())->Root();

				XpathContextAuto automatic(state,root,1,1);
				Next->Select(sequence,state,match);
			}
			break;
		case XpathExpressionStepAxis::AXIS_SELF:
			{

				XpathContextAuto automatic(state,state(),1,1);
				if (Match(state, (XmlAttribute*)state()) && Predicates.Match(state))
				{
					if (Next)
					{
						Next->Select(sequence,state,match);
					}
					else

					{
						sequence.Append(state());
					}
				}
			}
			break;
		case XpathExpressionStepAxis::AXIS_ANCESTOR_OR_SELF:
			{

				if (Match(state, (XmlAttribute*)state()))
				{
					if (! Predicates.ContextDimensions)
					{
						XpathContextAuto automatic(state,state(),1,1);
						if (Predicates.Match(state))
						{
							if (Next)
							{
								Next->Select(sequence,state,match);
							}
							else

							{
								sequence.Append(state());
								if (match) return;
							}
						}
					}
					else
					{
						collection.Append(state());
					}
				}

			}
		case XpathExpressionStepAxis::AXIS_ANCESTOR:
			{

				XmlObject *ancestor = ((XmlObject*)state())->Parent;
				while (ancestor != 0)
				{
					if (state.Scope() && !((XmlObject*)state.Scope()->Object)->Contains(ancestor))
						break;

					if (Match(state, ancestor))
					{
						++position;
						if (! Predicates.ContextDimensions)
						{
							XpathContextAuto automatic(state,ancestor,position,0);
							if (Predicates.Match(state))
							{
								if (Next)
								{
									Next->Select(sequence,state,match);
								}
								else

								{
									sequence.Append(ancestor);
									if (match) return;
								}
							}
						}
						else
						{
							collection.Append(ancestor);
						}
					}

					ancestor = ancestor->Parent;
				}
			}
			break;
		case XpathExpressionStepAxis::AXIS_PARENT:
			{
				if (((XmlObject*)state())->Parent)
				{

					XpathContextAuto automatic(state,((XmlObject*)state())->Parent,1,1);
					if (Match(state, ((XmlObject*)state())->Parent) && Predicates.Match(state))
					{
						if (Next)
						{
							Next->Select(sequence,state,match);
						}
						else

						{
							sequence.Append(state());
							if (match) return;
						}
					}
				}
			}
			break;
		}

		for(collection.Forward();collection.Has();collection.Move())
		{
			XpathContextAuto automatic(state,collection.Pointer(),collection.Index()+1,collection.Length());
			if (Predicates.Match(state))
			{
				if (Next)
				{
					Next->Select(sequence,state,match);
				}
				else

				{
					sequence.Append(collection.Pointer());
					if (match) return;
				}
			}
		}

	}

}

void XpathExpressionStepAxis::SelectNamespace(XpathSequence &sequence, XpathState & state, bool match)
{

	OutputError("XpathExpressionStepAxis::Select - Namepsace axis not implemented.\n");
}

void XpathExpressionStepAxis::SelectStyle(XpathSequence &sequence, XpathState & state, bool match)
{
	OutputError("XpathExpressionStepAxis::Select - Style axis not implemented.\n");
}

bool XpathExpressionStepAxis::Match(XpathState & state, const XpathContext & context)
{
	if (state.Path->Extension)
	{

		XpathExtensionHandlerHandler * handler = state.Path->Extension->Lookup(XpathExtension::EXTENSION_STEP_TEST,"");
		if (handler)
		{
			if (!handler->ProcessTest(state,context))
				return false;
		}

		if (!Test.IsEmpty())
		{
			handler = state.Path->Extension->Lookup(XpathExtension::EXTENSION_STEP_TEST,Test);
			if (handler)
			{
				return handler->ProcessTest(state,context);
			}
		}
	}

	switch (context.Type.Bits)
	{
	case XpathContext::TYPE_OBJECT:
	case XpathContext::TYPE_MARKUP:
		return MatchObject((XmlObject*)context.Object);
		break;
	case XpathContext::TYPE_ATTRIBUTE:
		return MatchAttribute((XmlAttribute*)context.Object);
		break;
	#ifdef REASON_USING_XML_STYLE	
	case XpathContext::TYPE_STYLE:
		return MatchStyle((XmlStyle*)context.Object);
		break;
	#endif
	case XpathContext::TYPE_NAMESPACE:
		return MatchNamespace((XmlNamespace*)context.Object);
		break;
	}

	return false;
}

bool XpathExpressionStepAxis::MatchObject(XmlObject *object)
{

	if (object->Type.Is(XmlObject::XML_END) )
	{

		return false;
	}

	if (Test.Type.Is(XpathExpressionStepAxis::TEST_NAME_QUALIFIED))
	{
		if (object->Type.Is(XmlObject::XML_ELEMENT))
		{
			XmlElement * element = (XmlElement *)object;

			int i = Test.IndexOf(":");

			if (i != -1)
			{

				Substring name;
				name.Assign(Test.Data,Test.Data+i-1);

				if (element->Namespace && element->Namespace->Prefix.Is(name,true))
				{

					name.Assign(Test.Data+i+1,Test.Data+Test.Size-1);
					return element->Token.Is(name,true);
				}
			}
			else
			{

				return element->Token.Is(Test,true);
			}
		}
	}
	else
	if(Test.Type.Is(XpathExpressionStepAxis::TEST_NODE_ALL))
	{

		if ( ! object->Type.Is(XmlObject::XML_DECLARATION))
			return true;
	}
	else
	if (Test.Type.Is(XpathExpressionStepAxis::TEST_NAME_ALL))
	{

		if (object->Type.Is(XmlObject::XML_ELEMENT))
			return true;
	}
	else
	if (Test.Type.Is(XpathExpressionStepAxis::TEST_NODE_COMMENT))
	{
		if (object->Type.Is(XmlObject::XML_COMMENT))
			return true;
	}
	else
	if (Test.Type.Is(XpathExpressionStepAxis::TEST_NODE_TEXT))
	{

		if (object->Type.Is(XmlObject::XML_TEXT) || object->Type.Is(XmlObject::XML_CDATA))
			return true;
	}
	else
	if (Test.Type.Is(XpathExpressionStepAxis::TEST_NODE_PROCESSING))
	{

		if (object->Type.Is(XmlObject::XML_PROCESSING_INSTRUCTION))
			return true;
	}
	else
	if (Test.Type.Is(XpathExpressionStepAxis::TEST_NAME_NAMESPACE))
	{
		if (object->Type.Is(XmlObject::XML_ELEMENT))
		{
			XmlElement * element = (XmlElement *)object;
			return element->Namespace->Prefix.Is(Test,true);
		}
	}

	return false;
}

bool XpathExpressionStepAxis::MatchAttribute(XmlAttribute *attribute)
{

	if(Test.Type.Is(XpathExpressionStepAxis::TEST_NODE_ALL))
	{
		return true;
	}
	else
	if (Test.Type.Is(XpathExpressionStepAxis::TEST_NAME_ALL) )
	{
		return true;
	}
	else
	if (Test.Type.Is(XpathExpressionStepAxis::TEST_NAME_NAMESPACE))
	{

		OutputError("XpathExpressionStep::Match - XML Namespaces are not yet supported. See XpathExpressionStep::Match(XmlAttribute *context)\n");

		return (attribute->Namespace->Prefix.Is(Test));

	}
	else
	if (Test.Type.Is(XpathExpressionStepAxis::TEST_NAME_QUALIFIED))
	{
		int i = Test.IndexOf(":");

		if (i != -1)
		{

			Substring string;
			string.Assign(Test.Data,Test.Data+i-1);

			if (attribute->Namespace->Prefix.Is(string))
			{

				string.Assign(Test.Data+i+1,Test.Data+Test.Size-1);

				return (attribute->Name.Is(string,true));
			}
		}
		else
		{

			return (attribute->Name.Is(Test,true));
		}
	}

	return false;
}

bool XpathExpressionStepAxis::MatchNamespace(XmlNamespace *name)
{

	return false;
}

#ifdef REASON_USING_XML_STYLE
bool XpathExpressionStepAxis::MatchStyle(XmlStyle *style)
{
	return style->Select(Test) != 0;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathParser::XpathParser():
	Warnings(0),Errors(0),Failures(0),Extension(0)
{
	Handled=this;
}

XpathParser::~XpathParser()
{

}

void XpathParser::Catch(const unsigned int type,const char *label,const char * message,va_list args)
{

	{
		switch (type & Handler::TypeMask)
		{
			case Handler::HANDLER_FAILURE:
			{
				++Failures;
				OutputFailure("XpathParser::%s - ", label);
			}
			break;
			case Handler::HANDLER_WARNING:
			{
				OutputWarning("XpathParser::%s - ", label);
				++Warnings;
			}
			break;

			default:
			{
				OutputError("XpathParser::%s - ", label);
				++Errors;
			};
		}

		#ifdef REASON_PRINT_MESSAGE
		vfprintf(REASON_HANDLE_OUTPUT,message,args);
		#endif

		OutputMessage("\n");
	}
}

void XpathParser::Report()
{
	OutputMessage("XpathParser:Report - %d Failure(s), %d Error(s), %d Warning(s)\n",Failures,Errors,Warnings);
}

XpathExpression * XpathParser::Parse(const Sequence &sequence)
{
	Assign(sequence);

	XpathExpression *expr = ParseExpression();

	if (!Eof())
	{
		Error("Parse","Invalid token in expresssion \"%s\", column %d.",(char*)Scanner->Token.Data+Scanner->Token.Offset,(int)Column());
		return 0;
	}

	return expr;
}

bool XpathParser::ParsePredicateList(XpathExpressionPredicateList &predicates)
{

	XpathExpressionPredicate * predicate;
	while((predicate=(XpathExpressionPredicate *)ParseExpressionPredicate()) != 0)
	{
		predicates.Append(predicate);
	}

	if (predicates.Count > 0)
		predicates.Position();

	return true;
}

XpathExpression * XpathParser::ParseExpressionPredicate()
{
	if (Is('['))
	{
		XpathExpressionPredicate * predicate = new XpathExpressionPredicate();

		Mark(predicate->Token);
		Next();

		SkipWhitespace();

		if ((predicate->Expression=ParseExpression()) != 0)
		{
			if (Is(']'))
			{
				Next();
				Trap(predicate->Token);
				return predicate;
			}
			else
			{
				Error("ParseExpressionPredicate","Missing closing \"]\" in predicate, column %d.",Column());
				delete predicate;
			}
		}
		else
		{
			Error("ParseExpressionPredicate","Problem while parsing predicate, column %d.",Column());
			delete predicate;
		}
	}

	return 0;
}

XpathExpression * XpathParser::ParseExpressionOr()
{

	Substring token;
	Mark(token);

	XpathExpression *e = ParseExpressionAnd();

	if (e)
	{
		if (Is("or"))
		{
			Skip(2);
			SkipWhitespace();

			XpathExpressionBinary *binaryExpression = new XpathExpressionBinary();
			binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_OR;
			binaryExpression->OperandOne = e;

			e = ParseExpressionOr();

			if (e)
			{
				Trap(token);
				binaryExpression->Token = token;
				binaryExpression->OperandTwo = e;
				return binaryExpression;
			}
			else
			{
				delete binaryExpression;
				return e;	
			}
		}
	}

	return e;

}

XpathExpression * XpathParser::ParseExpressionAnd()
{
	Substring token;
	Mark(token);

	XpathExpression *e = ParseExpressionEquality();

	if (e)
	{
		if (Is("and"))
		{
			Skip(3);
			SkipWhitespace();

			XpathExpressionBinary * binaryExpression = new XpathExpressionBinary();
			binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_AND;
			binaryExpression->OperandOne = e;

			e = ParseExpressionAnd();

			if (e)
			{
				Trap(token);
				binaryExpression->Token = token;
				binaryExpression->OperandTwo = e;
				return binaryExpression;
			}
			else
			{
				delete binaryExpression;
				return e;	
			}
		}
	}

	return e;
}

XpathExpression * XpathParser::ParseExpressionEquality()
{
	Substring token;
	Mark(token);

	XpathExpression *e = ParseExpressionRelational();

	if (e)
	{

		if (Is('!'))
		{
			Next();
			if (Is(Character::Equal))
			{
				Next();

				XpathExpressionBinary * binaryExpression = new XpathExpressionBinary();
				binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_NOT_EQUAL;
				binaryExpression->OperandOne = e;

				SkipWhitespace();
				e = ParseExpressionEquality();

				if (e)
				{
					Trap(token);
					binaryExpression->Token = token;
					binaryExpression->OperandTwo = e;
					return binaryExpression;
				}
				else
				{
					delete binaryExpression;
					return e;	
				}

			}
			else
			{
				delete e;
				return 0;
			}
		}
		else
		if (Is(Character::Equal))
		{
			Next();

			XpathExpressionBinary * binaryExpression = new XpathExpressionBinary();
			binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_EQUAL;
			binaryExpression->OperandOne = e;

			SkipWhitespace();
			e = ParseExpressionEquality();

			if (e)
			{
				Trap(token);
				binaryExpression->Token = token;
				binaryExpression->OperandTwo = e;
				return binaryExpression;
			}
			else
			{
				Error("ParseExpressionEquality","Equality expression is missing second operand, column %d.",Column());
				delete binaryExpression;
				return e;	
			}
		}
	}

	return e;
}

XpathExpression * XpathParser::ParseExpressionRelational()
{
	Substring token;
	Mark(token);

	XpathExpression *e = ParseExpressionAdditive();

	if (e)
	{

		if (Is('>'))
		{
			Next();

			XpathExpressionBinary * binaryExpression = new XpathExpressionBinary();
			if (Is('='))
			{
				Next();
				binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_GREATER_THAN_OR_EQUAL;
			}
			else
			{
				binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_GREATER_THAN;
			}

			binaryExpression->OperandOne = e;

			SkipWhitespace();
			e = ParseExpressionRelational();

			if (e)
			{
				Trap(token);
				binaryExpression->Token = token;
				binaryExpression->OperandTwo  = e;
				return binaryExpression;
			}
			else
			{
				delete binaryExpression;
			}
		}
		else
		if (Is('<'))
		{
			Next();

			XpathExpressionBinary * binaryExpression = new XpathExpressionBinary();

			if (Is('='))
			{
				Next();
				binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_LESS_THAN_OR_EQUAL;
			}
			else
			{
				binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_LESS_THAN;
			}

			binaryExpression->OperandOne = e;

			SkipWhitespace();
			e = ParseExpressionRelational();

			if (e)
			{
				Trap(token);
				binaryExpression->Token = token;
				binaryExpression->OperandTwo = e;
				return binaryExpression;
			}
			else
			{
				delete binaryExpression;
			}
		}

	}

	return e;
}

XpathExpression * XpathParser::ParseExpressionAdditive()
{
	Substring token;
	Mark(token);

	XpathExpression *e = ParseExpressionMultiplicative();

	if (e)
	{
		if (Is('+'))
		{
			Next();

			XpathExpressionBinary * binaryExpression = new XpathExpressionBinary();
			binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_ADD;
			binaryExpression->OperandOne = e;

			SkipWhitespace();
			e = ParseExpressionAdditive();

			if (e)
			{
				Trap(token);
				binaryExpression->Token = token;
				binaryExpression->OperandTwo = e;
				return binaryExpression;
			}
			else
			{
				delete binaryExpression;
			}

		}
		else
		if (Is('-'))
		{
			Next();
			XpathExpressionBinary * binaryExpression = new XpathExpressionBinary();
			binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_SUBTRACT;
			binaryExpression->OperandOne = e;

			SkipWhitespace();
			e = ParseExpressionAdditive();

			if (e)
			{
				Trap(token);
				binaryExpression->Token = token;
				binaryExpression->OperandTwo = e;
				return binaryExpression;
			}
			else
			{
				delete binaryExpression;
			}
		}
	}

	return e;
}

XpathExpression * XpathParser::ParseExpressionMultiplicative()
{

	Substring token;
	Mark(token);

	XpathExpression *e = ParseExpressionOrdinal();

	if (e)
	{

		XpathExpressionBinary * binaryExpression=0;

		if(Is('*'))
		{
			Next();
			binaryExpression = new XpathExpressionBinary();
			binaryExpression->OperandOne = e;
			binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_MULTIPLY;
		}
		else
		if (Is("div"))
		{
			Skip(3);
			binaryExpression = new XpathExpressionBinary();
			binaryExpression->OperandOne = e;
			binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_DIV;
		}
		else
		if (Is("mod"))
		{
			Skip(3);
			binaryExpression = new XpathExpressionBinary();
			binaryExpression->OperandOne = e;
			binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_MOD;
		}

		if (binaryExpression)
		{
			SkipWhitespace();

			e = ParseExpressionMultiplicative();

			if (e)
			{
				Trap(token);
				binaryExpression->Token = token;
				binaryExpression->OperandTwo = e;
				return binaryExpression;
			}
			else
			{
				delete binaryExpression;
			}
		}
	}

	return e;
}

XpathExpression * XpathParser::ParseExpressionOrdinal()
{
	Substring token;
	Mark(token);

	XpathExpression *e = ParseExpressionUnary();

	if (e)
	{
		if (Is("<<"))
		{
			Skip(2);

			XpathExpressionBinary * binaryExpression = new XpathExpressionBinary();
			binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_PRECEDING;
			binaryExpression->OperandOne = e;

			SkipWhitespace();
			e = ParseExpressionOrdinal();

			if (e)
			{
				Trap(token);
				binaryExpression->Token = token;
				binaryExpression->OperandTwo = e;
				return binaryExpression;
			}
			else
			{
				delete binaryExpression;
			}

		}
		else
		if (Is(">>"))
		{
			Skip(2);

			XpathExpressionBinary * binaryExpression = new XpathExpressionBinary();
			binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_FOLLOWING;
			binaryExpression->OperandOne = e;

			SkipWhitespace();
			e = ParseExpressionOrdinal();

			if (e)
			{
				Trap(token);
				binaryExpression->Token = token;
				binaryExpression->OperandTwo = e;
				return binaryExpression;
			}
			else
			{
				delete binaryExpression;
			}
		}
	}

	return e;
}

XpathExpression * XpathParser::ParseExpressionUnary()
{

	XpathExpression *e;

	if (Is('-') || Is('+'))
	{
		Next();

		Substring token;
		Mark(token);

		e = ParseExpressionUnary();
		if (e)
		{
			Trap(token);

			XpathExpressionSigned* sign = new XpathExpressionSigned();		

			sign->Type.Assign(XpathExpression::EXPRESSION_SIGNED);
			sign->Expression = e;
			sign->Token = token;

			return sign;
		}
	}
	else
	{
		e = ParseExpressionUnion();
	}

	return e;
}

XpathExpression * XpathParser::ParseExpressionUnion()
{

	Substring token;
	Mark(token);

	XpathExpression *e = ParseExpressionIntersection();

	if(e)
	{

		SkipWhitespace();

		if (Is('|'))
		{
			Next();
			SkipWhitespace();

			XpathExpressionBinary * binaryExpression = new XpathExpressionBinary();
			binaryExpression->OperandOne = e;
			binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_UNION;

			e = ParseExpressionUnion();

			if (e)
			{
				Trap(token);
				binaryExpression->Token = token;
				binaryExpression->OperandTwo = e;
				return binaryExpression;
			}
			else
			{
				delete binaryExpression;
			}
		}
	}

	return e;
}

XpathExpression * XpathParser::ParseExpressionIntersection()
{

	Substring token;
	Mark(token);

	XpathExpression *e = ParseExpressionDifference();

	if(e)
	{
		SkipWhitespace();

		if (Is('&'))
		{
			Next();
			SkipWhitespace();

			XpathExpressionBinary * binaryExpression = new XpathExpressionBinary();
			binaryExpression->OperandOne = e;
			binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_INTERSECTION;

			e = ParseExpressionIntersection();

			if (e)
			{
				Trap(token);
				binaryExpression->Token = token;
				binaryExpression->OperandTwo = e;
				return binaryExpression;
			}
			else
			{
				delete binaryExpression;
			}
		}
	}

	return e;
}

XpathExpression * XpathParser::ParseExpressionDifference()
{

	Substring token;
	Mark(token);

	XpathExpression *e = ParseExpressionPath();

	if(e)
	{
		SkipWhitespace();

		if (Is('^'))
		{
			Next();
			SkipWhitespace();

			XpathExpressionBinary * binaryExpression = new XpathExpressionBinary();
			binaryExpression->OperandOne = e;
			binaryExpression->Type = XpathExpression::EXPRESSION_BINARY_DIFFERENCE;

			e = ParseExpressionDifference();

			if (e)
			{
				Trap(token);
				binaryExpression->Token = token;
				binaryExpression->OperandTwo = e;
				return binaryExpression;
			}
			else
			{
				delete binaryExpression;
			}
		}
	}

	return e;
}

XpathExpression * XpathParser::ParseExpressionPath()
{

	if (Is(Character::ForwardSlash))
	{
		return ParseExpressionPathAbsolute();
	}
	else
	{
		return ParseExpressionPathRelative();
	}

}

XpathExpression * XpathParser::ParseExpressionStepFilter()
{

	Substring token;
	Mark(token);

	XpathExpression *e = ParseExpressionPrimary();

	if (e)
	{
		XpathExpressionPredicate * predicate = (XpathExpressionPredicate *) ParseExpressionPredicate();
		if (predicate)
		{

			XpathExpressionStepFilter *filter = new XpathExpressionStepFilter(e);
			filter->Predicates.Append(predicate);

			ParsePredicateList(filter->Predicates);

			Trap(token);
			filter->Token = token;

			return filter;
		}
	}

	return e;

}

XpathExpression * XpathParser::ParseExpressionPrimary()
{

	if (IsNumeric())
	{

		Substring string;
		Mark(string);

		ParseNumber();
		if (Is('.'))
		{
			Next();
			if (!ParseNumber())
			{
				return 0;
			}
		}
		if (IsAny("eE"))
		{

			Next();
			if (IsAny("-+"))
			{
				Next();
			}

			if (!ParseNumber())
			{
				return 0;
			}
		}

		Trap(string);

		XpathString objectString;
		objectString.Value.Construct(string);

		XpathExpressionLiteral *primary = new XpathExpressionLiteral();
		primary->Literal = new XpathNumber(&objectString);
		return primary;
	}
	else
	if (Is('.') && IsNumeric(1))
	{

		Substring string;
		Mark(string);

		Next();
		if (ParseNumber())
		{
			if (IsAny("eE"))
			{

				Next();
				if (IsAny("-+"))
				{
					Next();
				}

				if (!ParseNumber())
				{
					return 0;
				}
			};
		}

		Trap(string);

		XpathString objectString;
		objectString.Value.Assign(string);

		XpathExpressionLiteral * primary = new XpathExpressionLiteral();
		primary->Literal = new XpathNumber(&objectString);
		return primary;

	}
	else
	if (Is(Character::SingleQuote) || Is(Character::DoubleQuote))
	{
		ParseString();

		XpathString * objectString = new XpathString();
		objectString->Value.Assign(Token);

		XpathExpressionLiteral * primary = new XpathExpressionLiteral();
		primary->Literal = objectString;
		return primary;

	}
	else
	if (Is('$'))
	{
		Next();

		if (!ParseQualifiedName())
			return 0;

		XpathExpressionVariable *reference = new XpathExpressionVariable();
		reference->Token.Assign(Token);
		return reference;
	}
	else
	if (Is('('))
	{

		Next();

		SkipWhitespace();

		XpathExpression * e = ParseExpression();

		if (e)
		{
			if (Is(')'))
			{
				Next();

				XpathExpressionParenthesised *parenthesised = new XpathExpressionParenthesised();
				parenthesised->Expression = e;
				return parenthesised;
			}
			else
			{
				Error("ParseExpressionPrimary","Expected closing brace ')' after filter expression, column %d.",Column());
				delete e;
			}
		}
	}
	else
	{
		class Token state(Scanner->Token);

		ParseQualifiedName();

		if (Is('('))
		{
			XpathExpressionFunction *function = new XpathExpressionFunction();
			function->Token.Assign(Token);

			Next();

			SkipWhitespace();

			if (!Is(')'))
			{
				XpathExpression * e = ParseExpression();
				if (e)
				{
					function->Arguments.Append(e);

					while (Is(','))
					{
						Next();
						e = ParseExpression();

						if (e)
						{
							function->Arguments.Append(e);
						}
						else
						{
							Error("ParseExpressionPrimary","Expected function argument after separator \",\", column %d.",Column());
							delete function;
							return 0;
						}
					}
				}
			}

			if (Is(')'))
			{
				Next();
			}
			else
			{
				Error("ParseExpressionPrimary","Expected closing brace \")\" after function arguments, column %d.",Column());
				delete function;
				return 0;
			}

			return function;
		}
		else
		{

			Load(state);
		}
	}

	return 0;
}

XpathExpression * XpathParser::ParseExpression()
{

	XpathExpression *e = ParseExpressionOr();

	return e;
}

XpathExpression * XpathParser::ParseExpressionPathAbsolute()
{

	if (Is(Character::ForwardSlash))
	{
		Mark();
		Next();

		XpathExpressionStep * path = new XpathExpressionStepAxis(XpathExpressionStepAxis::AXIS_ROOT,XpathExpressionStepAxis::TEST_NONE);
		XpathExpressionStep * step = path;

		if (Is(Character::ForwardSlash))
		{
			Next();
			Trap();

			step = step->Append(new XpathExpressionStepAxis(XpathExpressionStepAxis::AXIS_DESCENDANT_OR_SELF,XpathExpressionStepAxis::TEST_NODE_ALL));
			step->Token.Assign(Token);

			XpathExpression * e = ParseExpressionPathRelative();

			if (e)
			{
				if (e->Type.Is(XpathExpression::EXPRESSION_PRIMARY))
				{
					step = step->Append(new XpathExpressionStepFilter(e));
				}
				else
				{
					step = step->Append((XpathExpressionStep*)e);
				}
			}
		}
		else
		{
			XpathExpression * e = ParseExpressionPathRelative();

			if (e && e->Type.Is(XpathExpression::EXPRESSION_PRIMARY))
			{
				step = step->Append(new XpathExpressionStepFilter(e));
			}
			else
			{
				step = step->Append((XpathExpressionStep*)e);
			}
		}

		return path;
	}

	return 0;
}

XpathExpression * XpathParser::ParseExpressionPathRelative()
{

	XpathExpression * e = ParseExpressionStep();

	if (Is(Character::ForwardSlash))
	{

		XpathExpressionStep * path;

		if (e->Type.Is(XpathExpression::EXPRESSION_PRIMARY))
		{
			path = new XpathExpressionStepFilter(e);
		}
		else
		{
			path = (XpathExpressionStep *) e;
		}

		XpathExpressionStep * step = path;

		while (Is(Character::ForwardSlash))
		{
			Mark();
			Next();

			if (Is(Character::ForwardSlash))
			{
				Next();
				Trap();

				step = step->Append(new XpathExpressionStepAxis(XpathExpressionStepAxis::AXIS_DESCENDANT_OR_SELF,XpathExpressionStepAxis::TEST_NODE_ALL));
				step->Token.Assign(Token);
			}

			e = ParseExpressionStep();

			if (e)
			{
				if (e->Type.Is(XpathExpression::EXPRESSION_PRIMARY))
				{
					step = step->Append(new XpathExpressionStepFilter(e));
				}
				else
				{
					step = step->Append((XpathExpressionStep*)e);
				}
			}
			else
			{

				if (! Token.IsEmpty())
				{
					Error("ParseExpressionPathRelative","Invalid location step \"%s\", column %d.",Token.Print(),Column()-Token.Size);
					return false;
				}
			}
		}

		return path;
	}

	return e;
}

bool XpathParser::ParseNodeTest(XpathExpressionStepAxis &axis)
{

	Mark(axis.Test);

	if (Is('*'))
	{
		Next();

		Trap(axis.Test);
		axis.Test.Type.Assign(XpathExpressionStepAxis::TEST_NAME_ALL);

		return true;
	}
	else
	if (ParseNcname())
	{
		if (Is(':'))
		{
			if (Is(1,'*'))
			{
				Next();
				Trap(axis.Test);
				axis.Test.Type.Assign(XpathExpressionStepAxis::TEST_NAME_NAMESPACE);
				return true;
			}
			else
			if (At(1,1) && IsNcnameChar(*At(1,1)))
			{
				Next();
				ParseNcname();
			}
		}

		Trap(axis.Test);

		if (axis.Test.Is("comment"))
		{
			axis.Test.Type.Assign(XpathExpressionStepAxis::TEST_NODE_COMMENT);
		}
		else
		if (axis.Test.Is("text"))
		{
			axis.Test.Type.Assign(XpathExpressionStepAxis::TEST_NODE_TEXT);
		}
		else
		if (axis.Test.Is("node"))
		{
			axis.Test.Type.Assign(XpathExpressionStepAxis::TEST_NODE_ALL);
		}
		else
		if (axis.Test.Is("processing-instruction"))
		{
			axis.Test.Type.Assign(XpathExpressionStepAxis::TEST_NODE_PROCESSING);
		}
		else
		if (Extension)
		{
			XpathExtensionHandlerHandler * handler = Extension->Lookup(XpathExtension::EXTENSION_STEP_TEST,axis.Test);
			if (handler)
			{
				axis.Test.Type.Assign(XpathExpressionStepAxis::TEST_EXTENSION);
			}
		}

		if (Is('('))
		{
			if (axis.Test.Type.Is(0))
			{

				return false;
			}

			Next();
			if( Is(')') )
			{
				Next();
				return true;
			}

			if  (axis.Test.Type.Is(XpathExpressionStepAxis::TEST_NODE_PROCESSING))
			{

				if (Is(Character::SingleQuote) || Is(Character::DoubleQuote))
				{

					if (!ParseString())
					{
						Error("ParseNodeTest","String literal for processing-instruction expected, column %d.",Column());
						return false;
					}

					if (Is(')'))
					{
						Next();
						XpathExpressionLiteral * primary = new XpathExpressionLiteral();
						primary->Literal = new XpathString(Token);

												axis.Arguments.Append(primary);
						axis.Test.Type.Assign(XpathExpressionStepAxis::TEST_NODE_PROCESSING);
						return true;
					}
					else
					{
						Error("ParseNodeTest","Required \")\" is missing in processing-instruction, column %d.",Column());
						return false;
					}
				}
				else
				{
					Error("ParseNodeTest","Required \' or \" missing from processing-instruction, column %d",Column());
					return false;
				}
			}
			else
			if  (axis.Test.Type.Is(XpathExpressionStepAxis::TEST_EXTENSION))
			{
				SkipWhitespace();

				XpathExpression * e = ParseExpression();
				if (e)
				{
					axis.Arguments.Append(e);

					while (Is(','))
					{
						Next();
						e = ParseExpression();

						if (e)
						{
							axis.Arguments.Append(e);
						}
						else
						{
							Error("ParseNodeTest","Expected function argument after separator \",\", column %d.",Column());
							return false;
						}
					}
				}

				if (Is(')'))
				{
					Next();
				}
				else
				{
					Error("ParseNodeTest","Expected closing brace \")\" after function arguments, column %d.",Column());
					return false;
				}

				return true;
			}
			else
			{
				Error("ParseNodeTest","Node test function missing \")\", column %d",Column());
				return false;
			}
		}

		axis.Test.Type.Assign(XpathExpressionStepAxis::TEST_NAME_QUALIFIED);
		return true;
	}

	return false;
}

XpathExpression * XpathParser::ParseExpressionStep()
{

	class Token state(Scanner->Token);

	XpathExpression * e = ParseExpressionStepAxis();

	if ( !e )
	{
		Load(state);
		e = ParseExpressionStepScope();
	}

	return e;
}

XpathExpression * XpathParser::ParseExpressionStepScope()
{
	XpathExpression *e = ParseExpressionStepFilter();

	if (!e)
	{
		if (Is("::"))
		{

			XpathExpressionStepScope *scope = new XpathExpressionStepScope();

			Mark(scope->Token);
			Skip(2);
			Trap(scope->Token);

			return scope;
		}

	}

	return e;
}

XpathExpression * XpathParser::ParseExpressionStepAxis()
{

	if (Is('.'))
	{

		XpathExpressionStepAxis *axis = new XpathExpressionStepAxis();
		Mark(axis->Token);
		Next();

		if (Is('.'))
		{

			Next();

			if (Is(Character::ForwardSlash) || Eof())
			{

				axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_PARENT);
				axis->Test.Type.Assign(XpathExpressionStepAxis::TEST_NODE_ALL);

				ParsePredicateList(axis->Predicates);
				Trap(axis->Token);

				return axis;
			}
			else
			{
				Error("ParseExpressionStep","Invalid character following abbreviated step '..', column %d",Column());
				delete axis;
				return 0;
			}
		}
		else
		if (! IsNumeric() ) 
		{

			axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_SELF);
			axis->Test.Type.Assign(XpathExpressionStepAxis::TEST_NODE_ALL);

			ParsePredicateList(axis->Predicates);
			Trap(axis->Token);

			return axis;
		}

		delete axis;

	}
	else if (Is('@'))
	{

		XpathExpressionStepAxis *axis = new XpathExpressionStepAxis();
		axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_ATTRIBUTE);

		Mark(axis->Token);
		Next();

		if (!ParseNodeTest(*axis))
		{
			Error("ParseExpressionStep","Expected attribute name, column %d",Column());
			delete axis;
			return 0;
		}

		ParsePredicateList(axis->Predicates);

		Trap(axis->Token);

		return axis;
	}
	else if (Is('#'))
	{

		XpathExpressionStepAxis *axis = new XpathExpressionStepAxis();
		axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_STYLE);

		Mark(axis->Token);
		Next();

		if (!ParseNodeTest(*axis))
		{
			Error("ParseExpressionStep","Expected style name, column %d",Column());
			delete axis;
			return 0;
		}

		ParsePredicateList(axis->Predicates);

		Trap(axis->Token);

		return axis;
	}
	else
	{

		XpathExpressionStepAxis *axis = new XpathExpressionStepAxis();
		Mark(axis->Token);

		if (ParseNodeTest(*axis))
		{
			if (axis->Test.Type.Is(XpathExpressionStepAxis::TEST_NAME_QUALIFIED))
			{

				axis->Axis.Assign(axis->Test);

				if (Is("::"))
				{

					if (axis->Test.Is("ancestor"))
					{
						axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_ANCESTOR);
					}
					else
					if (axis->Test.Is("ancestor-or-self"))
					{
						axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_ANCESTOR_OR_SELF);
					}
					else
					if (axis->Test.Is("attribute"))
					{
						axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_ATTRIBUTE);
					}
					else
					if (axis->Test.Is("child"))
					{
						axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_CHILD);
					}
					else
					if (axis->Test.Is("descendant"))
					{
						axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_DESCENDANT);
					}
					else
					if (axis->Test.Is("descendant-or-self"))
					{
						axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_DESCENDANT_OR_SELF);
					}
					else
					if (axis->Test.Is("following"))
					{
						axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_FOLLOWING);
					}
					else
					if (axis->Test.Is("following-sibling"))
					{
						axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_FOLLOWING_SIBLING);
					}
					else
					if (axis->Test.Is("namespace"))
					{
						axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_NAMESPACE);
					}
					else
					if (axis->Test.Is("parent"))
					{
						axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_PARENT);
					}
					else
					if (axis->Test.Is("preceding"))
					{
						axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_PRECEDING);
					}
					else
					if (axis->Test.Is("preceding-sibling"))
					{
						axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_PRECEDING_SIBLING);
					}
					else
					if (axis->Test.Is("self"))
					{
						axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_SELF);
					}

					if (axis->Axis.Type == 0)
					{
						Error("ParseExpressionStep","Unrecognised axis name \"%s\", column %d.",Token.Print(),Column());
						delete axis;
					}
					else
					{
						Skip(2);

						axis->Test.Type.Assign(0);

						if(! ParseNodeTest(*axis))
						{
							Error("ParseExpressionStep","Axis name must be followed by a valid node test, column %d.",Column());
							delete axis;
						}
						else
						{
							ParsePredicateList(axis->Predicates);

							Trap(axis->Token);
							return axis;
						}
					}
				}
				else
				{

					axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_CHILD);

					ParsePredicateList(axis->Predicates);

					Trap(axis->Token);
					return axis;
				}
			}
			else
			{

				axis->Axis.Type.Assign(XpathExpressionStepAxis::AXIS_CHILD);

				ParsePredicateList(axis->Predicates);

				Trap(axis->Token);
				return axis;
			}
		}
		else
		{

			delete axis;
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Xpath::Xpath(const char * expression):
	String(expression),Expression(0),Extension(0)
{
}

Xpath::Xpath(const Sequence & expression):
	String(expression),Expression(0),Extension(0)
{
}

Xpath::Xpath():
	Expression(0),Extension(0)
{
}

Xpath::~Xpath()
{
	if (Expression)
		delete Expression;
}

void Xpath::Construct(char * data, int size)
{
	Release();
	String::Construct(data,size);
}

void Xpath::Release()
{
	if (Expression)
	{
		delete Expression;
		Expression = 0;
	}

	String::Release();
}

void Xpath::Destroy()
{
	if (Expression)
	{
		delete Expression;
		Expression = 0;
	}

	String::Destroy();
}

void Xpath::Print(String & string)
{
	string << "<hv:xpath query=\"" << *this <<"\">\n";
	if (Expression && Expression->Result)
	{
		if (Expression->Result->Type.Is(XpathObject::TYPE_SEQUENCE))
		{
			((XpathSequence*)Expression->Result)->Print(string);
		}
		else
		{
			Expression->Result->XpathObject::Print(string);

		}
	}

	string << "</hv:xpath>\n";
}

void Xpath::Compile()
{
	if ( Expression )
	{
		delete Expression;
		Expression=0;
	}

	if ( ! IsEmpty() )
	{
		XpathParser parser;
		Expression = parser.Parse(*this);
		parser.Report();
	}
}

void Xpath::Compile(const String & expression)
{
	Construct(expression);
	Compile();
}

XpathObject * Xpath::operator() ()
{
	if (Expression)
		return Expression->Result;

	return 0;
}

bool Xpath::Match(Reason::System::Object *object, char * data, int size)
{
	Evaluate(object,data,size);
	XpathBoolean boolean;
	return ((*this)())?boolean.Boolean((*this)()).Value:false;
}

bool Xpath::Match(XpathState & state, char * data, int size)
{
	Evaluate(state,data,size);
	XpathBoolean boolean;
	return ((*this)())?boolean.Boolean((*this)()).Value:false;
}

bool Xpath::Match(Reason::System::Object *object)
{
	Evaluate(object,*this);
	XpathBoolean boolean;
	return ((*this)())?boolean.Boolean((*this)()).Value:false;
}

bool Xpath::Match(XpathState & state)
{
	Evaluate(state,*this);
	XpathBoolean boolean;
	return ((*this)())?boolean.Boolean((*this)()).Value:false;
}

XpathSequence * Xpath::Select(Reason::System::Object *object, char * data, int size)
{
	Construct(data,size);
	Compile();
	return Select(object);
}

XpathSequence * Xpath::Select(XpathState & state, char * data, int size)
{
	Construct(data,size);
	Compile();
	return Select(state);
}

XpathSequence * Xpath::Select(Reason::System::Object * object)
{
	if (object == 0)
	{
		OutputWarning("XpathObjectNodeSet::Select - Context was null.\n");
		return 0;
	}

	if (object->InstanceOf(XmlMarkup::Instance) && ((XmlMarkup*)object)->IsEmpty())
	{
		OutputWarning("Xpath::Select - Context was markup, but it is empty.\n");
		return 0;
	}

	if (! IsCompiled()) Compile();

	if ( Expression )
	{
		XpathState state(this);
		state.Push(object,1,1);
		Expression->Evaluate(state);

		if ( Expression->Result->IsSequence() )
		{

			((XpathSequence*)Expression->Result)->Forward();
			return (XpathSequence*)Expression->Result;
		}
		else
		{
			OutputWarning("Xpath::Select - Expression does not evaluate to a sequence, try using Evaluate().\n");
			return 0;
		}
	}
	else
	{
		OutputError("Xpath::Select - Expression \"%s\"does not compile.\n",Print());
		return 0;
	}
}

XpathSequence * Xpath::Select(XpathState & state)
{

	if (state.Context() == 0)
	{
		OutputWarning("XpathObjectNodeSet::Select - Context was null.\n");
		return 0;
	}

	if (! IsCompiled()) Compile();

	if ( Expression )
	{
		Expression->Evaluate(state);
		if ( Expression->Result->IsSequence() )
		{

			((XpathSequence*)Expression->Result)->Forward();
			return (XpathSequence*)Expression->Result;
		}
		else
		{
			OutputWarning("Xpath::Select - Expression does not evaluate to a sequence, try using Evaluate().\n");
			return 0;
		}
	}
	else
	{
		OutputError("Xpath::Select - Expression \"%s\"does not compile.\n",Print());
		return 0;
	}
}

void Xpath::Select(XpathSequence &sequence, Object *object, char * data, int size)
{
	Construct(data,size);
	Select(sequence,object);
}

void Xpath::Select(XpathSequence &sequence, XpathState & state, char * data, int size)
{
	Construct(data,size);
	Select(sequence,state);
}

void Xpath::Select(XpathSequence &sequence, Object *object)
{
	if (object == 0)
	{
		OutputWarning("XpathObjectNodeSet::Select - Context was null.\n");
		return;
	}

	if (object->InstanceOf(XmlMarkup::Instance) && ((XmlMarkup*)object)->IsEmpty())
	{
		OutputWarning("Xpath::Select - Context was markup, but it is empty.\n");
		return;
	}

	if (! IsCompiled())	Compile();

	if (Expression)
	{
		XpathState state(this);
		state.Push(object,1,1);

		if (Expression->Type.Is(XpathExpression::EXPRESSION_PATH))
		{
			((XpathExpressionStep*)Expression)->Select(sequence,state);

			XpathExpressionStep * step = (XpathExpressionStep *)Expression;
			while (step->Next) step = step->Next;
			if (step->Step == XpathExpressionStep::STEP_AXIS)
			{

				sequence.OrdinalSort();

			}
			else
			if (step->Step == XpathExpressionStep::STEP_FILTER)
			{

				sequence.OrdinalSort();
			}

			sequence.Forward();
		}
		else
		if (Expression->Type.Is(XpathExpression::EXPRESSION_BINARY_UNION) || Expression->Type.Is(XpathExpression::EXPRESSION_BINARY_INTERSECTION) || Expression->Type.Is(XpathExpression::EXPRESSION_BINARY_DIFFERENCE))
		{
			XpathSequence * result = (XpathSequence *)Expression->Evaluate(state);
			if (result) 
			{
				sequence.Take(*result);
				sequence.Forward();
			}

		}
		else
		{
			OutputWarning("Xpath::Select - Expression does not evaluate to a sequence, try using Evaluate().\n");
			return;
		}
	}
	else
	{
		OutputError("Xpath::Select - Expression \"%s\"does not compile.\n",Print());
		return;
	}
}

void Xpath::Select(XpathSequence &sequence, XpathState & state)
{

	if (state.Context() == 0)
	{
		OutputWarning("XpathObjectNodeSet::Select - Context was null.\n");
		return;
	}

	if (! IsCompiled())	Compile();

	if (Expression)
	{
		if (Expression->Type.Is(XpathExpression::EXPRESSION_PATH))
		{
			((XpathExpressionStep*)Expression)->Select(sequence,state);

			XpathExpressionStep * step = (XpathExpressionStep *)Expression;
			while (step->Next) step = step->Next;
			if (step->Step == XpathExpressionStep::STEP_AXIS)
			{

				sequence.OrdinalSort();

			}
			else
			if (step->Step == XpathExpressionStep::STEP_FILTER)
			{
				sequence.OrdinalSort();
			}

			sequence.Forward();
		}
		else
		if (Expression->Type.Is(XpathExpression::EXPRESSION_BINARY_UNION) || Expression->Type.Is(XpathExpression::EXPRESSION_BINARY_INTERSECTION) || Expression->Type.Is(XpathExpression::EXPRESSION_BINARY_DIFFERENCE))
		{
			XpathSequence * result = (XpathSequence *)Expression->Evaluate(state);
			if (result) 
			{
				sequence.Take(*result);
				sequence.Forward();
			}
		}
		else
		{
			OutputWarning("Xpath::Select - Expression does not evaluate to a sequence, try using Evaluate().\n");
			return;
		}
	}
	else
	{
		OutputError("Xpath::Select - Expression \"%s\"does not compile.\n",Print());
		return;
	}
}

XpathObject * Xpath::Evaluate(Reason::System::Object *object, char * data, int size)
{
	Construct(data,size);
	Compile();
	return Evaluate(object);
}

XpathObject * Xpath::Evaluate(XpathState & state, char * data, int size)
{
	Construct(data,size);
	Compile();
	return Evaluate(state);
}

XpathObject * Xpath::Evaluate(Reason::System::Object *object)
{
	if (object == 0)
	{
		OutputWarning("Xpath::Evaluate - Context was null.\n");
		return 0;
	}

	if (object->InstanceOf(XmlMarkup::Instance) && ((XmlMarkup*)object)->IsEmpty())
	{
		OutputWarning("Xpath::Evaluate - Context was markup, but it is empty.\n");
		return 0;
	}

	if (! IsCompiled())	Compile();

	if (Expression)
	{
		XpathState state(this);
		state.Push(object,1,1);

		XpathObject * result = Expression->Evaluate(state);

		if (result->IsSequence()) ((XpathSequence*)result)->Forward();
		return result;
	}
	else
	{
		OutputError("Xpath::Evaluate - XPath expression could not be compiled.\n");
		return 0;
	}
}

XpathObject * Xpath::Evaluate(XpathState & state)
{

	if (state.Context() == 0)
	{
		OutputWarning("Xpath::Evaluate - Context was null.\n");
		return 0;
	}

	if (! IsCompiled())	Compile();

	if (Expression)
	{
		XpathObject * result = Expression->Evaluate(state);

		if (result->IsSequence()) ((XpathSequence*)result)->Forward();
		return result;
	}
	else
	{
		OutputError("Xpath::Evaluate - XPath expression could not be compiled.\n");
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpressionLiteral::XpathExpressionLiteral():
	XpathExpressionPrimary(EXPRESSION_PRIMARY_LITERAL)
{
}

XpathExpressionLiteral::~XpathExpressionLiteral()
{
	if (Literal) delete Literal;
}

void XpathExpressionLiteral::Print(String & string)
{
	Literal->Print(string);
}

XpathObject * XpathExpressionLiteral::Evaluate(XpathState & state)
{

	return Literal;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpressionVariable::XpathExpressionVariable():
	XpathExpressionPrimary(EXPRESSION_PRIMARY_VARIABLE)
{

}

XpathExpressionVariable::~XpathExpressionVariable()
{

}

XpathObject * XpathExpressionVariable::Evaluate(XpathState & state)
{

	OutputError("XpathExpressionVariable::Evaluate - Variable reference evaluation is not implemented yet.\n");
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpressionPredicate::XpathExpressionPredicate():
	XpathExpression(XpathExpression::EXPRESSION_PREDICATE)
{

}

XpathExpressionPredicate::XpathExpressionPredicate(XpathExpression *expression):
	XpathExpression(XpathExpression::EXPRESSION_PREDICATE),Expression(expression)
{

}

XpathExpressionPredicate::~XpathExpressionPredicate()
{

	if (Expression)
		delete Expression;
}

void XpathExpressionPredicate::Print(String & string)
{
	string << "[";
	Expression->Print(string);
	string << "]";
}

XpathObject * XpathExpressionPredicate::Evaluate(XpathState & state)
{
	Release();
	Result = new XpathBoolean();
	((XpathBoolean *)Result)->Value = Match(state);
	return Result;
}

bool XpathExpressionPredicate::Match(XpathState & state)
{

	if (Expression->Type.Is(XpathExpression::EXPRESSION_PATH))
	{

		return ((XpathExpressionStep *)Expression)->Match(state);
	}
	else
	if  (Expression->Type.Is(XpathExpression::EXPRESSION))
	{

		XpathObject *result = Expression->Evaluate(state);

		if (result)
		{
			if (result->Type.Is(XpathObject::TYPE_SEQUENCE))
			{
				return ! ((XpathSequence*)result)->IsEmpty();
			}
			else
			if (result->Type.Is(XpathObject::TYPE_NUMBER))
			{

				return ((XpathNumber*)result)->Value == state.Position();
			}
			else
			if (result->Type.Is(XpathObject::TYPE_BOOLEAN))
			{
				return ((XpathBoolean*)result)->Value;
			}
			else
			{

				XpathBoolean boolean;
				boolean.Boolean(result);
				return boolean.Value;
			}
		}
		else
		{

			return false;
		}
	}
	else
	{
		OutputError("XpathExpressionPredicate::Match - Unrecognised predicate type, should be a location path or an expression, primary or otherwise.\n");
		return false;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XpathExpressionPredicateList::XpathExpressionPredicateList():
	ContextDimensions(false),ContextAtomic(false),ContextPosition(0),ContextPositionMin(0),ContextPositionMax(0)
{

}

XpathExpressionPredicateList::~XpathExpressionPredicateList()
{
	Destroy();
}

bool XpathExpressionPredicateList::Match(XpathState & state)
{
	if (Length() > 0)
	{

		if (ContextPosition != 0)
		{
			if (state.Position() != ContextPosition)
				return false;

			if (ContextAtomic && state.Position() == ContextPosition)
				return true;
		}

		if (ContextPositionMin != 0)
		{
			if (state.Position() < ContextPositionMin)
				return false;

			if (ContextAtomic && state.Position() >= ContextPositionMin)
				return true;
		}

		if (ContextPositionMax != 0)
		{
			if (state.Position() > ContextPositionMax)
				return false;

			if (ContextAtomic && state.Position() <= ContextPositionMax)
				return true;
		}

		for (int p=0;p<Length();++p)
		{
			if (! (*this)[p]->Match(state) )
			{

				return false;
			}
		}
	}

	return true;
}

bool XpathExpressionPredicateList::Contains(const char *expression)
{

	for (int p=0;p<Length();++p)
	{
		if ((*this)[p]->Expression->Token.Contains(expression))
			return true;
	}

	return false;
}

void XpathExpressionPredicateList::Position()
{

	if ( Contains("last") )
		ContextDimensions = true;

	XpathExpression *expression;

	for (int p=0;p<Length();++p)
	{

		expression = (*this)[p]->Expression;

		XpathState state;
		XpathObject *result = expression->Evaluate(state);

		if (result && result->Type.Is(XpathObject::TYPE_NUMBER))
		{

			ContextPosition = (int)((XpathNumber*)result)->Value;
			if (Length()==1)ContextAtomic=true;
			return;
		}
		else
		{
			Position(expression);
		}
	}

}

void XpathExpressionPredicateList::Position(XpathExpression *expression, bool atomic)
{
	if (expression->Type.Is(XpathExpression::EXPRESSION_PRIMARY_PARENTHESISED))
	{
		Position(((XpathExpressionParenthesised*)expression)->Expression);
	}
	else
	if (expression->Type.Is(XpathExpression::EXPRESSION_BINARY))
	{
		XpathExpressionBinary *binary = (XpathExpressionBinary*)expression;
		XpathObject * result=0;
		XpathState state;

		if (binary->OperandOne->Type.Is(XpathExpression::EXPRESSION_PRIMARY_FUNCTION)
			&& ((XpathExpressionFunction*)binary->OperandOne)->Token.Is("position") )
		{
			result = binary->OperandTwo->Evaluate(state);
			if (result)
			{
				XpathNumber number;
				number.Number(result);

				switch (binary->Type)
				{
				case XpathExpression::EXPRESSION_BINARY_EQUAL:
					ContextPosition = (int)number.Value;
					break;
				case XpathExpression::EXPRESSION_BINARY_LESS_THAN:
					ContextPositionMax = (int)number.Value-1;
					break;
				case XpathExpression::EXPRESSION_BINARY_LESS_THAN_OR_EQUAL:
					ContextPositionMax = (int)number.Value;
					break;
				case XpathExpression::EXPRESSION_BINARY_GREATER_THAN:
					ContextPositionMin = (int)number.Value+1;
					break;
				case XpathExpression::EXPRESSION_BINARY_GREATER_THAN_OR_EQUAL:
					ContextPositionMin = (int)number.Value;
					break;
				}

				if (atomic && Length()==1) ContextAtomic = true;

			}
		}
		else
			if (binary->OperandTwo->Type.Is(XpathExpression::EXPRESSION_PRIMARY_FUNCTION)
			&& ((XpathExpressionFunction*)binary->OperandTwo)->Token.Is("position") )
		{
			result = binary->OperandOne->Evaluate(state);
			if (result)
			{
				XpathNumber number;
				number.Number(result);

				switch (binary->Type)
				{
				case XpathExpression::EXPRESSION_BINARY_EQUAL:
					ContextPosition = (int)number.Value;
					break;
				case XpathExpression::EXPRESSION_BINARY_LESS_THAN:
					ContextPositionMin = (int)number.Value+1;
					break;
				case XpathExpression::EXPRESSION_BINARY_LESS_THAN_OR_EQUAL:
					ContextPositionMin = (int)number.Value;
					break;
				case XpathExpression::EXPRESSION_BINARY_GREATER_THAN:
					ContextPositionMax = (int)number.Value-1;
					break;
				case XpathExpression::EXPRESSION_BINARY_GREATER_THAN_OR_EQUAL:
					ContextPositionMax = (int)number.Value;
					break;
				}

				if (atomic && Length()==1) ContextAtomic = true;
			}
		}
		else
		{
			Position(binary->OperandOne,false);
			Position(binary->OperandTwo,false);
		}

	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity XpathObject::Instance;

XpathObject::XpathObject():
	Type(TYPE_NONE),References(0)
{

}

XpathObject::XpathObject(unsigned char type):
	Type(type),References(0)
{

}

XpathObject::~XpathObject()
{

}

int XpathObject::Compare(XpathObject *object, int comparitor)
{

	return this - object;
}

void XpathObject::Print(String & string)
{

	string << "<hv:xpath-object value=\"";

	string << "\"/>\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int XpathNavigator::Select()
{
	Release();
	Path->Select(*this,Context);
	return Count;
}

int XpathNavigator::Select(XpathState & state, char * expression, int expressionSize)
{
	Path->Construct(expression,expressionSize);
	Release();
	Path->Select(*this,state);
	return Count;
}

int XpathNavigator::Select(XpathState & state)
{
	Release();
	Path->Select(*this,state);
	return Count;
}

int XpathNavigator::Select(Object * context)
{
	Release();
	Path->Select(*this,context);
	return Count;
}

int XpathNavigator::Select(Object * context, const char * expression)
{
	Path->Construct(expression);
	Release();
	Path->Select(*this,context);
	return Count;
}

int XpathNavigator::Select(Object * context, const Sequence &expression)
{
	Path->Construct(expression);
	Release();
	Path->Select(*this,context);
	return Count;
}

int XpathNavigator::Select(const char * expression)
{
	Path->Construct(expression);
	Release();
	Path->Select(*this,Context);
	return Count;
}

int XpathNavigator::Select(const Sequence &expression)
{
	Path->Construct(expression);
	Release();
	Path->Select(*this,Context);
	return Count;
}

void XpathNavigator::NavigateParent()
{
	if (Context->InstanceOf(XmlObject::Instance))
		Context = ((XmlObject*)Context)->Parent;
	else
	if (Context->InstanceOf(XmlAttribute::Instance))
		Context = ((XmlAttribute*)Context)->Parent;
}

void XpathNavigator::NavigateChild()
{
	if (Context->InstanceOf(XmlObject::Instance))
		Context = ((XmlObject*)Context)->Child;
}

void XpathNavigator::NavigateForward()
{
	if (Context->InstanceOf(XmlObject::Instance))
		Context = ((XmlObject*)Context)->FirstSibling();
	else
	if (Context->InstanceOf(XmlAttribute::Instance))
		while(((XmlAttribute*)Context)->Prev)
			Context = ((XmlAttribute*)Context)->Prev;
}

void XpathNavigator::NavigateReverse()
{
	if (Context->InstanceOf(XmlObject::Instance))
		Context = ((XmlObject*)Context)->LastSibling()->Before;
	else
	if (Context->InstanceOf(XmlAttribute::Instance))
		while(((XmlAttribute*)Context)->Next)
			Context = ((XmlAttribute*)Context)->Next;
}

void XpathNavigator::NavigateNext()
{
	if (Context->InstanceOf(XmlObject::Instance))
		Context = ((XmlObject*)Context)->After->After;
	else
	if (Context->InstanceOf(XmlAttribute::Instance))
		Context = ((XmlAttribute*)Context)->Next;
}

void XpathNavigator::NavigatePrev()
{
	if (Context->InstanceOf(XmlObject::Instance))
		Context = ((XmlObject*)Context)->Before->Before;
	else
	if (Context->InstanceOf(XmlAttribute::Instance))
		Context = ((XmlAttribute*)Context)->Prev;
}

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

