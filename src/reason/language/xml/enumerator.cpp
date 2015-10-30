
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


#include "reason/language/xml/enumerator.h"
#include "reason/language/xml/markup.h"
#include "reason/language/xml/xml.h"

using namespace Reason::Language::Xml;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

XmlNavigator::XmlNavigator():
Nav(0),NavNext(0),NavPrev(0),NavFirst(0),NavLast(0),NavDirection(0),
NavDepth(0),NavDepthMin(0),NavDepthMax(0),NavIndex(0),Mode(0)
{

}

XmlNavigator::~XmlNavigator()
{

}

void XmlNavigator::Release()
{
	Nav			= 0;
	NavNext		= 0;
	NavPrev		= 0;
	NavFirst	= 0;
	NavLast		= 0;
	NavDepth				= 0;
	NavDepthMin			= 0;
	NavDepthMax			= 0;
	NavIndex				= 0;
	Mode				= 0;
}

bool XmlNavigator::Found(XmlObject *object)
{

	if (object==0 || Mode == 0)
		return true;

	if (NavLast)
	{
		if (object == NavLast && !Mode.Equals(MODE_ALL) && !Mode.Is(MODE_DESCENDANT) && (Mode.Is(MODE_ANCESTOR) || Mode.Is(MODE_PRECEDING)))
			return false;
	}

	if (NavFirst)
	{
		if (object == NavFirst && !Mode.Equals(MODE_ALL) && !Mode.Is(MODE_DESCENDANT) && Mode.Is(MODE_FOLLOWING))
			return false;

		if (!Mode.Is(MODE_DESCENDANT) && object->IsDescendantOf(NavFirst))
			return false;
	}

	switch (object->Type)
	{
	case XmlObject::XML_START:
		{
			if (Mode.Is(MODE_START))
			{
				if (!Mode.Is(MODE_ANCESTOR))
					return object != NavLast && !object->IsAncestorOf(NavLast);

				return true;
			}
			else
			{
				if (Mode.Is(MODE_ANCESTOR))
					return object != NavLast && object->IsAncestorOf(NavLast);

				return false;
			}
		}
		break;
	case XmlObject::XML_EMPTY: return true;
		break;
	case XmlObject::XML_END: return Mode.Is(MODE_END);
		break;
	case XmlObject::XML_COMMENT: return Mode.Is(MODE_COMMENT);
		break;

	case XmlObject::XML_CDATA:	
	case XmlObject::XML_TEXT: return Mode.Is(MODE_TEXT);
		break;
	case XmlObject::XML_MARKUP: return false;
		break;
	default:
		if (object->Type.Is(XmlObject::XML_DTD_ELEMENT))
			return Mode.Is(MODE_DTD);
		break;
	}

	return true;

}

XmlObject *XmlNavigator::Next()
{
	XmlObject *object = Nav;
	while (object != 0)
	{
		if (object == NavLast)
			return 0;

		if (object->Type.Is(XmlObject::XML_START) && object->Child != 0 && (!IsRestricted() || NavDepth < NavDepthMax) )
		{
			++NavDepth;
			++NavIndex;
			object = object->Child;
		}
		else
		if (object->After != 0)
		{
			++NavIndex;
			object = object->After;
		}
		else
		if ( object->Parent !=0  && (!IsRestricted() || NavDepth> NavDepthMin) )
		{
#ifdef _DEBUG

			object = object->Parent;
			while (object && !object->After && (!IsRestricted() || NavDepth> NavDepthMin))
			{
				--NavDepth;
				object = object->Parent;
			}

			if (object)
			{
				--NavDepth;
				++NavIndex;
				object = object->After;
			}

#else

			--NavDepth;
			++NavIndex;
			object = object->Parent->After;

#endif
		}
		else
		{

			object = 0;
		}

		if (Found(object))
			break;
	}

	return object;

}

XmlObject *XmlNavigator::Prev()
{
	XmlObject * object = Nav;	
	while (object != 0)
	{
		if (object == NavFirst)
			return 0;

		if ( object->Type.Is(XmlObject::XML_END) && object->Child != 0  && (!IsRestricted() || NavDepth < NavDepthMax) )
		{

			++NavDepth;
			--NavIndex;
			object = object->Child;
		}
		else
		if (object->Before != 0)
		{
			--NavIndex;
			object = object->Before;
		}
		else
		if (object->Parent !=0   && (!IsRestricted() || NavDepth > NavDepthMin) )
		{
			--NavDepth;
			--NavIndex;
			object = object->Parent;
		}
		else
		{
			object = 0;
		}

		if (Found(object))
			break;
	}

	return object;
}

bool XmlNavigator::Move()
{
	return Move(1);
}

bool XmlNavigator::Move(int amount)
{
	amount *= NavDirection;
	if (amount > 0)
	{
		if (NavNext == 0)
		{

			Nav = Next();
			return (Nav != 0);
		}
		else
		{

			NavPrev	= Nav;
			Nav = NavNext;
			NavNext	= 0;
			return true;
		}
	}
	else
	{
		if (NavPrev == 0)
		{

			Nav = Prev();
			return (Nav != 0);
		}
		else
		{

			NavNext	= Nav;
			Nav = NavPrev;
			NavPrev	= 0;
			return true;
		}
	}
}

bool XmlNavigator::Reverse()
{
	NavDirection = -1;

	if (NavLast != 0)
	{
		NavIndex = 0;
		NavDepth = 0;

		if ( !Found((Nav = NavLast)) )
			Move();
	}
	else
	{
		NavNext = Next();
		if (!NavNext && NavFirst)
			Forward();

		while ((NavNext = Next()) != 0)
			Move(-1);
	}

	return Nav != 0;
}

bool XmlNavigator::Forward()
{
	NavDirection = 1;

	if (NavFirst != 0)
	{
		NavIndex = 0;
		NavDepth = 0;

		if ( !Found((Nav = NavFirst)) )
			Move();
	}
	else
	{
		NavPrev = Prev();
		if (!NavPrev && NavLast)
			Reverse();

		while ((NavPrev = Prev()) != 0)
			Move(-1);
	}

	return Nav != 0;
}

void XmlNavigator::NavigateAncestors(XmlObject *object, int depthMin)
{

	Mode.Off(MODE_DESCENDANT);
	Mode.On(MODE_ANCESTOR);

	if (depthMin > -1)
	{
		NavDepthMin=depthMin;	
		Mode.Off(MODE_UNRESTRICTED);
	}
	else
	{
		Mode.On(MODE_UNRESTRICTED);
	}

	NavLast = object;

	Reverse();
}

void XmlNavigator::NavigateDescendants(XmlObject *object, int depthMax)
{

	if (Mode.Is(0))
		Mode.On(MODE_ALL);

	Mode.On(MODE_DESCENDANT);

	if (depthMax > -1)
	{
		NavDepthMax=depthMax;	
		Mode.Off(MODE_UNRESTRICTED);
	}
	else
	{
		Mode.On(MODE_UNRESTRICTED);
	}

	if (object->Type.Is(XmlObject::XML_MARKUP))
	{
		NavFirst = ((XmlMarkup *)object)->First;
		NavLast	= ((XmlMarkup *)object)->Last;
		Nav = NavFirst;
	}
	else
	if (object->Type.Is(XmlObject::XML_START) && object->Child != 0)
	{
		NavFirst = object->Child;
		NavLast = object->LastChild();
		Nav = NavFirst;
	}
	else
	if (object->Type.Is(XmlObject::XML_END) && object->Child != 0)
	{
		NavFirst = object->FirstChild();
		NavLast = object->Child;
		Nav = NavFirst;
	}

	Forward();
}

void XmlNavigator::NavigateSiblings(XmlObject *object)
{
	Mode.Off(MODE_UNRESTRICTED);
	NavDepthMax = 0;
	NavDepthMin = 0;

	NavFirst = object->FirstSibling();
	NavLast = object->LastSibling();
	Nav = object;

	Forward();
}

void XmlNavigator::NavigateFollowing(XmlMarkup *markup, XmlObject *context)
{

	NavLast = markup->Last;
	NavigateFollowing(context);

	Forward();
}

void XmlNavigator::NavigateFollowing(XmlObject *object)
{

	if (Mode.Is(0))
		Mode.On(MODE_ALL);

	Mode.Off(MODE_DESCENDANT);
	Mode.Off(MODE_END);

	Mode.On(MODE_FOLLOWING);

	NavFirst = object;
	Forward();

}

void XmlNavigator::NavigatePreceding(XmlMarkup *markup, XmlObject *context)
{

	NavFirst = markup->First;
	NavigatePreceding(context);

	Reverse();
}

void XmlNavigator::NavigatePreceding(XmlObject *object)
{

	if (Mode.Is(0))
		Mode.On(MODE_ALL);

	Mode.Off(MODE_DESCENDANT);
	Mode.Off(MODE_ANCESTOR);
	Mode.Off(MODE_END);

	Mode.On(MODE_PRECEDING);

	NavLast = object;

	Reverse();
}

void XmlNavigator::Navigate(XmlMarkup *markup, XmlObject *context)
{
	NavFirst = markup->First;
	NavLast	= markup->Last;
	Navigate(context);
}

void XmlNavigator::Navigate(XmlObject *object)
{

	if (Mode.Is(0))
		Mode.On(MODE_ALL);

	if (object->Type.Is(XmlObject::XML_MARKUP))
	{
		NavFirst = ((XmlMarkup *)object)->First;
		NavLast	= ((XmlMarkup *)object)->Last;
		Nav = NavFirst;
	}
	else
	{
		Nav = object;
	}

	Forward();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

