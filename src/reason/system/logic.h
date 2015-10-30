
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
#ifndef SYSTEM_LOGIC_H
#define SYSTEM_LOGIC_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/string.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Logic
{

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Probability
{
public:

	double	Threshold;
	double	Percentage;

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Approximate
{
public:

	bool	Logic;		
	float	Scale;		
	float	Weight;	
	float	Deviation;	

	Approximate()
	{
		Initialise();
	};

	void Initialise()
	{
		Logic	= false;	
		Weight = 0;
		Scale	= 5;		
		Deviation = 0;		
	};

	void Initialise(bool logic, float weight=0, int scale=5, float deviation=0)
	{
		Logic = logic;
		Weight = weight;
		Scale = (float) scale;		
		Deviation = deviation;

	};

	Approximate(bool logic, float weight=0, int scale=5, float deviation=0)
	{	
		Initialise(logic,weight,scale,deviation);
	};

	inline float ScaledWeight() {	return Weight * Scale; };

	void AddWeight(int percentage)
	{
		if (percentage == 75)
			Weight += (Scale/4)*3;
		else
		if (percentage == 50)
			Weight += Scale/2;
		else
		if (percentage == 25)
			Weight += Scale/4;
		else
		if (percentage == 20)
			Weight += Scale/5;
		else
		if (percentage == 10)
			Weight += Scale/10;
		else
		if (percentage == 5)
			Weight += Scale/20;
		else
			Weight += (Scale/100)*percentage;
	}

	float Required(int percentage)	
	{
		if (percentage == 75)
			return (Scale/4)*3;
		else
		if (percentage == 50)
			return Scale/2;
		else
		if (percentage == 25)
			return Scale/4;
		else
		if (percentage == 20)
			return Scale/5;
		else
		if (percentage == 10)
			return Scale/10;
		else
		if (percentage == 5)
			return Scale/20;
		else
			return (Scale/100)*percentage;
	}

	float Outcome()
	{
		return (Weight/Scale)*100;	
	}

	bool operator > (Approximate &obj)
	{
		if (Logic == obj.Logic)
		{

			if(Logic)
			{

				if ( ScaledWeight() > obj.ScaledWeight() )
					return true;
				else
					return false;

			}
			else
			{

				if ( ScaledWeight() < obj.ScaledWeight() )
					return true;
				else
					return false;

			}
		}
		else
		{

			if (Logic)
				return true;
			else
				return false;
		}
	};

	bool operator < (Approximate &obj)
	{

		if (Logic == obj.Logic)
		{

			if(Logic)
			{

				if ( ScaledWeight() < obj.ScaledWeight() )
					return true;
				else
					return false;

			}
			else
			{

				if ( ScaledWeight() > obj.ScaledWeight() )
					return true;
				else
					return false;

			}
		}
		else
		{

			if (! Logic)
				return true;
			else
				return false;
		}
	};

	bool operator == (Approximate &obj)
	{
		if (Logic == obj.Logic && ScaledWeight() == obj.ScaledWeight())
			return true;
		else
			return false;
	};

	bool operator != (Approximate &obj)
	{
		if (Logic == obj.Logic && ScaledWeight() == obj.ScaledWeight())
			return false;
		else
			return true;
	};

};

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

}}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

