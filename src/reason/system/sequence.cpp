
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

#include "reason/system/sequence.h"
#include "reason/system/character.h"
#include "reason/system/string.h"
#include "reason/system/type.h"
#include "reason/system/indices.h"
#include "reason/language/regex/regex.h"
#include "reason/system/output.h"
#include "reason/system/number.h"

#include <string.h>
#include <ctype.h>

#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace System {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Sequences::Compare(const char * left, const char * right)
{

	if (left == right) return 0;
	return strcmp(left,right);	

	while (*(int*)left == *(int*)right)
	{
		long mask = *(int*)left;
		if ( (mask&0xFF000000)==0 || (mask&0x00FF0000)==0 || (mask&0x0000FF00)==0 || (mask&0x000000FF)==0 ) return 0;

		left+=4;
		right+=4;
	}

	while(*left == *right)
	{
		if (*left==0)return 0;

		++left;
		++right;
	}

	return *left-*right;

}

int Sequences::Compare(const char * left, const char * right, const int size)
{
	int result;
	for (int index = (size>>2);index>0;--index)
	{
		if (*(int*)left != *(int*)right)
		{
			result = (*left++)-(*right++);
			if (result) return result;

			result = (*left++)-(*right++);
			if (result) return result;

			result = (*left++)-(*right++);
			if (result) return result;

			result = (*left++)-(*right++);
			if (result) return result;
		}
		else
		{
			left+=4;
			right+=4;
		}
	}

	for (int index = (size&0x00000003);index>0;--index)
	{
		result = (*left++)-(*right++);
		if (result) return result;
	}

	return 0;
}

int Sequences::Compare(const char * left, const int leftSize, const char * right, const int rightSize)
{
	int result = Compare(left,right,(leftSize<rightSize)?leftSize:rightSize);
	return (result)?result:leftSize-rightSize;
}

int Sequences::CompareCaseless(const char * left, const char * right)
{

	int result;
	while (*left != 0 || *right != 0)
	{
		if (*(int*)left == *(int*)right)
		{
			int mask = *(int*)left;
			if ( (mask&0x00FF0000)==0 || (mask&0x0000FF00)==0 || (mask&0x000000FF)==0 ) return 0;

			left+=4;
			right+=4;
		}
		else
		if (*left == *right)
		{
			left++;
			right++;
		}
		else
		{

			result = Characters::Caseless(*left++)-Characters::Caseless(*right++);
			if (result) return result;
		}
	}

	return 0;
}

int Sequences::CompareCaseless(const char * left, const char * right, const int size)
{
	int result;
	for (int index = (size>>2);index>0;--index)
	{
		if (*(int*)left != *(int*)right)
		{
			result = Characters::Caseless(*left++)-Characters::Caseless(*right++);
			if (result) return result;

			result = Characters::Caseless(*left++)-Characters::Caseless(*right++);
			if (result) return result;

			result = Characters::Caseless(*left++)-Characters::Caseless(*right++);
			if (result) return result;

			result = Characters::Caseless(*left++)-Characters::Caseless(*right++);
			if (result) return result;
		}
		else
		{
			left+=4;
			right+=4;
		}
	}

	for (int index = (size&0x00000003);index>0;--index)
	{
		result = Characters::Caseless(*left++)-Characters::Caseless(*right++);
		if (result) return result;
	}

	return 0;
}

int Sequences::CompareCaseless(const char * left, const int leftSize, const char * right, const int rightSize)
{
	int result = CompareCaseless(left,right,(leftSize<rightSize)?leftSize:rightSize);
	return (result)?result:leftSize-rightSize;
}

int Sequences::CompareReverse(const char * left, const char * right)
{
	int result;
	int leftSize = strlen(left);
	int rightSize = strlen(right);
	result = CompareReverse(left,right,(leftSize<rightSize)?leftSize:rightSize);
	return (result)?result:leftSize-rightSize;
}

int Sequences::CompareReverse(const char * left, const char * right, const int size)
{
	int result;
	left+=size;
	right+=size;
	for (int index = (size&0x00000003);index>0;--index)
	{
		result = (*--left)-(*--right);
		if (result) return result;
	}

	for (int index = (size>>2);index>0;--index)
	{
		if (*(int*)(left-=4) != *(int*)(right-=4))
		{
			result = (left[3])-(right[3]);
			if (result) return result;

			result = (left[2])-(right[2]);
			if (result) return result;

			result = (left[1])-(right[1]);
			if (result) return result;

			result = (left[0])-(right[0]);
			if (result) return result;
		}
	}

	return 0;
}

int Sequences::CompareReverse(const char * left, const int leftSize, const char * right, const int rightSize)
{
	int result = CompareReverse(left,right,(leftSize<rightSize)?leftSize:rightSize);
	return (result)?result:leftSize-rightSize;
}

int Sequences::CompareReverseCaseless(const char * left, const char * right)
{
	int result;
	int leftSize = strlen(left);
	int rightSize = strlen(right);
	result = CompareReverseCaseless(left,right,(leftSize<rightSize)?leftSize:rightSize);
	return (result)?result:leftSize-rightSize;
}

int Sequences::CompareReverseCaseless(const char * left, const char * right, const int size)
{
	int result;
	left+=size;
	right+=size;

	for (int index = (size&0x00000003);index>0;--index)
	{
		result = Characters::Caseless(*--left)-Characters::Caseless(*--right);
		if (result) return result;
	}

	for (int index = (size>>2);index>0;--index)
	{
		if (*(int*)(left-=4) != *(int*)(right-=4))
		{
			result = Characters::Caseless(left[3])-Characters::Caseless(right[3]);
			if (result) return result;

			result = Characters::Caseless(left[2])-Characters::Caseless(right[2]);
			if (result) return result;

			result = Characters::Caseless(left[1])-Characters::Caseless(right[1]);
			if (result) return result;

			result = Characters::Caseless(left[0])-Characters::Caseless(right[0]);
			if (result) return result;
		}
	}

	return 0;

}

int Sequences::CompareReverseCaseless(const char * left, const int leftSize, const char * right, const int rightSize)
{
	int result = CompareReverseCaseless(left,right,(leftSize<rightSize)?leftSize:rightSize);
	return (result)?result:leftSize-rightSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Sequences::Equals(const char * left, const char * right)
{
	if (left == right) return true;
	return strcmp(left,right)==0;

}

bool Sequences::Equals(const char * left, const char * right, const int size)
{

	OutputAssert(size >= 0);

	if ((left == 0 || right == 0) && left != right) return false;	
	if (left == right || size == 0) return true;	
	if (left[size-1] != right[size-1] || left[0] != right[0]) return false;

	if (size > 2)
	{
		for (int index = (size>>2);index>0;--index)
		{
			if (*(int*)left != *(int*)right) return false;

			left+=4;
			right+=4;
		}

		for (int index = (size&0x00000003);index>0;--index)
		{
			if ((*left++)!=(*right++)) return false;
		}
	}

	return true;
}

bool Sequences::Equals(const char * left, const int leftSize, const char * right, const int rightSize)
{
	if (leftSize != rightSize) return false;
	if (left == right) return true;
	return Equals(left,right,rightSize);
}

bool Sequences::EqualsCaseless(const char * left, const char * right)
{
	if (left == right) return true;
	return Sequences::CompareCaseless(left,right)==0;
}

bool Sequences::EqualsCaseless(const char * left, const char * right, const int size)
{

	OutputAssert(size >= 0);

	if ((left == 0 || right == 0) && left != right) return false;
	if (left == right || size == 0) return true;
	if (Characters::Caseless(left[size-1]) != Characters::Caseless(right[size-1])) return false;

	for (int index=(size>>2);index>0;--index)
	{
		if (*(int*)left != *(int*)right)
		{
			if (Characters::Caseless(*left++)!=Characters::Caseless(*right++)) return false;
			if (Characters::Caseless(*left++)!=Characters::Caseless(*right++)) return false;
			if (Characters::Caseless(*left++)!=Characters::Caseless(*right++)) return false;
			if (Characters::Caseless(*left++)!=Characters::Caseless(*right++)) return false;
		}
		else
		{
			left+=4;
			right+=4;
		}
	}

	for (int index=(size&0x00000003);index>0;--index)
	{
		if (Characters::Caseless(*left++) != Characters::Caseless(*right++)) return false;
	}

	return true;
}

bool Sequences::EqualsCaseless(const char * left, const int leftSize, const char * right, const int rightSize)
{
	if (leftSize != rightSize) return false;
	if (left == right) return true;
	return EqualsCaseless(left,right,rightSize);
}

bool Sequences::EqualsReverse(const char * left, const char * right)
{
	if (left == right) return true;
	int leftSize = strlen(left);
	int rightSize = strlen(right);
	if (leftSize != rightSize) return false;
	return EqualsReverse(left,right,rightSize);
}

bool Sequences::EqualsReverse(const char * left, const char * right, const int size)
{

	OutputAssert(size >= 0);

	if ((left == 0 || right == 0) && left != right) return false;
	if (left == right || size == 0) return true;
	if (left[0] != right[0]) return false;

	left+=size;
	right+=size;

	for (int index = (size&0x00000003);index>0;--index)
	{
		if ((*--left) != (*--right)) return false;
	}

	for (int index = (size>>2);index>0;--index)
	{
		if (*(int*)(left-=4) != *(int*)(right-=4))
			return false;
	}

	return true;
}

bool Sequences::EqualsReverse(const char * left, const int leftSize, const char * right, const int rightSize)
{
	if (leftSize != rightSize) return false;
	if (left == right) return true;
	return EqualsReverse(left,right,rightSize);
}

bool Sequences::EqualsReverseCaseless(const char * left, const char * right)
{
	if (left == right) return true;
	return Sequences::CompareReverseCaseless(left,right)==0;
}

bool Sequences::EqualsReverseCaseless(const char * left, const char * right, const int size)
{
	if (left == right) return true;
	if (Characters::Caseless(left[0]) != Characters::Caseless(right[0])) return false;

	left+=size;
	right+=size;

	for (int index = (size&0x00000003);index>0;--index)
	{
		if (Characters::Caseless(*--left) != Characters::Caseless(*--right)) return false;
	}

	for (int index = (size>>2);index>0;--index)
	{
		if (*(int*)(left-=4) != *(int*)(right-=4))
		{
			if (Characters::Caseless(left[3])!=Characters::Caseless(right[3])) return false;
			if (Characters::Caseless(left[2])!=Characters::Caseless(right[2])) return false;
			if (Characters::Caseless(left[1])!=Characters::Caseless(right[1])) return false;
			if (Characters::Caseless(left[0])!=Characters::Caseless(right[0])) return false;
		}
	}

	return true;
}

bool Sequences::EqualsReverseCaseless(const char * left, const int leftSize, const char * right, const int rightSize)
{
	if (leftSize != rightSize) return false;
	if (left == right) return true;
	return EqualsReverseCaseless(left,right,rightSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Sequences::Length(const char * data)
{

	return (data)?strlen(data):0;
}

int Sequences::Hash(char *data, int size)
{
	if ( data==0 || !(size>0) ) return 0;

	unsigned int key = 5381;
	for (char * c = (char*)data+size-1;c >= data;)
		key = ((key << 5) + key) + *c--; 

	return key;

}

int Sequences::HashCaseless(char *data, int size)
{
	if ( data==0 || !(size>0) ) return 0;

	unsigned int key = 5381;
	for (char * c = (char*)data+size-1;c >= data;)
		key = ((key << 5) + key) + Character::Caseless(*c--); 

	return key;

}

bool Sequences::IsUppercase(char *data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (int i=0;i<size;++i)
	{
		if (! isupper(data[i]))
			return false;
	}

	return true;
}

bool Sequences::IsLowercase(char *data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (int i=0;i<size;++i)
	{
		if (!isupper(data[i]))
			return false;
	}

	return true;
}

bool Sequences::IsProppercase(char *data, int size)
{
	if ( data==0 || !(size>0) || !isupper(*data)) return false;

	for (int i=1;i<size;++i)
	{
		if (!islower(data[i]))
			return false;
	}

	return true;
}

bool Sequences::IsAlpha(char *data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (int i=0;i<size;++i)
	{

		if ((data[i]|0x20) < 'a' || (data[i]|0x20) > 'z')
			return false;
	}

	return true;
}

bool Sequences::IsNumeric(char *data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (int i=0;i<size;++i)
	{
		if (!isdigit(data[i]))
			return false;
	}

	return true;
}

bool Sequences::IsAlphanumeric(char *data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (int i=0;i<size;++i)
	{
		if (!isalnum(data[i]))
			return false;
	}

	return true;
}

bool Sequences::IsBinary(char * data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (char * eof=data+size;data != eof;++data)
		if (*data != '1' && *data != '0')
			return false;

	return true;
}

long long Sequences::Binary(char * data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	long long result = 0;
	long long sign=1;

	if (*data == '-')
	{
		++data;--size;
		sign=-1;
	}
	else
	if (*data == '+')
	{
		++data;--size;
	}

	--size;
	while (size > -1)
	{
		result <<= 1;
		if (*data == '1')
			result |= 1;
		else
		if (*data == '0')
			result |= 0;
		else
			break;

		++data;--size;
	}

	return result * sign;
}

bool Sequences::IsOctal(char *data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (char * eof=data+size;data != eof;++data)
		if (!(*data >= '0' && *data <= '7'))
			return false;

	return true;
}

long long Sequences::Octal(char *data,int size)
{
	if ( data==0 || !(size>0) ) return 0;

	long long result = 0;
	long long sign=1;

	if (*data == '-')
	{
		++data;--size;
		sign=-1;
	}
	else
	if (*data == '+')
	{
		++data;--size;
	}

	if (size > 1 && data[0] == '0' && (data[1]|0x20) == 'x')
	{
		data += 2;size -= 2;
	}
	else	
	if (size > 0 && data[0] == '0')
	{
		++data;--size;
	}

	--size;

	while (size > -1)
	{
		if (Reason::System::Characters::IsOctal(*data))
		{

			result = result*8 + (*data-0x30);

		}
		else
		{
			break;
		}

		++data;--size;
	}

	return result * sign;
}

bool Sequences::IsHex(char *data, int size)
{
	if ( data==0 || !(size>0) ) return false;

	for (char * eof=data+size;data != eof;++data)
		if (!(isdigit(*data) ||
			(*data == 'A') || (*data == 'B') || (*data == 'C') || (*data == 'D') || (*data == 'E') || (*data == 'F') ||
			(*data == 'a') || (*data == 'b') || (*data == 'c') || (*data == 'd') || (*data == 'e') || (*data == 'f')
			)) return false;

	return true;
}

long long Sequences::Hex(char *data,int size)
{
	if ( data==0 || !(size>0) ) return 0;

	long long result = 0;
	long long sign=1;

	if (*data == '-')
	{
		++data;--size;
		sign=-1;
	}
	else
	if (*data == '+')
	{
		++data;--size;
	}	

	if (size > 1 && data[0] == '0' && (data[1]|0x20) == 'x')
	{
		data += 2;size -= 2;
	}
	else
	if (size > 0 && data[0] == '#')
	{
		++data;--size;
	}

	--size;

	while (size > -1)
	{
		if (isdigit(*data))
		{

			result = result*16 + (*data-0x30);

		}
		else
		if (Reason::System::Characters::IsHex(*data))
		{

			result = result*16 + ((*data|0x20)-0x57);

		}
		else
		{
			break;
		}

		++data;--size;
	}

	return result * sign;
}

long long Sequences::Decimal(char *data,int size)
{
	if ( data==0 || !(size>0) ) return 0;

	long long result=0;
	long long sign=1;

	if (*data == '-')
	{
		++data;--size;
		sign=-1;
	}
	else
	if (*data == '+')
	{
		++data;--size;
	}

	const char * eof = data+size;

	while(data+4 <= eof)
	{	

		if ( (*data&0xF0) != 0x30 || (*data&0x0F) > 9 )	

		break;

		result = result*10 + (*(data++)-0x30);

		if ( (*data&0xF0) != 0x30 || (*data&0x0F) > 9 )	

		break;

		result = result*10 + (*(data++)-0x30);

		if ( (*data&0xF0) != 0x30 || (*data&0x0F) > 9 )	

		break;

		result = result*10 + (*(data++)-0x30);

		if ( (*data&0xF0) != 0x30 || (*data&0x0F) > 9 )	

		break;

		result = result*10 + (*(data++)-0x30);
	}

	while(data != eof)
	{	

		if ( (*data&0xF0) != 0x30 || (*data&0x0F) > 9 )	

		break;

		result = result*10 + (*(data++)-0x30);
	}

	return sign * result;

}

long long Sequences::Integer(int radix, char * data, int size)
{

	long long result=0;

	switch(radix)
	{
	case 0: result = Sequences::Integer(data,size);break; 
	case 2: result = Sequences::Binary(data,size);break;
	case 8: result = Sequences::Octal(data,size);break;
	case 10: result = Sequences::Decimal(data,size);break;
	case 16: result = Sequences::Hex(data,size);break;
	}

	return result;
}

long long Sequences::Integer(char *data,int size)
{

	if ( data==0 || !(size>0) ) return 0;

	int radix = 0;
	long long result=0;

	long long sign=1;
	if (*data == '-')
	{
		++data;--size;
		sign=-1;
	}
	else
	if (*data == '+')
	{
		++data;--size;
	}

	if (size > 0 && *data >= '1' && *data <= '9')
	{
		radix = 10;
	}
	else
	if (size > 0 && *data == '0')
	{
		++data;--size;
		if (size >0)
		{
			if (*data >= '0' && *data <= '7')
			{
				radix = 8;
			}
			else
			if (*data == 'x' || *data == 'X')
			{
				++data;--size;
				radix = 16;
			}
		}
	}

	switch (radix)
	{
	case 8: result = Sequences::Octal(data,size);break;
	case 10: result = Sequences::Decimal(data,size);break;
	case 16: result = Sequences::Hex(data,size);break;
	}

	return result * sign;

}

double Sequences::Real(char * data,int size)
{
	if ( data==0 || !(size>0) ) return Number::Nan();

	double result = 0.0;

	char * integer		=0;
	int	   integerSize	=0;
	double integerSign	=1;	
	char * fraction		=0;
	int	   fractionSize	=0;
	char * exponent		=0;
	int    exponentSize =0;
	double exponentSign	=1;

	int i=0;

	if (data[i] == '+')
	{
		++i;
	}
	else
	if (data[i] == '-')
	{
		++i;
		integerSign = -1;
	}

	if (i < size && Character::IsNumeric(data[i]))
	{
		integer = (char*) &data[i];
		while (i < size)
		{
			if (Character::IsNumeric(data[i]))
			{
				++integerSize;
			}
			else

			{
				break;
			}

			++i;
		}
	}

	if (i < size && data[i] == '.')
	{
		++i;

		if (i < size && Character::IsNumeric(data[i]))
		{

			fraction = (char*) &data[i];
			while (i < size)
			{
				if (Character::IsNumeric(data[i]))
				{
					++fractionSize;
				}
				else
				{
					break;
				}

				++i;
			}
		}

	}

	if (i < size && (integerSize > 0 || fractionSize > 0) && (data[i]|0x20) == 'e')
	{

		++i;

		if (data[i] == '+')
		{
			++i;
		}
		else
		if (data[i] == '-')
		{
			++i;
			exponentSign = -1;
		}

		if (i < size && Character::IsNumeric(data[i]))
		{
			exponent = (char*) &data[i];
			while (i < size)
			{
				if (Character::IsNumeric(data[i]))
				{
					++exponentSize;
				}
				else
				{
					break;
				}

				++i;
			}
		}

	}

	if (integerSize == 0 && fractionSize == 0 )
		return Number::Nan();

	result = Reason::System::Sequences::Decimal(integer,integerSize);

	if (fractionSize > 0)
	{

		long long fractionValue = Reason::System::Sequences::Decimal(fraction,fractionSize);
		result += fractionValue * (double)pow((double)10,-fractionSize);
	}

	result *= integerSign;

	if (exponentSize > 0)
	{
		result = result * (double)pow((double)10,Sequences::Decimal(exponent,exponentSize) * exponentSign);
	}

	return result;
}

void Sequences::Lowercase(char *data, int size)
{
	if ( data==0 || !(size>0) ) return;

	{
		char * p = (char*)data;
		char * s = (char*)data + size;
		do
		{
			if (Character::IsUppercase(*p))
				*p |= 0x20;
		}
		while (++p != s);
	}
}

void Sequences::Uppercase(char *data, int size)
{

	{
		char * p = (char*)data;
		char * s = (char*)data + size;
		do
		{
			if (Character::IsLowercase(*p))
				*p &= ~0x20;
		}
		while (++p != s);
	}
}

void Sequences::Proppercase(char *data, int size)
{
	if ( data==0 || !(size>0) ) return;

	{

		char * p = (char*)data;
		char * s = (char*)data + size;

		while (p != s)
		{	
			if (Character::IsAlpha(*p))
			{
				*p++ &= ~0x20;
				while (p != s && Character::IsAlpha(*p))
					*p++ |= 0x20;
			}
			else
			{
				++p;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/output.h"

#include <string.h>

using namespace Reason::System;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char * Sequences::Search(const char * left, const char * right)
{
	OutputError("Sequences::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return (char*)strstr(left,right);
}

char * Sequences::Search(const char * left, const char * right, const int size)
{
	OutputError("Sequences::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * Sequences::Search(const char * left, const int leftSize, const char * right, const int rightSize)
{
	if (right == 0 || rightSize < 1 || rightSize > leftSize) return 0;

	int limit = leftSize-rightSize;
	for(int n=0;n <= limit;++n)
	{
		if (left[n] == right[0] && left[n+rightSize-1] == right[rightSize-1])
			if (Sequences::Equals((left+n),right,rightSize)) return (char *) (left+n);
	}

	return 0;	
}

char * Sequences::SearchCaseless(const char * left, const char * right)
{
	OutputError("Sequences::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * Sequences::SearchCaseless(const char * left, const char * right, const int size)
{
	OutputError("Sequences::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * Sequences::SearchCaseless(const char * left, const int leftSize, const char * right, int rightSize)
{
	if (right == 0 || rightSize < 1 || rightSize > leftSize) return 0;

	int limit = leftSize-rightSize;
	for(int n=0;n <= limit;++n)
	{
		if ( Characters::Caseless(left[n]) == Characters::Caseless(right[0]) && Characters::Caseless(left[n+rightSize-1]) == Characters::Caseless(right[rightSize-1]) )
			if (Sequences::EqualsCaseless((left+n),right,rightSize)) return (char *) (left+n);
	}

	return 0;	
}

char * Sequences::SearchReverse(const char * left, const char * right)
{
	OutputError("Sequences::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * Sequences::SearchReverse(const char * left, const char * right, const int size)
{
	OutputError("Sequences::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * Sequences::SearchReverse(const char * left, const int leftSize, const char * right, int rightSize)
{
	if (right == 0 || rightSize < 1 || rightSize > leftSize)
		return 0;

	int limit = leftSize-rightSize;
	for(int n=limit;n >= 0;--n)
	{
		if (left[n] == right[0] && left[n+rightSize-1] == right[rightSize-1])
			if (Sequences::Equals((left+n),right,rightSize)) return (char *) (left+n);
	}

	return 0;	
}

char * Sequences::SearchReverseCaseless(const char * left, const char * right)
{
	OutputError("Sequences::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * Sequences::SearchReverseCaseless(const char * left, const char * right, const int size)
{
	OutputError("Sequences::SearchReverseCaseless - Method not implemented\n");
	OutputTrap();
	return 0;
}

char * Sequences::SearchReverseCaseless(const char * left, const int leftSize, const char * right, int rightSize)
{

	if (right == 0 || rightSize < 1 || rightSize > leftSize)
		return 0;

	int limit = leftSize-rightSize;
	for(int n=limit;n >= 0;--n)
	{
		if ( Characters::Caseless(left[n]) == Characters::Caseless(right[0]) && Characters::Caseless(left[n+rightSize-1]) == Characters::Caseless(right[rightSize-1]) )
			if (Sequences::EqualsCaseless((left+n),right,rightSize)) return (char *) (left+n);
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

Identity Sequence::Instance;

Sequence::Sequence():
	Data(0),Size(0)
{

}

Sequence::Sequence(const Sequence & sequence):
	Data(sequence.Data),Size(sequence.Size)
{

}

Sequence::Sequence(char *data, int size):
	Data(data),Size(size)
{

}

Sequence::Sequence(const char *data):
	Data((char*)data),Size(String::Length(data))
{

}

Sequence::~Sequence()
{

}

Sequence & Sequence::operator = (const Sequence & sequence)
{
	Data = sequence.Data;
	Size = sequence.Size;
	return *this;
}

char Sequence::CharAt(int index)
{
	Indices::Normalise(index,Size);
	OutputAssert(index >= 0 && index < Size);

	return *(Data+index);
}

char & Sequence::ReferenceAt(int index)
{
	Indices::Normalise(index,Size);
	OutputAssert(index >= 0 && index <= Size);

	return *(Data+index);
}

char * Sequence::PointerAt(int index)
{
	Indices::Normalise(index,Size);
	OutputAssert(index >= 0 && index <= Size);

	return Data+index;
}

bool Sequence::Contains(char *data, int size, bool caseless)
{ 
	if (Data == 0) return 0; 

	return Sequences::Search(Data,Size,data,size,caseless) != 0;
}

int Sequence::Match(int from, char * data, int size)
{
	if (Indices::Normalise(from,Size))
	{
		Reason::Language::Regex::Regex regex(data,size);
		if (regex.Match(Data+from,Size-from))
			return regex.Matches[0][0].Data-Data;
	}

	return -1;
}

int Sequence::Match(int from, Reason::Language::Regex::Regex & regex)
{

	if (Indices::Normalise(from,Size))
	{
		if (regex.Match(Data+from,Size-from))
			return regex.Matches[0][0].Data-Data;
	}

	return -1;
}

int Sequence::LastIndexOf(int from, char *data, int size, bool caseless)
{
	Indices::Normalise(from,Size);

	if (from >= 0)
	{
		char * offset = Sequences::SearchReverse(Data,min(Size,from+1),data,size,caseless);
		if (offset)
			return offset-Data;
	}

	return -1;
}

int Sequence::LastIndexOf(int from, const char c,  bool caseless)
{
	Indices::Normalise(from,Size);

	if (from >= 0)
	{

		char *offset = Data+min(Size-1,from);
		if (caseless)
		{
			while (offset != Data)
			{
				if (Characters::Caseless(*offset) == Characters::Caseless(c)) 
					return offset-Data;
				--offset;
			}
		}
		else
		{
			while (offset != Data)
			{
				if (*offset == c) 
					return offset-Data;
				--offset;
			}
		}
	}

	return -1;
}

int Sequence::IndexOf(int from, char *data, int size,  bool caseless)
{
	if (Indices::Normalise(from,Size))
	{
		char * offset = Sequences::Search(Data+from,Size-from,data,size, caseless);
		if (offset)
			return offset-Data;
	}

	return -1;
}

int Sequence::IndexOf(int from, const char c, bool caseless)
{

	if (Indices::Normalise(from,Size))
	{
		char *offset = Data+from;
		if (caseless)
		{
			while (offset <= (Data+Size-1))
			{
				if (Characters::Caseless(*offset) == Characters::Caseless(c)) 
					return offset-Data;
				++offset;
			}
		}
		else
		{
			while (offset <= (Data+Size-1))
			{
				if (*offset == c) 
					return offset-Data;
				++offset;
			}
		}
	}

	return -1;
}

bool Sequence::StartsWith(char *prefix, int prefixSize, bool caseless)
{
	if (Size == 0 || Size < prefixSize || prefix == 0) return false;

	return  Sequences::Equals(Data,prefix,prefixSize,caseless);
}

bool Sequence::StartsWith(const char prefix, bool caseless)
{
	if (Size==0) return false;
	return Characters::Equals(CharAt(0),prefix,caseless);
}

bool Sequence::EndsWith(char *suffix, int suffixSize, bool caseless)
{
	if (Size == 0 || Size < suffixSize || suffix == 0) return false;

	char *end = Data+Size-suffixSize;
	return Sequences::Equals(end,suffix,suffixSize,caseless);
}

bool Sequence::EndsWith(const char suffix, bool caseless)
{
	if (Size==0) return false;
	return Characters::Equals(CharAt(Size-1),suffix,caseless);
}

bool Sequence::Is(char *data, int size, bool caseless)
{
	return Sequences::Equals(Data,Size,data,size,caseless);
}

bool Sequence::Is(char data, bool caseless)
{
	if (Size==0||Size>1) return false;
	return (caseless)?Characters::Caseless(*Data)==Characters::Caseless(data):*Data==data;
}

int Sequence::Hash()
{
	return Sequences::Hash(Data,Size);
}

int Sequence::Compare(Reason::System::Object *object,int comparitor)
{

	if (this == object) return 0;

	if (object->InstanceOf(Sequence::Instance))
	{
		Sequence * sequence = ((Sequence *)object);

		switch(comparitor)
		{
		case Comparable::COMPARE_GENERAL:case Comparable::COMPARE_PRECISE:
			return Sequences::Compare(Data,Size,sequence->Data,sequence->Size,!comparitor);
		default:
			return this - sequence;
		}
	}
	else
	{
		OutputError("Sequence::Compare - Invalid object type for comparison.\n");
		return Identity::Error;
	}
}

bool Sequence::Equals(Reason::System::Object * object,int comparitor)
{
	if (this == object) return true;

	if (object->InstanceOf(this))
	{
		Sequence * sequence = ((Sequence *)object);

		switch(comparitor)
		{
		case COMPARE_GENERAL:case COMPARE_PRECISE:
			return Sequences::Equals(Data,Size,sequence->Data,sequence->Size,!comparitor);
		default:
			return false;
		}
	}
	else
	{
		OutputMessage("Sequence::Equals - ERROR: Invalid object type for equality.\n");
		return false;
	}
}

void Sequence::Print(class String & string)
{
	string << *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
