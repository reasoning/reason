
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
#ifndef LANGUAGE_XML_PATH_H
#define LANGUAGE_XML_PATH_H

#include "reason/reason.h"
#include "reason/system/object.h"
#include "reason/structure/objects/objects.h"

#include "reason/system/parser.h"
#include "reason/system/string.h"
#include "reason/system/number.h"
#include "reason/system/time.h"

#include "reason/language/xml/xml.h"
#include "reason/language/xml/parser.h"
#ifdef REASON_USING_XML_STYLE
#include "reason/language/xml/style.h"
#endif

#include "reason/structure/map.h"

using namespace Reason::System;
using namespace Reason::Structure;
using namespace Reason::Language::Xml;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Language { namespace Xpath {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathObject : public Object
{
public:

	enum ObjectTypes
	{
		TYPE_NONE		=0,
		TYPE_STRING		=(1),
		TYPE_BOOLEAN	=(1)<<1,
		TYPE_NUMBER		=(1)<<2,

		TYPE_SEQUENCE	=(1)<<3,
	};

	static Identity Instance;
	Identity & Identify() {return Instance;}

	Bitmask8 Type;
	int References;

	bool IsNone()		{return Type.Is(TYPE_NONE);};
	bool IsString()		{return Type.Is(TYPE_STRING);};
	bool IsBoolean()	{return Type.Is(TYPE_BOOLEAN);};

	bool IsSequence()	{return Type.Is(TYPE_SEQUENCE);};
	bool IsNumber()		{return Type.Is(TYPE_NUMBER);};

	XpathObject();
	XpathObject(unsigned char type);
	virtual ~XpathObject();

	void Increment()
	{
		++References;
	}

	void Decrement()
	{
		--References;
		if (References <= 0)
			delete this;
	}

	virtual int Compare(XpathObject *object, int comparitor=Comparable::COMPARE_GENERAL);

	virtual void Print(String & string);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathSequence : public XpathObject, public Reason::Structure::Objects::Enumeration

{
public:

	enum SequenceComposition
	{
		COMPOSITION_VOID		=(1),

		COMPOSITION_TEXT		=(1)<<1,	
		COMPOSITION_ELEMENT		=(1)<<2,	
		COMPOSITION_ATTRIBUTE	=(1)<<3,	
		COMPOSITION_NAMESPACE	=(1)<<4,	

		COMPOSITION_MARKUP		=(1)<<6,	
		COMPOSITION_STYLE		=(1)<<7,

		COMPOSITION_STRING		=(1)<<8,
		COMPOSITION_NUMBER		=(1)<<9,
		COMPOSITION_BOOLEAN		=(1)<<10,
		COMPOSITION_SEQUENCE	=(1)<<11,

		COMPOSITION_OBJECT		=COMPOSITION_TEXT|COMPOSITION_ELEMENT,
	};

	Bitmask32 Composition;

	Reason::Structure::Objects::Hashtable Table;

	XpathSequence(const XpathSequence & sequence);
	XpathSequence(Reason::System::Object * object);
	XpathSequence();
	~XpathSequence();

	virtual void Print(String & string);

	bool IsHeterogenous();	
	bool IsHomogenous();	
	bool IsAtomic()			{return Count==1;};

	bool IsText()			{return !IsEmpty() && IsHomogenous() && HasText();}
	bool IsElements()		{return !IsEmpty() && IsHomogenous() && HasElements();}
	bool IsAttributes()		{return !IsEmpty() && IsHomogenous() && HasAttributes();}
	bool IsNamespaces()		{return !IsEmpty() && IsHomogenous() && HasNamespaces();}
	bool IsObjects()		{return !IsEmpty() && HasObjects() && !Composition.Is(~COMPOSITION_OBJECT);}
	bool IsMarkup()			{return !IsEmpty() && IsHomogenous() && HasMarkup();}
	bool IsStyles()			{return !IsEmpty() && IsHomogenous() && HasStyles();}

	bool IsStrings()		{return !IsEmpty() && IsHomogenous() && HasStrings();}
	bool IsNumbers()		{return !IsEmpty() && IsHomogenous() && HasNumbers();}
	bool IsBooleans()		{return !IsEmpty() && IsHomogenous() && HasBooleans();}
	bool IsSequences()		{return !IsEmpty() && IsHomogenous() && HasSequences();}

	bool HasText()			{return Composition.Is(COMPOSITION_TEXT);}
	bool HasElements()		{return Composition.Is(COMPOSITION_ELEMENT);}
	bool HasAttributes()	{return Composition.Is(COMPOSITION_ATTRIBUTE);}
	bool HasNamespaces()	{return Composition.Is(COMPOSITION_NAMESPACE);}
	bool HasObjects()		{return Composition.Is(COMPOSITION_OBJECT);}
	bool HasMarkup()		{return Composition.Is(COMPOSITION_MARKUP);}
	bool HasStyles()		{return Composition.Is(COMPOSITION_STYLE);}

	bool HasStrings()		{return Composition.Is(COMPOSITION_STRING);}
	bool HasNumbers()		{return Composition.Is(COMPOSITION_NUMBER);}
	bool HasBooleans()		{return Composition.Is(COMPOSITION_BOOLEAN);}
	bool HasSequences()		{return Composition.Is(COMPOSITION_SEQUENCE);}

	void Evaluate(String &result, bool padded=false);
	int Compare(XpathObject *object, int comparitor=Comparable::COMPARE_GENERAL);

	void Release();
	void Destroy();

	int Ordinal(Reason::System::Object * object);
	int OrdinalCompare(Reason::System::Object * object, Reason::System::Object * right);
	void OrdinalSort(int order=ORDER_ASCENDING);

	void InsertEntry(Reason::Structure::Objects::Entry *entry,Reason::Structure::Objects::Entry *sentinel, int placement=PLACE_AFTER);

	Reason::System::Object * Select(Reason::System::Object *object)
	{

		return Table.Select(object,(int)object,COMPARE_INSTANCE);

	}

	void Insert(Reason::System::Object *object, Reason::System::Object *sentinel, int placement=PLACE_AFTER)
	{
		assert(object!=0);

		if (!Table.Select(object,(int)object,COMPARE_INSTANCE))
		{
			Table.Insert(object,(int)object);
			Enumeration::Insert(object,sentinel,placement);
		}
	}

	void Insert(Reason::System::Object *object, Reason::Structure::Objects::Entry * index, int placement=PLACE_AFTER)
	{

		assert(object!=0);

		if (!Table.Select(object,(int)object,COMPARE_INSTANCE))
		{

			Table.Insert(object,(int)object);
			Enumeration::Insert(object,index,placement);
		}
	}

	void Append(Reason::System::Object * object)
	{
		Enumeration::Append(object);
	}

	void Prepend(Reason::System::Object * object)
	{
		Enumeration::Prepend(object);
	}

	void Remove(Reason::System::Object * object)
	{
		assert(object!=0);

		Table.Remove(object,(int)object);
		Enumeration::Remove(object);
		if (object->InstanceOf(XpathObject::Instance))
			((XpathObject*)object)->Decrement();
	}

	void Delete(Reason::System::Object * object)
	{
		assert(object!=0);

		Table.Delete(object,(int)object);
		Enumeration::Delete(object);

	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathContext
{
public:

	enum ContextType
	{
		TYPE_OBJECT		=(1),	
		TYPE_ATTRIBUTE	=(1)<<1,
		TYPE_NAMESPACE	=(1)<<2,
		TYPE_STYLE		=(1)<<3,
		TYPE_MARKUP		=(1)<<4,

		TYPE_STRING		=(1)<<5,
		TYPE_NUMBER		=(1)<<6,
		TYPE_BOOLEAN	=(1)<<7,
		TYPE_SEQUENCE	=(1)<<8,
	};

	Bitmask16 Type;
	Reason::System::Object * Object;
	int Position;
	int Size;

	XpathContext():Type(0),Object(0),Position(0),Size(0) {}

	XpathContext(XmlObject * object, int position=0, int size=0):
		Type(TYPE_OBJECT),Object(object),Position(position),Size(size)
	{
	}

	XpathContext(XmlAttribute * attribute, int position=0, int size=0):
		Type(TYPE_ATTRIBUTE),Object(attribute),Position(position),Size(size)
	{
	}

	XpathContext(XmlNamespace * name, int position=0, int size=0):
		Type(TYPE_NAMESPACE),Object(name),Position(position),Size(size)
	{
	}

	#ifdef REASON_USING_XML_STYLE
	XpathContext(XmlStyle * style, int position=0, int size=0):
		Type(TYPE_STYLE),Object(style),Position(position),Size(size)
	{
	}
	#endif

	XpathContext(XmlMarkup * markup, int position=0, int size=0):
		Type(TYPE_MARKUP),Object(markup),Position(position),Size(size)
	{
	}

	XpathContext(XpathObject * object, int position=0, int size=0):
		Type(0),Object(object),Position(position),Size(size)
	{
		switch(object->Type)
		{
		case XpathObject::TYPE_STRING: Type = TYPE_STRING; break;
		case XpathObject::TYPE_NUMBER: Type = TYPE_NUMBER; break;
		case XpathObject::TYPE_BOOLEAN: Type = TYPE_BOOLEAN; break;
		case XpathObject::TYPE_SEQUENCE: Type = TYPE_SEQUENCE; break;
		}
	}

	~XpathContext()
	{
	}

	bool IsObject() {return Type == TYPE_OBJECT;}
	bool IsMarkup() {return Type == TYPE_MARKUP;}
	bool IsStyle() {return Type == TYPE_STYLE;}
	bool IsNamespace() {return Type == TYPE_NAMESPACE;}
	bool IsAttribute() {return Type == TYPE_ATTRIBUTE;}

	bool IsString() {return Type == TYPE_STRING;}
	bool IsNumber() {return Type == TYPE_NUMBER;}
	bool IsBoolean() {return Type == TYPE_BOOLEAN;}	
	bool IsSequence() {return Type == TYPE_SEQUENCE;}

};

class XpathScope
{
public:

	enum ScopeType
	{
		TYPE_OBJECT		=(1),	
		TYPE_MARKUP		=(1)<<1,
	};

	XpathScope(XmlObject * object):Type(TYPE_OBJECT),Object(object) {}
	XpathScope(XmlMarkup * markup):Type(TYPE_MARKUP),Object(markup) {}
	XpathScope():Type(0),Object(0) {}

	bool IsObject() {return Type == TYPE_OBJECT;}
	bool IsMarkup() {return Type == TYPE_MARKUP;}

	Bitmask8 Type;
	Reason::System::Object * Object;

};

class Xpath;

class XpathState
{
public:

	Xpath * Path;
	Reason::Structure::Array<XpathContext> Contexts;
	Reason::Structure::Array<XpathScope> Scopes;

	XpathState(Xpath * path):Path(path) {}
	XpathState():Path(0) {}

	void Push(XmlObject * object, int position=0, int size=0)
	{
		Contexts.Append(XpathContext(object,position,size));
		if (object->InstanceOf(XmlMarkup::Instance))
			Scopes.Append(XpathScope((XmlObject*)object));
	}

	void Push(XmlAttribute * attribute, int position=0, int size=0)
	{
		Contexts.Append(XpathContext(attribute,position,size));
	}

	void Push(XmlNamespace * name, int position=0, int size=0)
	{
		Contexts.Append(XpathContext(name,position,size));
	}

	#ifdef REASON_USING_XML_STYLE
	void Push(XmlStyle * style, int position=0, int size=0)
	{
		Contexts.Append(XpathContext(style,position,size));
	}
	#endif

	void Push(XpathObject * object, int position=0, int size=0)
	{
		Contexts.Append(XpathContext(object,position,size));
	}

	void Push(Object * object, int position=0, int size=0)
	{
		if (object->InstanceOf(XmlObject::Instance))
			Push((XmlObject*)object,position,size);
		else
		if (object->InstanceOf(XmlAttribute::Instance))
			Push((XmlAttribute*)object,position,size);
		else
		if (object->InstanceOf(XmlNamespace::Instance))
			Push((XmlNamespace*)object,position,size);
		else
		#ifdef REASON_USING_XML_STYLE
		if (object->InstanceOf(XmlStyle::Instance))
			Push((XmlStyle*)object,position,size);
		else
		#endif
		if (object->InstanceOf(XpathObject::Instance))
			Push((XpathObject*)object,position,size);
	}

	Reason::System::Object * Peek()
	{
		return (Contexts.Size)?Contexts[Contexts.Size-1].Object:0;
	}

	Reason::System::Object * Pop()
	{
		if (Contexts.Size==0) return 0;

		XpathContext context = Contexts.RemoveAt(Contexts.Size-1)();		
		Object * object = context.Object;
		if (object->InstanceOf(XmlMarkup::Instance))
			Scopes.RemoveAt(Scopes.Size-1);

		return object;
	}

	int Count()
	{
		return Contexts.Size;
	}

	int Position()
	{
		return (Contexts.Size)?Contexts[Contexts.Size-1].Position:0;
	}

	int Size()
	{
		return (Contexts.Size)?Contexts[Contexts.Size-1].Size:0;
	}

	int Type()
	{
		return (Contexts.Size)?(int)Contexts[Contexts.Size-1].Type:0;
	}

	XpathContext * Context()
	{
		return (Contexts.Size)?&Contexts[Contexts.Size-1]:0;
	}

	XpathScope * Scope()
	{
		return (Scopes.Size)?&Scopes[Scopes.Size-1]:0;
	}

	XmlObject * Root()
	{
		XpathScope * scope = Scope();
		XpathContext * context = Context();

		if (scope)
		{
			return (XmlObject*)scope->Object;
		}
		else
		if (context && (context->Type.Is(XpathContext::TYPE_OBJECT) || context->Type.Is(XpathContext::TYPE_MARKUP)))
		{

			return ((XmlObject*)context->Object)->Root();

		}
		else
		{
			return 0;
		}
	}

	Reason::System::Object * operator () (void)
	{
		return Peek();
	}

	Reason::System::Object * operator [] (int index)
	{
		if (index < 0 || index > Contexts.Size-1) return 0;
		return Contexts[index].Object;
	}
};

class XpathContextAuto
{
public:

	XpathState & State;

	XpathContextAuto(XpathState & state, Object * object, int position=0, int size=0):State(state) {state.Push(object,position,size);}
	XpathContextAuto(XpathState & state, XmlObject * object, int position=0, int size=0):State(state) {state.Push(object,position,size);}
	XpathContextAuto(XpathState & state, XmlAttribute * attribute, int position=0, int size=0):State(state) {state.Push(attribute,position,size);}
	XpathContextAuto(XpathState & state, XmlNamespace * name, int position=0, int size=0):State(state) {state.Push(name,position,size);}
	#ifdef REASON_USING_XML_STYLE
	XpathContextAuto(XpathState & state, XmlStyle * style, int position=0, int size=0):State(state) {state.Push(style,position,size);}
	#endif
	XpathContextAuto(XpathState & state, XmlMarkup * markup, int position=0, int size=0):State(state) {state.Push(markup,position,size);}
	~XpathContextAuto() {State.Pop();}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpression : public Printable
{
public:

	enum Type
	{

		NONE									=0,
		EXPRESSION								=0xFFFFFFFF,

		EXPRESSION_BINARY_OR					=(1),
		EXPRESSION_BINARY_AND					=(1)<<1,

		EXPRESSION_BINARY_EQUAL					=(1)<<2,
		EXPRESSION_BINARY_NOT_EQUAL				=(1)<<3,
		EXPRESSION_BINARY_LESS_THAN				=(1)<<4,
		EXPRESSION_BINARY_LESS_THAN_OR_EQUAL	=(1)<<5,
		EXPRESSION_BINARY_GREATER_THAN			=(1)<<6,
		EXPRESSION_BINARY_GREATER_THAN_OR_EQUAL	=(1)<<7,

		EXPRESSION_BINARY_ADD					=(1)<<8,
		EXPRESSION_BINARY_SUBTRACT				=(1)<<9,

		EXPRESSION_BINARY_MULTIPLY				=(1)<<10,
		EXPRESSION_BINARY_DIV					=(1)<<11,
		EXPRESSION_BINARY_MOD					=(1)<<12,

		EXPRESSION_BINARY_UNION					=(1)<<13,	
		EXPRESSION_BINARY_INTERSECTION			=(1)<<14,	
		EXPRESSION_BINARY_DIFFERENCE			=(1)<<15,	

		EXPRESSION_BINARY_PRECEDING				=(1)<<16,
		EXPRESSION_BINARY_FOLLOWING				=(1)<<17,

		EXPRESSION_BINARY						=EXPRESSION_BINARY_OR|EXPRESSION_BINARY_AND|
												EXPRESSION_BINARY_EQUAL|EXPRESSION_BINARY_NOT_EQUAL|
												EXPRESSION_BINARY_LESS_THAN|EXPRESSION_BINARY_LESS_THAN_OR_EQUAL|
                                                EXPRESSION_BINARY_GREATER_THAN|EXPRESSION_BINARY_GREATER_THAN_OR_EQUAL|
												EXPRESSION_BINARY_ADD|EXPRESSION_BINARY_SUBTRACT|
												EXPRESSION_BINARY_MULTIPLY|EXPRESSION_BINARY_DIV|EXPRESSION_BINARY_MOD|
												EXPRESSION_BINARY_UNION|EXPRESSION_BINARY_INTERSECTION|EXPRESSION_BINARY_DIFFERENCE|
                                                EXPRESSION_BINARY_PRECEDING|EXPRESSION_BINARY_FOLLOWING,

		EXPRESSION_SIGNED						=(1)<<18,

		EXPRESSION_PREDICATE					=(1)<<19,

		EXPRESSION_PATH							=(1)<<20,

		EXPRESSION_PRIMARY_LITERAL				=(1)<<24,
		EXPRESSION_PRIMARY_FUNCTION				=(1)<<25,
		EXPRESSION_PRIMARY_VARIABLE				=(1)<<26,	
		EXPRESSION_PRIMARY_PARENTHESISED		=(1)<<27,
		EXPRESSION_PRIMARY						=EXPRESSION_PRIMARY_LITERAL|EXPRESSION_PRIMARY_FUNCTION|
												EXPRESSION_PRIMARY_VARIABLE|EXPRESSION_PRIMARY_PARENTHESISED

	};

	Bitmask32	Type;
	Substring	Token;

	XpathExpression(unsigned int type);
	XpathExpression();
	virtual ~XpathExpression();

	virtual XpathObject * Evaluate(XpathState & state)=0;

	void Release();		

public:

	XpathObject *	Result;	

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpressionBinary : public XpathExpression
{
public:

	XpathExpressionBinary();
	~XpathExpressionBinary();

	XpathExpression *OperandOne;
	XpathExpression *OperandTwo;

	virtual XpathObject * Evaluate(XpathState & state);

	virtual void Print(String & string);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpressionSigned : public XpathExpression
{
public:

	enum Sign
	{
		SIGN_NEGATIVE	=0,
		SIGN_POSITIVE	=1,
	};

	unsigned char	Sign;

	bool IsPositive()	{return Sign == SIGN_POSITIVE;};
	bool IsNegative()	{return Sign == SIGN_NEGATIVE;};

	XpathExpressionSigned();
	~XpathExpressionSigned();

	XpathExpression *Expression;

	virtual XpathObject * Evaluate(XpathState & state);

	virtual void Print(String & string);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpressionPredicate : public XpathExpression
{
public:

	XpathExpression * Expression;

	XpathExpressionPredicate();
	XpathExpressionPredicate(XpathExpression *expression);
	~XpathExpressionPredicate();

	virtual XpathObject * Evaluate(XpathState & state);

	bool Match(XpathState & state);

	virtual void Print(String & string);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpressionPredicateList : public Reason::Structure::List<XpathExpressionPredicate*>
{
public:

	XpathExpressionPredicateList();
	~XpathExpressionPredicateList();

	bool Match(XpathState & state);

	void Position();	

	bool	ContextDimensions;		
	bool	ContextAtomic;			

	int		ContextPosition;
	int		ContextPositionMin;
	int		ContextPositionMax;

private:

	bool Contains(const char *expression);
	void Position(XpathExpression * expression,bool atomic=true);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpressionPrimary : public XpathExpression
{
public:

	XpathExpressionPrimary(unsigned int type);
	~XpathExpressionPrimary();

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpressionLiteral : public XpathExpressionPrimary
{
public:

	XpathObject		*Literal;

	XpathExpressionLiteral();
	~XpathExpressionLiteral();

	virtual XpathObject * Evaluate(XpathState & state);

	virtual void Print(String & string);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpressionArguments : public Reason::Structure::List<XpathExpression*>
{
public:

};

class XpathExpressionFunction : public XpathExpressionPrimary
{
public:

	XpathExpressionArguments Arguments;

	XpathExpressionFunction();
	~XpathExpressionFunction();

	virtual XpathObject * Evaluate(XpathState & state);

	virtual void Print(String & string);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathString;

class XpathBoolean : public XpathObject
{
public:

	XpathBoolean(bool value);
	XpathBoolean(XpathObject *object);
	XpathBoolean();
	~XpathBoolean();

	XpathBoolean & Boolean(XpathObject *object);
	XpathBoolean & Not(XpathBoolean *boolean);
	XpathBoolean & True();
	XpathBoolean & False();

	virtual int Compare(XpathObject *object, int comparitor=Comparable::COMPARE_GENERAL);

	virtual void Print(String & string);

	bool Value;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathTime : public XpathObject
{
public:

	XpathTime(XpathObject *object);
	XpathTime();
	~XpathTime();

	int Compare(XpathObject *object, int comparitor=Comparable::COMPARE_GENERAL);

	virtual void Print(String & string);

	XpathTime & Time(XpathObject *object);

	virtual void Evaluate(XmlObject *contextObject, int contextPosition, int contextSize, XpathObject *result);
	virtual void Evaluate(XmlObject *contextObjectParent, XmlNamespace *contextObject, int contextPosition, int contextSize, XpathObject *result);
	virtual void Evaluate(XmlObject *contextObjectParent, XmlAttribute *contextObject, int contextPosition, int contextSize, XpathObject *result);

	Reason::System::Calendar Value;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathNumber :public XpathObject
{
public:

	XpathNumber(int number);
	XpathNumber(double number);
	XpathNumber(XpathObject *object);
	XpathNumber();
	~XpathNumber();

	int Compare(XpathObject *object, int comparitor=Comparable::COMPARE_GENERAL);

	virtual void Print(String & string);

	XpathNumber & Number(XpathObject *object);
	XpathNumber & Sum(XpathSequence *sequence);
	XpathNumber & Floor(XpathNumber *number);
	XpathNumber & Ceiling(XpathNumber *number);
	XpathNumber & Round(XpathNumber *number);

	virtual void Evaluate(XmlObject *contextObject, int contextPosition, int contextSize, XpathObject *result);
	virtual void Evaluate(XmlObject *contextObjectParent, XmlNamespace *contextObject, int contextPosition, int contextSize, XpathObject *result);
	virtual void Evaluate(XmlObject *contextObjectParent, XmlAttribute *contextObject, int contextPosition, int contextSize, XpathObject *result);

	Reason::System::Number Value;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathString : public XpathObject
{
public:

	Reason::System::String Value;

	XpathString(const Sequence & sequence);
	XpathString(XpathObject *object);
	XpathString();
	~XpathString();

	XpathString & String(XpathObject *object);

	XpathString & Concat(XpathString *first,XpathString *second =0);
	XpathString & SubstringBefore(XpathString *target, XpathString *string);
	XpathString & SubstringAfter(XpathString *target, XpathString *string);
	XpathString & Substring(XpathString *string, XpathNumber *position, XpathNumber *length=0);
	XpathString & NormaliseSpace(XpathString *string = 0);
	XpathString & Translate(XpathString *string,XpathString *match,XpathString *replacement);

	bool StartsWith(XpathString * string);
	bool Contains(XpathString * string);
	int StringLength();

	static bool StartsWith(XpathString *target, XpathString *string);
	static bool Contains(XpathString *target, XpathString *string);
	static int StringLength(XpathString *string);

	int Compare(XpathObject *object, int comparitor=Comparable::COMPARE_GENERAL);
	void Print(class String & string);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpressionVariable : public XpathExpressionPrimary
{
public:

	XpathExpressionVariable();
	~XpathExpressionVariable();

	virtual XpathObject * Evaluate(XpathState & state);

	virtual void Print(String & string)	{string << Token;};

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpressionParenthesised : public XpathExpressionPrimary
{
public:

	XpathExpressionParenthesised();
	~XpathExpressionParenthesised();

	XpathExpression *Expression;

	virtual XpathObject * Evaluate(XpathState & state);

	virtual void Print(String & string);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpressionStep : public XpathExpression
{
public:

	XpathExpressionStep * Prev;
	XpathExpressionStep * Next;

	enum Step
	{
		STEP_AXIS		=(1),
		STEP_FILTER		=(1)<<1,
		STEP_SCOPE		=(1)<<2,
	};

	unsigned char		 Step;

	XpathExpressionStep();
	XpathExpressionStep(unsigned char step);
	virtual ~XpathExpressionStep();

	bool IsAxis(){return Step == STEP_AXIS;};
	bool IsFilter(){return Step == STEP_FILTER;};

	XpathExpressionStep * Append(XpathExpressionStep * step);
	XpathExpressionStep * Prepend(XpathExpressionStep * step);

	virtual void Print(String & string)=0;

	XpathObject * Evaluate(XpathState & state);

	bool Match(XpathState & state);

	virtual void Select(XpathSequence &sequence, XpathState & state, bool match = false)=0;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpressionStepScope : public XpathExpressionStep
{
public:

	XpathExpressionStepScope();
	~XpathExpressionStepScope();

	virtual void Select(XpathSequence &sequence, XpathState & state, bool match = false);
	virtual void Print(String & string);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpressionStepAxis :
	public XpathExpressionStep
{
public:

	enum AxisType
	{
		AXIS_NONE		= 0,

		AXIS_ROOT		= 0x01,

		AXIS_ATTRIBUTE	 = 0x02,
		AXIS_NAMESPACE	 = 0x04,

		AXIS_CHILD				= 0x08,
		AXIS_ANCESTOR			= 0x10,
		AXIS_ANCESTOR_OR_SELF	= 0x20,
		AXIS_DESCENDANT			= 0x40,
		AXIS_DESCENDANT_OR_SELF	= 0x80,
		AXIS_FOLLOWING			= 0x100,
		AXIS_FOLLOWING_SIBLING	= 0x200,
		AXIS_PARENT				= 0x400,
		AXIS_PRECEDING			= 0x800,
		AXIS_PRECEDING_SIBLING	= 0x1000,
		AXIS_SELF				= 0x2000,

		AXIS_STYLE				= 0x4000,

		AXIS_TYPE_FORWARD		= AXIS_ROOT|AXIS_SELF|AXIS_CHILD|AXIS_DESCENDANT|AXIS_DESCENDANT_OR_SELF|
								AXIS_FOLLOWING|AXIS_FOLLOWING_SIBLING|AXIS_ATTRIBUTE|AXIS_NAMESPACE,

		AXIS_TYPE_REVERSE		= AXIS_PARENT|AXIS_ANCESTOR|AXIS_ANCESTOR_OR_SELF|AXIS_PRECEDING|AXIS_PRECEDING_SIBLING,

		AXIS_TYPE_ELEMENT		= 0x3FF9,
		AXIS_TYPE_ATTRIBUTE		= AXIS_ATTRIBUTE,
		AXIS_TYPE_NAMESPACE		= AXIS_NAMESPACE,
		AXIS_TYPE_STYLE			= AXIS_STYLE,
	};

	class AxisToken : public Substring
	{
	public:
		AxisToken(int type=0):Type(type){}
		Bitmask16 Type;
	};

	AxisToken Axis;

	enum TestType
	{
		TEST_NONE				=0,
		TEST_NAME_ALL			=1,		
		TEST_NAME_NAMESPACE		=2,		
		TEST_NAME_QUALIFIED		=4,		
		TEST_NAME				=TEST_NAME_ALL|TEST_NAME_NAMESPACE|TEST_NAME_QUALIFIED,

		TEST_NODE_COMMENT		=8,		
		TEST_NODE_TEXT			=16,	
		TEST_NODE_PROCESSING	=32,	
		TEST_NODE_ALL			=64,	
		TEST_NODE				=TEST_NODE_COMMENT|TEST_NODE_TEXT|TEST_NODE_PROCESSING|TEST_NODE_ALL,

		TEST_EXTENSION			=128,
	};

	class TestToken : public Substring
	{
	public:
		TestToken(int type=0):Type(type){}
		Bitmask8 Type;

	};

	TestToken Test;

	XpathExpressionArguments Arguments;
	XpathExpressionPredicateList	Predicates;

	XpathExpressionStepAxis();
	XpathExpressionStepAxis(unsigned short axis, unsigned char test);
	~XpathExpressionStepAxis();

	virtual void Print(String & string);

	bool IsForwardAxis() {return Axis.Type.Is(AXIS_TYPE_FORWARD);}
	bool IsReverseAxis() {return Axis.Type.Is(AXIS_TYPE_REVERSE);}
	bool IsStyleAxis() {return Axis.Type.Is(AXIS_TYPE_STYLE);}
	bool IsNamespaceAxis() {return Axis.Type.Is(AXIS_TYPE_NAMESPACE);}
	bool IsAttributeAxis() {return Axis.Type.Is(AXIS_TYPE_ATTRIBUTE);}

	void Select(XpathSequence &sequence, XpathState & state, bool match = false);

	void SelectObject(XpathSequence & sequence, XpathState & state, bool match = false);
	void SelectAttribute(XpathSequence & sequence, XpathState & state, bool match = false);
	void SelectNamespace(XpathSequence & sequence, XpathState & state, bool match = false);
	void SelectStyle(XpathSequence & sequence, XpathState & state, bool match = false);

	void SelectIndexed(XpathSequence &sequence, XpathState & state, bool match);
	bool CanSelectIndexed(XpathState & state);

	bool Match(XpathState & state, const XpathContext &context);
	bool MatchAttribute(XmlAttribute *contextObject);
	bool MatchNamespace(XmlNamespace *contextObject);
	bool MatchObject(XmlObject *contextObject);
	#ifdef REASON_USING_XML_STYLE
	bool MatchStyle(XmlStyle *contextObject);
	#endif

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathPlugin
{
public:

	void Select(XpathSequence & sequence, XpathState & state, bool match = false);
};

class XpathPluginManager
{

};

class XpathExtensionHandlerHandler
{
public:

	virtual XpathObject * ProcessFunction(XpathExpressionFunction * function, XpathState & state)
	{
		return 0;
	}

	virtual bool ProcessAxis(XpathExpressionStep * step, XpathSequence & sequence, XpathState & state, bool match = false)
	{
		return false;
	}

	virtual bool ProcessTest(XpathState & state, XpathContext context)
	{

		return false;
	}
};

class XpathExtension
{
public:

	enum Type
	{
		EXTENSION_FUNCTION	=(1),
		EXTENSION_STEP_AXIS	=(1)<<1,
		EXTENSION_STEP_TEST	=(1)<<2,

		EXTENSION_STEP		=EXTENSION_STEP_AXIS|EXTENSION_STEP_TEST,
	};

	Reason::Structure::Map<Pair<int,String>, XpathExtensionHandlerHandler*> Extensions;

	void Register(int type, const String & string, XpathExtensionHandlerHandler * handler)
	{
		Extensions.Insert(Pair<int,String>(type,string),handler);
	}

	void Unregister(int type, const String & string, XpathExtensionHandlerHandler * handler)
	{
		if (Extensions[Pair<int,String>(type,string)] == handler)
		{
			Extensions.Remove(Pair<int,String>(type,string));
		}
	}

	XpathExtensionHandlerHandler * Lookup(int type, const String & string)
	{
		return Extensions[Pair<int,String>(type,string)];
	}
};

class XpathAdapter
{
public:

};

class XpathAdapterInterface
{
};

class XpathFixture
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathFactory
{
public:

	Reason::Structure::Array<XpathString *> Strings;
	Reason::Structure::Array<XpathNumber *> Numbers;
	Reason::Structure::Array<XpathBoolean *> Booleans;
	Reason::Structure::Array<XpathSequence *> Sequences;

	XpathString * GetString();
	void PutString(XpathString * string);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathExpressionStepFilter :
	public XpathExpressionStep
{
public:

	XpathExpression	* Expression;
	XpathExpressionPredicateList Predicates;

	XpathExpressionStepFilter();
	XpathExpressionStepFilter(XpathExpression * expression);
	~XpathExpressionStepFilter();

	void Print(String & string);

protected:

	void Select(XpathSequence & sequence, XpathState & state, bool match = false);
};

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

class XpathParser :
	public XmlNamespaceParser,
	public XmlNamespaceHandler
{

public:

	XpathExtension * Extension;

	void Report();

	XpathParser();
	~XpathParser();

	XpathExpression * Parse(const Sequence & sequence);

protected:

	int Errors;
	int Failures;
	int Warnings;

	virtual void Catch(const unsigned int type,const char *label,const char * message,va_list args);

	bool Changed();		
	void Watch();		
	void Undo();		

	bool ParseNodeTest(XpathExpressionStepAxis &axis);
	bool ParsePredicateList(XpathExpressionPredicateList &predicates);

	XpathExpression * ParseExpression();

	XpathExpression * ParseExpressionPredicate();

	XpathExpression * ParseExpressionOr();
	XpathExpression * ParseExpressionAnd();
	XpathExpression * ParseExpressionEquality();
	XpathExpression * ParseExpressionRelational();
	XpathExpression * ParseExpressionAdditive();
	XpathExpression * ParseExpressionMultiplicative();
	XpathExpression * ParseExpressionOrdinal();
	XpathExpression * ParseExpressionUnary();
	XpathExpression * ParseExpressionUnion();
	XpathExpression * ParseExpressionIntersection();
	XpathExpression * ParseExpressionDifference();
	XpathExpression * ParseExpressionPath();
	XpathExpression * ParseExpressionPathAbsolute();
	XpathExpression * ParseExpressionPathRelative();
	XpathExpression * ParseExpressionStep();
	XpathExpression * ParseExpressionStepAxis();
	XpathExpression * ParseExpressionStepScope();
	XpathExpression * ParseExpressionStepFilter();
	XpathExpression * ParseExpressionPrimary();

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Xpath : public String
{
public:

	XpathExtension * Extension;
	XpathExpression *	Expression;

	Xpath(const Sequence &expression);
	Xpath(const char * expression);
	Xpath();
	~Xpath();

	void Construct(const Sequence &sequence)	{Construct(sequence.Data,sequence.Size);}
	void Construct(const char * data)			{Construct((char*)data,Length(data));}
	void Construct(char * data, int size);

	void Compile();
	void Compile(const String & expression);
	bool IsCompiled() {return Expression != 0;};

	XpathSequence * Select(Reason::System::Object *context, const char * expression) {return Select(context,(char*)expression,String::Length(expression));}
	XpathSequence * Select(Reason::System::Object *context, const Sequence & expression) {return Select(context,expression.Data,expression.Size);}
	XpathSequence * Select(Reason::System::Object *context, char * expression, int expressionSize);
	XpathSequence * Select(Reason::System::Object *context);

	XpathSequence * Select(XpathState & state, const char * expression) {return Select(state,(char*)expression,String::Length(expression));}
	XpathSequence * Select(XpathState & state, const Sequence & expression) {return Select(state,expression.Data,expression.Size);}
	XpathSequence * Select(XpathState & state, char * expression, int expressionSize);
	XpathSequence * Select(XpathState & state);

	void Select(XpathSequence &sequence, Reason::System::Object *context, const char * expression) {return Select(sequence,context,(char*)expression,String::Length(expression));}
	void Select(XpathSequence &sequence, Reason::System::Object *context, const Sequence & expression) {return Select(sequence,context,expression.Data,expression.Size);}
	void Select(XpathSequence &sequence, Reason::System::Object *context, char * expression, int expressionSize);
	void Select(XpathSequence &sequence, Reason::System::Object *context);

	void Select(XpathSequence &sequence, XpathState & state, const char * expression) {return Select(sequence,state,(char*)expression,String::Length(expression));}
	void Select(XpathSequence &sequence, XpathState & state, const Sequence & expression) {return Select(sequence,state,expression.Data,expression.Size);}
	void Select(XpathSequence &sequence, XpathState & state, char * expression, int expressionSize);
	void Select(XpathSequence &sequence, XpathState & state);

	XpathObject * Evaluate(Reason::System::Object *context, const char * expression) {return Evaluate(context,(char*)expression,String::Length(expression));}
	XpathObject * Evaluate(Reason::System::Object *context, const Sequence & expression) {return Evaluate(context,expression.Data,expression.Size);}
	XpathObject * Evaluate(Reason::System::Object *context, char * expression, int expressionSize);
	XpathObject * Evaluate(Reason::System::Object *context);

	XpathObject * Evaluate(XpathState & state, const char * expression) {return Evaluate(state,(char*)expression,String::Length(expression));}
	XpathObject * Evaluate(XpathState & state, const Sequence & expression) {return Evaluate(state,expression.Data,expression.Size);}
	XpathObject * Evaluate(XpathState & state, char * expression, int expressionSize);
	XpathObject * Evaluate(XpathState & state);

	bool Match(Reason::System::Object *context, const char * expression) {return Match(context,(char*)expression,String::Length(expression));}
	bool Match(Reason::System::Object *context, const Sequence & expression) {return Match(context,expression.Data,expression.Size);}
	bool Match(Reason::System::Object *context, char * expression, int expressionSize);
	bool Match(Reason::System::Object *context);

	bool Match(XpathState & state, const char * expression) {return Match(state,(char*)expression,String::Length(expression));}
	bool Match(XpathState & state, const Sequence & expression) {return Match(state,expression.Data,expression.Size);}
	bool Match(XpathState & state, char * expression, int expressionSize);
	bool Match(XpathState & state);

	void Release();
	void Destroy();

	XpathObject * operator() ();

	using String::Print;
	void Print(String & string);

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class XpathNavigator : public XpathSequence
{
protected:

	Xpath	* Path;

	XpathNavigator(Xpath * path, Object * context):Path(path),Context(context) {}
	XpathNavigator(Xpath * path):Path(path),Context(0) {}

public:

	Object * Context;

	XpathNavigator(Object * context, const char * expression):Context(context),Path(new Xpath(expression)) {}
	XpathNavigator(Object * context, const Sequence & expression):Context(context),Path(new Xpath(expression)) {}
	XpathNavigator(Object * context):Context(context),Path(new Xpath()) {}
	XpathNavigator(const char * expression):Context(0),Path(new Xpath(expression)) {}
	XpathNavigator(const Sequence & expression):Context(0),Path(new Xpath(expression)) {}
	XpathNavigator():Context(0),Path(new Xpath()) {}

	~XpathNavigator()
	{
		if (Path)
		{
			delete Path;
		}
	}

	int Select(XpathState & state, const char * expression) {return Select(state,(char*)expression,String::Length(expression));}
	int Select(XpathState & state, const Sequence & expression) {return Select(state,expression.Data,expression.Size);}
	int Select(XpathState & state, char * expression, int expressionSize);
	int Select(XpathState & state);

	int Select(Object * context, const char * expression);
	int Select(Object * context, const Sequence &expression);
	int Select(Object * context);

	int Select(const char * expression);
	int Select(const Sequence &expression);

	int Select();

	int SelectAncestors() {return Select("./ancestor::node()");}
	int SelectDescendants() {return Select("./descendant::node()");}
	int SelectChildren() {return Select("./node()");}
	int SelectSiblings() {return Select("./preceding-sibling::node()|./following-sibling::node()");}
	int SelectText() {return Select("./descendant-or-self::text()");}

	void NavigateParent();
	void NavigateChild();

	void NavigateForward();
	void NavigateReverse();
	void NavigateNext();
	void NavigatePrev();
};

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

#endif

