
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
#ifndef SYSTEM_PARAMETER_H
#define SYSTEM_PARAMETER_H

#include "reason/system/object.h"
#include "reason/system/sequence.h"
#include "reason/system/number.h"
#include "reason/system/time.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::Structure;
using namespace Reason::System;

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Parameter
{
public:

	enum ParameterType
	{
		PARAMEER_NULL,
		PARAMETER_CHAR,
		PARAMETER_SHORT,
		PARAMETER_INT,
		PARAMETER_LONG,
		PARAMETER_FLOAT,
		PARAMETER_DOUBLE,
		PARAMETER_VOID,

		PARAMETER_LITERAL,
		PARAMETER_NUMBER,
		PARAMETER_STRING,
		PARAMETER_OBJECT,
		PARAMETER_TIME,

	};

	union ParameterUnion
	{
		char Char;
		short Short;
		int Int;
		long long Long;
		float Float;
		double Double;
		void * Void;

		Reason::System::Object * Object;

		union ParameterUnionString
		{
			char * Data;
			int Size;
		};

		ParameterUnionString String;

		union ParameterUnionNumber
		{
			long long Long;
			int Int;
			float Float;
			double Double;
		};

		ParameterUnionNumber Number;

		union ParameterUnionTime
		{
			long long Seconds;
			short Milliseconds;
		};

		ParameterUnionTime Time;

	};

	ParameterUnion Kind;
	int Type;
};

#define Parameters const Parameter paramOne, const Parameter paramTwo, const Parameter paramThree, const Parameter paramFour, const Parameter paramFive, \
const Parameter paramSix, const Parameter paramSeven, const Parameter paramEight, const Parameter paramNine, const Parameter paramTen

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
