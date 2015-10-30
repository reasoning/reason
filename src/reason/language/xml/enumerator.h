
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
#ifndef LANGUAGE_XML_ENUMERATOR_H
#define LANGUAGE_XML_ENUMERATOR_H

#include "reason/language/xml/xml.h"
#include "reason/structure/objects/objects.h"

namespace Reason { namespace Language { namespace Xml {

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

class XmlMarkup;

class XmlNavigator : public Reason::Structure::Objects::Enumerator
{
private:

	XmlObject * NavPrev;
	XmlObject * NavNext;

	XmlObject * NavFirst;
	XmlObject * NavLast;
	XmlObject * Nav;	

	int NavDirection;

	XmlObject *Next();
	XmlObject *Prev();

	bool Found(XmlObject *object);

	int	NavDepth;			
	int	NavDepthMin;
	int	NavDepthMax;
	int	NavIndex;			

public:

	enum XmlEnumeratorModes
	{

		MODE_ALL		=0xFFFF,

		MODE_START		=(1)<<2,
		MODE_END		=(1)<<3,
		MODE_TEXT		=(1)<<4,
		MODE_COMMENT	=(1)<<5,
		MODE_DTD		=(1)<<6,	
		MODE_ANCESTOR	=(1)<<7,
		MODE_DESCENDANT	=(1)<<8,
		MODE_PRECEDING	=(1)<<9,
		MODE_FOLLOWING	=(1)<<10,

		MODE_NAMED		=(1)<<11,

		MODE_UNRESTRICTED			=(1)<<12,
	};

	Bitmask16		Mode;

	XmlNavigator();
	~XmlNavigator();

	bool IsRestricted()	{return !Mode.Is(MODE_UNRESTRICTED);};
	void Restrict(int min, int max)
	{
		if (min <= 0 && max <= 0)
		{
			Mode.On(MODE_UNRESTRICTED);
			NavDepthMin = -1;
			NavDepthMax = -1;
		}
		else
		{
			Mode.Off(MODE_UNRESTRICTED);
			NavDepthMin = min;
			NavDepthMax = max;		
		}
	}

	int Depth()			{return NavDepth;};
	int Index()			{return NavIndex;};

	void First(XmlObject * object) {NavFirst = object;}
	void Last(XmlObject * object) {NavLast = object;}

	void Navigate(XmlMarkup *markup, XmlObject *context);
	void Navigate(XmlObject *object);

	void NavigateFollowing(XmlMarkup *markup, XmlObject *context);
	void NavigateFollowing(XmlObject *object);

	void NavigatePreceding(XmlMarkup *markup, XmlObject *context);
	void NavigatePreceding(XmlObject *object);

	void NavigateSiblings(XmlMarkup *markup, XmlObject *context);
	void NavigateSiblings(XmlObject *object);

	void NavigateDescendants(XmlObject *object, int depthMax=-1);

	void NavigateAncestors(XmlObject *object, int depthMin=-1);

	void NavigateChildren(XmlObject *object)
	{
		NavigateDescendants(object,0);
	}

	bool Has(){return Nav!=0;};

	bool Move();
	bool Move(int amount);
	bool Reverse();
	bool Forward();

	inline XmlObject & Reference() {return *Nav;};
	inline XmlObject * Pointer() {return Nav;};
	inline XmlObject * operator()(void)	{return Nav;};

	void Release();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

