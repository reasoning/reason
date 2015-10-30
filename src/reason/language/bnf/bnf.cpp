
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

#include "reason/language/bnf/bnf.h"
#include "reason/language/xml/xml.h"
#include "reason/language/xml/markup.h"
#include "reason/language/xml/enumerator.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::Language::Xml;
using namespace Reason::System;

namespace Reason { namespace Language { namespace Bnf {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Bnf::Bnf():Object(0)
{

}

Bnf::~Bnf()
{

	if (Object)
		delete Object;
}

void Bnf::Construct(Sequence &grammar)
{
	BnfParser parser;
	Object = parser.Parse(grammar);
}

void Bnf::Debug(BnfToken *object, int depth)
{
	if (object == 0)
	{
		OutputMessage("Bnf::Debug\n");
		object = Object;
	}

	BnfEnumerator e(this);
	if (e.Forward())
	{
		do
		{
			OutputMessage("%s\n",e().Value.Print());
		}
		while(e.Move());
	}

}

void Bnf::Print(BnfToken *object, int depth)
{
	if (object == 0)
	{
		if (depth == 0)
		{
			OutputMessage("Bnf::Print:\n");

			if (Object == 0)
			{
				OutputMessage("Bnf::Print - Bnf is empty.\n");
				return;
			}
			else
			{
				object = Object;
			}
		}
		else
		{
			return;
		}
	}

	switch (object->Type)
	{
	case BnfToken::SEQUENCE:
		{
			Print(((BnfTokenBinary*)object)->One,depth+1);
			OutputMessage(", ");
			Print(((BnfTokenBinary*)object)->Two,depth+1);
		}
	break;
	case BnfToken::CHOICE:
		{
			Print(((BnfTokenBinary*)object)->One,depth+1);
			OutputMessage("| ");
			Print(((BnfTokenBinary*)object)->Two,depth+1);
		}
	break;
	case BnfToken::GROUP:
		{
			OutputMessage("( ");
			Print(((BnfTokenGroup*)object)->One,depth+1);
			OutputMessage(")");

			((BnfTokenGroup*)object)->Operator.Print();
		}
	break;
	case BnfToken::LABEL:
		{
			OutputMessage("%s",((BnfTokenTerminal *)object)->Value.Print());
			((BnfTokenTerminal *)object)->Operator.Print();
		}
	break;
	case BnfToken::LITERAL:
		{
			OutputMessage("%s",((BnfTokenTerminal *)object)->Value.Print());
			((BnfTokenTerminal *)object)->Operator.Print();
		}
	break;

	}

	if (depth == 0)
	OutputMessage("\n\n");

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BnfToken * BnfParser::Parse(Sequence &grammar)
{
	if (grammar.IsEmpty())
		return 0;

	Assign(grammar);

	SkipWhitespace();

	return ParseBnf();
}

BnfToken * BnfParser::ParseBnf()
{
	if (! Eof() )
	{
		return ParseSequence();
	}
	else
	{
		return 0;
	}
}

BnfToken * BnfParser::ParseSequence()
{

	BnfToken *object = ParseChoice();

	if ( !Eof() && !Is(')') && object != 0)
	{

		if (Is(','))
		{
			Next();	 
			SkipWhitespace();
		}

		BnfToken *nextObject = ParseBnf();

		if (nextObject != 0)
		{

			BnfTokenSequence *sequence = new BnfTokenSequence();
			sequence->One = object;
			sequence->Two = nextObject;

			if (sequence->One == 0 || sequence->Two == 0)
			{
				OutputMessage("BnfParser::ParseSequence - ERROR: Missing one or more operands, line %d column %d\n",Line(), Column());
				delete sequence;
				return 0;
			}

			sequence->One->Parent = sequence;
			sequence->Two->Parent = sequence;

			return sequence;

		}
	}

	return object;
}

BnfToken * BnfParser::ParseChoice()
{
	BnfToken *object = ParseGroup();

	if ( !Eof() && Is('|'))
	{
		Next();

		SkipWhitespace();

		BnfTokenChoice *choice = new BnfTokenChoice();
		choice->One = object;
		choice->Two = ParseBnf();

		if (choice->One == 0 || choice->Two == 0)
		{
			OutputMessage("BnfParser::ParseChoice - ERROR: Missing one or more operands, line %d column %d\n",Line(), Column());
			delete choice;
			return 0;
		}

		choice->One->Parent = choice;
		choice->Two->Parent = choice;

		return choice;
	}

	return object;
}

BnfToken * BnfParser::ParseGroup()
{

	BnfToken *object = ParseTerminal();

	if (object == 0 && !Is('('))
	{
		OutputTrace();
	}

	if ( !Eof() && object == 0 && Is('(') )
	{
		Next();

		SkipWhitespace();

		object = ParseBnf();

		if (object==0)
		{
			OutputMessage("BnfParser::ParseGroup - WARNING: Group is empty, line %d column %d\n",Line(), Column());
		}

		if ( !Eof() && Is(')') )
		{
			Next();

			BnfTokenGroup * group = new BnfTokenGroup();

			if (object != 0)
			{
				group->One = object;
				group->One->Parent = group;	
			}

			ParseOperator(group->Operator);

			return group;
		}
		else
		{
			OutputMessage("BnfParser::ParseGroup - ERROR: Expected \")\" at end of group, line %d column %d\n",Line(), Column());
		}
	}

	return object;
}

BnfToken * BnfParser::ParseTerminal()
{
	BnfToken *object = ParseLiteral();

	if (object == 0)
	{
		object = ParseLabel();
	}

	if (object != 0)
	{
		ParseOperator(((BnfTokenLabel*)object)->Operator);
	}

	return object;
}

BnfToken * BnfParser::ParseLiteral()
{
	if ( !Eof() && ParseString())
	{
		BnfTokenLiteral *literal = new BnfTokenLiteral();
		literal->Value.Construct(Token);
		return literal;
	}

	return 0;
}

BnfToken * BnfParser::ParseLabel()
{
	if ( !Eof() && (IsPunctuation() && !IsAny("?*+,|()")) || IsAlphanumeric() )
	{
		Mark();
		Next();
		while( (IsPunctuation() && !IsAny("?*+,|()")) || IsAlphanumeric())
		{
			Next();
			if (Eof())
				break;
		}
		Trap();

		BnfTokenLabel *label = new BnfTokenLabel();
		label->Value.Construct(Token);
		return label;
	}

	return 0;
}

void BnfParser::ParseOperator(BnfOperator &op)
{
	if (!Eof())
	{
		if (Is('?'))
		{
			Next();
			op.Assign(BnfOperator::ZERO_OR_ONE);
		}
		else
		if (Is('*'))
		{
			Next();
			op.Assign(BnfOperator::ZERO_OR_MORE);
		}
		else
		if (Is('+'))
		{
			Next();
			op.Assign(BnfOperator::ONE_OR_MORE);
		}

		SkipWhitespace();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BnfToken::BnfToken()
{

	Parent		= 0;
	Type		= NONE;
}

BnfToken::~BnfToken()
{

}

void * BnfToken::Accept(BnfVisitor * visitor)
{
	switch (Type)
	{
	case UNARY:
		{
			visitor->Visit(((BnfTokenUnary*)this)->One);
		}
		break;
	case BINARY:
		{
			visitor->Visit(((BnfTokenBinary*)this)->One);
			visitor->Visit(((BnfTokenBinary*)this)->Two);
		}
		break;
	default:
		visitor->Visit(this);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BnfTokenUnary::BnfTokenUnary()
{
	One=0;
}

BnfTokenUnary::~BnfTokenUnary()
{

	delete One;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BnfTokenBinary::BnfTokenBinary()
{
	One=0;
	Two=0;
}

BnfTokenBinary::~BnfTokenBinary()
{

	delete One;
	delete Two;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BnfOperator::BnfOperator()
{
	Assign(NOOP);	
}

BnfOperator::~BnfOperator()
{

}

void BnfOperator::Print()
{
	switch (*this)
	{		
	case BnfOperator::NOOP:
		OutputMessage(" ");
	break;
	case BnfOperator::ZERO_OR_ONE:
		OutputMessage("? ");
	break;
	case BnfOperator::ZERO_OR_MORE:
		OutputMessage("* ");
	break;
	case BnfOperator::ONE_OR_MORE:
		OutputMessage("+ ");
	break;
	};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BnfTokenChoice::BnfTokenChoice()
{

	Type=CHOICE;
	One=0;
	Two=0;
}

BnfTokenChoice::~BnfTokenChoice()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BnfTokenSequence::BnfTokenSequence()
{

	Type=SEQUENCE;
}

BnfTokenSequence::~BnfTokenSequence()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BnfTokenGroup::BnfTokenGroup()
{
	Type=GROUP;
}

BnfTokenGroup::~BnfTokenGroup()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BnfTokenLiteral::BnfTokenLiteral()
{
	Type=LITERAL;
}

BnfTokenLiteral::~BnfTokenLiteral()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BnfTokenLabel::BnfTokenLabel()
{
	Type=LABEL;
}

BnfTokenLabel::~BnfTokenLabel()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BnfEnumerator::BnfEnumerator()
{

	ObjectNext = 0;
	ObjectPrev = 0;
	Object = 0;
	Direction = 0;
}

BnfEnumerator::~BnfEnumerator()
{

}

void BnfEnumerator::Enumerate(BnfToken *object,bool expandGroups )
{

	ObjectNext = 0;
	ObjectPrev = 0;
	Object = object;
	Expansion = expandGroups;
	Direction=1;

}

bool BnfEnumerator::Forward()
{
	Direction = 1;

	if (Object == 0)
		return false;

	while (Object->Parent != 0)
		Object = Object->Parent;

	if (Object->Type.Is(BnfToken::UNARY) && ((BnfTokenUnary*)Object)->One != 0 && !Expansion )
	{
		Object = ((BnfTokenUnary*)Object)->One;
		if (Object == 0)
			return false;
	}

	if ( ! Object->Type.Is(BnfToken::TERMINAL) )
		return Move();

	return true;
}

bool BnfEnumerator::Reverse()
{
	Direction = -1;

	if (Object == 0)
		return false;

	while ((ObjectNext = Next()) != 0)
		Move();

	return true;
}

BnfToken * BnfEnumerator::Next()
{
	BnfToken *G = Object;

	if (G->Type.Is(BnfToken::TERMINAL))
	{
		if (G->Parent == 0)
		{

			OutputMessage("BnfEnumerator::Next - WARNING: Bnf contains only a single terminal.\n");
			return 0;
		}
		else
		if (G->Parent->Type.Is(BnfToken::BINARY) && G != ((BnfTokenBinary*)G->Parent)->Two)
		{
			G = ((BnfTokenBinary*)G->Parent)->Two;
		}
		else
		{
			while ( G->Parent != 0 && 
				(( G->Parent->Type.Is(BnfToken::BINARY) && G == ((BnfTokenBinary*)G->Parent)->Two )  ||
				(G->Parent->Type.Is(BnfToken::UNARY) && Expansion)))
			{
				G = G->Parent;
			}

			if (G->Parent == 0)
				return 0;	

			G = ((BnfTokenBinary*)G->Parent)->Two;
		}
	}

	while (! G->Type.Is(BnfToken::TERMINAL))
	{
		if (G->Type.Is(BnfToken::BINARY))
		{
			G = ((BnfTokenBinary*)G)->One;
		}
		else
		if (G->Type.Is(BnfToken::UNARY))
		{
			if (Expansion)
			{

				G = ((BnfTokenUnary*)G)->One;
			}
			else
			{

				return 0;
			}
		}
		else
		{
			OutputMessage("BnfEnumerator::Next - ERROR: Unrecognised object type.\n");
			return 0;
		}

		if (G == 0)
		{

			OutputMessage("BnfEnumerator::Next - WARNING: An object in the grammar is incomplete.\n");	
			return 0;
		}
	}

	return G;

}

BnfToken * BnfEnumerator::Prev()
{
	BnfToken *G = Object;

	if (G->Parent == 0)
		return 0;	

	if (G->Type.Is(BnfToken::TERMINAL))
	{
		if (G->Parent->Type.Is(BnfToken::BINARY) && G != ((BnfTokenBinary*)G->Parent)->One)
		{
			G = G->Parent;
		}
		else
		{		
			while ( (G->Parent->Type.Is(BnfToken::BINARY) && G == ((BnfTokenBinary*)G->Parent)->One) 
					|| (G->Parent->Type.Is(BnfToken::UNARY) && Expansion) )
			{
				G = G->Parent;

				if (G==0)
					return 0;	
			}
		}			
	}

	while (! G->Type.Is(BnfToken::TERMINAL))
	{
		if (G->Type.Is(BnfToken::BINARY))
		{
			G = ((BnfTokenChoice*)Object)->One;
		}
		else
		if (G->Type.Is(BnfToken::UNARY) && Expansion)
		{
			G = ((BnfTokenUnary*)G)->One;
		}
		else
		{
			OutputMessage("BnfEnumerator::Prev - ERROR: Unrecognised object type.");
			return 0;
		}
	}

	return G;
}

bool BnfEnumerator::Move()
{
	return Move(1);
}

bool BnfEnumerator::Move(int amount)
{
	amount *= Direction;
	if (amount > 0)
	{
		if (ObjectNext == 0)
		{

			Object = Next();
			return (Object != 0);
		}
		else
		{

			ObjectPrev	= Object;
			Object = ObjectNext;
			ObjectNext	= 0;
			return true;
		}
	}
	else
	if (amount < 0)
	{
		if (ObjectPrev == 0)
		{

			Object = Prev();
			return (Object == 0);
		}
		else
		{

			ObjectNext	= Object;
			Object = ObjectPrev;
			ObjectPrev	= 0;
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void * BnfVisitor::Visit(BnfToken * token) {return 0;}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool BnfMatcher::Match(Reason::Structure::Objects::List &vector) {return false;}
bool BnfMatcher::Match(Sequence &sequence) {return false;}
bool BnfMatcher::Match(XmlObject *object) {return false;}
bool BnfMatcher::Match(Reason::Structure::Objects::Enumerator &iterator) {return false;}

bool BnfMatcher::Contains(XmlObject *object) {return false;}

void BnfMatcher::Visit(BnfToken * token) {}

bool BnfMatcher::MatchGroup(Reason::Structure::Objects::Enumerator &iterator) {return false;}
bool BnfMatcher::MatchChoice(Reason::Structure::Objects::Enumerator &iterator) {return false;}
bool BnfMatcher::MatchSwitch(Reason::Structure::Objects::Entry::Enumerator & enumerator) {return false;}
bool BnfMatcher::MatchSequence(Reason::Structure::Objects::Enumerator &enumerator) {return false;}

bool BnfTokenLiteral::Match(Reason::Structure::Objects::Enumerator &iterator)
{
	if (Operator == 0)
	{
		if (iterator.Has() && iterator()->Compare(&Value)==0 )
		{
			iterator.Move();
			return true;
		}
	}
	else
	{
		if(Operator.Is(BnfOperator::ZERO_OR_ONE))
		{

			if (iterator.Has() && iterator()->Compare(&Value)==0)
				iterator.Move();

			return true;
		}
		else
		if(Operator.Is(BnfOperator::ZERO_OR_MORE))
		{

			while (iterator.Has() && iterator()->Compare(&Value)==0)
				iterator.Move();

			return true;
		}
		else
		if(Operator.Is(BnfOperator::ONE_OR_MORE))
		{

			if (iterator.Has() && iterator()->Compare(&Value)==0)
			{

				while (iterator.Move() && iterator.Has())
				{
					if (!iterator()->Compare(&Value)==0)
						break;
				}

				return true;
			}
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

