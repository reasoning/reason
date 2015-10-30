
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

#include "reason/system/path.h"
#include "reason/system/output.h"
#include "reason/language/regex/regex.h"

#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>		

#ifdef REASON_PLATFORM_POSIX
	#include <unistd.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::System;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Identity Path::Instance;

Path::Path(const Path & path):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(path.Data,path.Size);
}

Path::Path(const Sequence & data):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(data.Data,data.Size);
}

Path::Path(const char * data):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(data);
}

Path::Path(char * data, int size):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(data,size);
}

Path::Path(char c):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(c);
}

Path::Path(int i):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(i);
}

Path::Path(float f):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(f);
}

Path::Path(double d):
	Lock(false),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{
	Construct(d);
}

Path::Path(bool lock):
	Lock(lock),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{

}

Path::Path(const String & string, bool lock):
	String(string),Lock(lock),Count(0),Parent(0),First(0),Last(0),Prev(0),Next(0)
{

}

Path::~Path()
{

	Path *path = First;
	while(path)
	{

		path = path->Next;
		delete First;
		First = path;
	}
}

Path * Path::Ancestor()
{
	Path * ancestor = this;
	while (ancestor->Parent)
		ancestor = ancestor->Parent;
	return ancestor;
}

int Path::Remaining()
{
	if (Parent)
	{
		return 0;
	}
	else
	{
		return String::Remaining();
	}
}

Path & Path::Allocate(int amount)
{

	if (Parent)
	{
		Parent->Allocate(amount);
		Activate();
		return *this;
	}
	else
	{

		char * data = Data;

		String::Allocate(amount);

		Path *path = First;
		char *offset = Data;
		while (path)
		{
			path->Relocate(offset+(path->Data-data));
			path = path->Next;
		}
	}

	return *this;
}

Path & Path::Assign(char * data, int size)
{

	if (data == 0 || size == 0)
	{

		if (IsActivated()) String::Assign(data,size);
	}
	else
	{	

		Deactivate();

		Path * parent = Parent;
		Path * path = 0, * next = 0, * prev = 0;

		prev = PrevSibling();
		if (prev && prev->Data > data)
		{
			while ((path = prev->PrevSibling()) && path->Data > data)
				prev = path;

			parent->Remove(this);
			parent->Insert(this,prev, Placeable::PLACE_BEFORE);

			next = prev;
			prev = PrevSibling();
		}

		next = NextSibling();
		if (next && next->Data < data)
		{
			while((path = next->NextSibling()) && next->Data < data)
				next = path;

			parent->Remove(this);
			parent->Insert(this,next, Placeable::PLACE_AFTER);

			prev = next;
			next = NextSibling();			
		}

		String::Assign(data,size);

		Activate();

		if (parent && (Data < parent->Data || Data+Size > parent->Data+parent->Size))
			Distribute(this);
		else
		if (prev && prev->Data+prev->Size > Data)
			Distribute(this);
		else
		if (next && Data+Size > next->Data)
			Distribute(this);
	}

	return *this;
}

void Path::Distribute(Path * origin)
{

	char * data = origin->Data;
	int size = origin->Size; 

	Path * parent = origin;
	Path * path = 0;
	int overlap = 0;

	while (parent)
	{

		path = parent->Next;
		while (path)
		{
			if (path->Count > 0)
			{
				path = path->First;
			}
			else
			{		
				while (path != parent->Parent && !path->Next)
					path = path->Parent;	

				if (path == parent->Parent) break;

				path = path->Next;			
			}		

			if (path->IsActivated())
			{
				int overlap = (data+size) - path->Data;
				if (overlap < 0) break;
				path->Data += overlap;
				path->Size -= (overlap > path->Size)?path->Size:overlap;	
			}
		}

		path = parent->Prev;
		while (path)
		{
			if (path->Count > 0)
			{
				path = path->Last;
			}
			else
			{
				while (path != parent->Parent && !path->Prev)
					path = path->Parent;

				if (path == parent->Parent) break;

				path = path->Prev;				
			}

			if (path->IsActivated())
			{
				int overlap = path->Data+path->Size - data;
				if (overlap < 0) break;
				path->Size -= (overlap > path->Size)?path->Size:overlap;	
			}
		}

		parent = parent->Parent;
		if (parent)
		{
			overlap = (data+size) - (parent->Data+parent->Size);
			if (overlap > 0)
			{
				parent->Size += overlap;
			}

			overlap = parent->Data - data;
			if (overlap > 0)
			{

				parent->Data -= overlap;
				parent->Size += overlap;
			}

			data = parent->Data;
			size = parent->Size;
		}

	}

}

Path & Path::Reserve(int amount)
{

	if (Parent)	
	{
		Parent->Reserve(amount);
	}
	else
	{
		String::Reserve(amount);
	}

	return *this;
}

Path & Path::Resize(int amount)
{

	if (Parent)
	{
		if (amount == 0 || amount > 0)
		{
			Right(amount-Size);
		}
		else
		if (amount < 0)
		{
			OutputError("Path::Resize - Cannot resize a negative amount.\n");
			OutputAssert(amount >= 0);
		}
	}
	else
	{

		String::Resize(amount);
	}

	return *this;
}

Path & Path::Clear()
{
	if (Size > 0) 
	{
		if (Parent)
		{

			OutputAssert(Data >= Parent->Data);
			Right(-Size);
		}
		else
		{

			Right(-Size);
		}
	}

	return *this;
}

Path & Path::Terminate()	
{
	if (Parent)
		Parent->Terminate();
	else
		Data[Size]=0;
	return *this;
}

void Path::Relocate(int offset)
{	
	if (IsActivated())
	{
		Path * path = First;
		while (path)
		{
			path->Relocate(offset);
			path = path->Next;
		}

		Data += offset;
	}
}

void Path::Relocate(char *offset)
{
	if (IsActivated())
	{	
		Path * path = First;
		while (path)
		{
			path->Relocate(offset + (path->Data - Data));
			path = path->Next;
		}
		Data = (char*) offset;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Path & Path::Left(int amount)
{
	if (amount < 0 && amount < -Size)
		amount = -Size;

	if (amount != 0)	
		Left(amount,this);

	return *this;
}

void Path::Left(int amount, Path * origin)
{

	if (Parent)
	{
		Parent->Left(amount,origin);
		Size += amount;

		Path * path = Next;
		while (path)
		{
			path->Relocate(amount);
			path = path->Next;
		}
	}
	else
	{
		OutputAssert(origin != 0);

		int offset = 0;
		int size = 0;

		if (amount > 0)
		{

			origin->Reserve(amount);
			origin->Activate();

			offset = (origin->Data)-Data;
			size = Size-offset;		
		}
		else
		if (amount < 0)
		{

			if (!origin->IsActivated())
				return;

			offset = (origin->Data-amount)-Data;
			size = Size-offset;	

			Path * prev = 0;
			Path * path = origin;
			while(path)
			{
				if (path->Count > 0)
				{
					path = path->First;
				}
				else
				if (path->Next)
				{
					path = path->Next;
				}
				else
				{
					while (path != origin && !path->Next)
						path = path->Parent;	

					if (path == origin) break;

					path = path->Next;		
				}

				if (path->IsActivated())
				{
					int overlap = (origin->Data-amount) - path->Data;

					if (overlap > 0)
					{
						path->Data += overlap;
						path->Size -= (overlap > path->Size)?path->Size:overlap;
					}

					if (prev) path->Data = prev->Data+prev->Size;
					prev = path;
				}
			}

		}	

		OutputAssert(origin->IsActivated());

		memmove(Data+offset+amount,Data+offset,size);

		OutputAssert(offset >= 0);
		OutputAssert(offset <= Size);
		OutputAssert(Data+offset+size <= Data+Size);

		Size += amount; 
		Terminate();
	}
}

Path & Path::Right(int amount)
{	
	if (amount < 0 && amount < -Size)
		amount = -Size;

	if (amount != 0)
		Right(amount,this);

	return *this;
}

void Path::Right(int amount, Path * origin)
{

	if (Parent)
	{
		Parent->Right(amount,origin);	
		Size += amount;

		Path * path = Next;
		while (path)
		{
			path->Relocate(amount);
			path = path->Next;
		}
	}
	else
	{

		OutputAssert(origin != 0);

		int offset = 0;
		int size = 0;

		if (amount > 0)
		{
			origin->Reserve(amount);
			origin->Activate();

			offset = (origin->Data+origin->Size)-Data;
			size = Size-offset;		
		}
		else
		if (amount < 0)
		{

			if (!origin->IsActivated())
				return;

			offset = (origin->Data+origin->Size)-Data;
			size = Size-(offset);

			Path * prev = 0;
			Path * path = origin;
			while(path)
			{
				if (path->Count > 0)
				{
					path = path->First;
				}
				else
				if (path->Next)
				{
					path = path->Next;
				}
				else
				{
					while (path != origin && !path->Next)
						path = path->Parent;	

					if (path == origin) break;

					path = path->Next;		
				}

				if (path->IsActivated())
				{
					int overlap = (path->Data+path->Size) - (origin->Data+origin->Size+amount);

					if (overlap > 0)
					{
						path->Size -= (overlap > path->Size)?path->Size:overlap;
					}

					if (prev) path->Data = prev->Data+prev->Size;
					prev = path;

				}
			}

		}	

		OutputAssert(origin->IsActivated());

		memmove(Data+offset+amount,Data+offset,size);

		OutputAssert(offset >= 0);
		OutputAssert(offset <= Size);
		OutputAssert(Data+offset+size <= Data+Size);

		Size += amount; 
		Terminate();	

	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Path::Activate(bool assign)
{

	if ( Parent )
	{
		if (!Parent->IsActivated())
		{
			if (IsActivated())
			{
				Parent->Data = Data;
				Parent->Size = Size;
			}

			Parent->Activate();
		}

		if (!IsActivated())
		{			
			Path * preceding = PrevSibling();
			if (preceding)
			{
				Data = preceding->Data + preceding->Size;
			}
			else
			{
				Data = Parent->Data;
			}

			if (assign)
			{
				Path * following = NextSibling();
				if (following)
				{
					Size = following->Data-Data;
				}
				else
				{
					Size = (Parent->Data+Parent->Size)-Data;
				}
			}
			else
			{

				Size = 0;
			}
		}
	}
}

void Path::Deactivate()
{
	if (Parent && IsActivated())
	{
		Data = 0;
		Size = 0;

		Path * path = First;
		while(path)
		{
			path->Deactivate();
			path = path->Next;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Path & Path::Attach(char * data, int size, int placement)
{
	OutputAssert(Parent);

	Parent->Insert(data,size,this,placement);
	return *this;
}

Path & Path::Attach(Path * path, int placement)
{
	OutputAssert(Parent);

	Parent->Insert(path,this,placement);
	return *this;
}

Path & Path::Insert(Path * path)
{

	OutputAssert(path->Parent == 0);
	OutputAssert(path->Next == 0);
	OutputAssert(path->Prev == 0);

	if (path->IsActivated() || path->Activated()) 
	{
		if (Count > 0 && path->IsPathOf(this))
		{

			Path * found = First->FirstSibling();
			if (found)
			{
				if (path->IsPathOf(found))
				{
					return found->Insert(path);
				}
				else
				{		
					if (found->Data >= path->Data+path->Size)
					{
						while (found->Data >= path->Data+path->Size)
							found = found->PrevSibling();

						if (found)
						{
							if (path->IsPathOf(found))
							{
								return found->Insert(path);
							}
							else
							if (found->Data+found->Size <= path->Data)
							{
								Insert(path,found,Placeable::PLACE_AFTER);
							}
							else
							{
								OutputError("Path::Insert - Cannot insert overlapping path.\n");
							}

							return *this;
						}

						return Prepend(path);
					}
					else
					if (found->Data+found->Size <= path->Data)
					{						
						while (found && found->Data+found->Size <= path->Data)
							found = found->NextSibling();

						if (found)
						{
							if (path->IsPathOf(found))
							{
								return found->Insert(path);
							}
							else
							if (found->Data >= path->Data+path->Size)
							{
								Insert(path,found,Placeable::PLACE_BEFORE);
							}
							else
							{
								OutputError("Path::Insert - Cannot insert overlapping path.\n");
							}

							return *this;
						}

						return Append(path);
					}
				}
			}
		}
	}

	return Append(path);
}

Path & Path::Swap(Path * left, Path * right)
{
	if (left != right)
	{

		Path *leftPrev = left->Prev;
		Path *rightPrev = right->Prev;

		if (leftPrev == right)
		{
			Remove(left);
			Insert(left,right,Placeable::PLACE_BEFORE);
		}
		else
		if (rightPrev == left)
		{
			Remove(right);
			Insert(right,left,Placeable::PLACE_BEFORE);
		}
		else
		{
			Remove(right);
			Insert(right,leftPrev,(leftPrev)?Placeable::PLACE_AFTER:Placeable::PLACE_BEFORE);
			Remove(left);
			Insert(left,rightPrev,(rightPrev)?Placeable::PLACE_AFTER:Placeable::PLACE_BEFORE);
		}
	}

	return *this;
}

Path & Path::Insert(char * data, int size, Path * place, int placement)
{

	if (!place->IsActivated()) place->Activate();

	int index;
	if (placement == PLACE_AFTER)
	{
		index = (place->Data+place->Size)-Data;
		place = place->Next;	
	}
	else
	if (placement == PLACE_BEFORE)
	{
		index = (place->Data)-Data;
	}

	String::Insert(data,size,index);

	while (place)
	{
		place->Relocate(size);
		place = place->Next;
	}

	return *this;
}

Path & Path::Insert(Path *path, const int index)
{

	OutputAssert(index >= 0 && index < Count);
	Insert(path,&(*this)[index],PLACE_BEFORE);
	return *this;
}

Path & Path::Insert(Path *path, Path *place, int placement)
{

	OutputAssert(place == 0 || place->Parent);
	OutputAssert(path->Next == 0 && path->Prev == 0 && path->Parent == 0);

	path->Parent = this;

	int index = 0;

	if (place == 0)
	{
		index = (placement == PLACE_BEFORE)?0:Size;

		if (Last == 0 || First == 0)
		{
			OutputAssert(Last==0&&First==0);
			First = path;
			Last = path;
		}
		else
		{

			place = (placement == PLACE_BEFORE)?First:Last;
		}
	}
	else
	{
		index = (placement == PLACE_BEFORE)?place->Data-Data:place->Data+place->Size-Data;
	}

	if (place != 0)
	{
		if (placement == PLACE_BEFORE)
		{
			if (place == First)
			{
				place->Prev = path;
				path->Next = place;
				First = path;
			}
			else
			{
				place->Prev->Next = path;
				path->Prev = place->Prev;
				place->Prev = path;
				path->Next = place;
			}
		}
		else
		if (placement == PLACE_AFTER)
		{
			if (place == Last)
			{
				place->Next = path;
				path->Prev = place;
				Last = path;
			}
			else
			{
				place->Next->Prev = path;
				path->Next = place->Next;
				place->Next = path;
				path->Prev = place;
			}
		}

		if (!path->IsEmpty() && !path->IsAllocated())
		{
			OutputAssert(!path->Prev || (path->Data >= path->Prev->Data+path->Prev->Size));
			OutputAssert(!path->Next || (path->Data+path->Size <= path->Next->Data));
		}
	}

	if (!path->IsEmpty())
	{
		if (path->IsAllocated())
		{
			String string;
			string.Acquire(*path);

			path->Data = Data+index;
			path->Size = 0;

			path->Replace(string);
		}

		OutputAssert(path->Data >= Data && path->Data <= (Data+Size));
	}

	++Count;

	return *this;
}

Path & Path::Remove(Path *path)
{
	if (path == 0) return *this;

	if (path->Prev != 0)
	{
		path->Prev->Next = path->Next;
	}
	else
	{
		First = path->Next;
	}

	if (path->Next != 0)
	{
		path->Next->Prev = path->Prev;
	}
	else
	{
		Last = path->Prev;
	}

	path->Parent = path->Next = path->Prev = 0;

	--Count;
	return *this;
}

Path & Path::Remove()
{
	Parent->Remove(this);
	return *this;
}

Path & Path::Delete(Path *path)
{
	Remove(path);
	delete path;
	return *this;
}

Path & Path::Delete()
{
	Parent->Delete(this);
	return *this;
}

Path & Path::operator [](int index)
{
	OutputAssert(index < Count);

	Path *path = First;
	for (int i=0;i<index;++i) 
		path = path->Next;

	return *path;
}

void Path::Release(bool complete)
{

	Path *path = First;
	while(path)
	{
		path->Release(complete);

		if ( path->Count == 0 && !path->Lock )
		{

			Path * next = path->Next;
			Delete(path);
			path = next;
		}
		else
		{
			path->Deactivate();
			path = path->Next;
		}
	}

	if (Parent)
		Deactivate();

	if (complete)
	{
		String::Release();
		Deactivate();
	}

}

void Path::Destroy(bool complete)
{

	Path * path = First;
	while (path != 0)
	{
		Path * target = path;
		path = path->Next;
		if (target->Lock)
		{

			target->Deactivate();
		}
		else
		{
			Delete(target);
		}
	}

	if (complete)
	{
		if (!Parent)
			String::Destroy();
		else
			Deactivate();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Path * Path::Select(Path *from, char *data, int size, bool caseless)
{
	if (data == 0)
		return 0;

	if (from && from->Is(data,size,caseless))
		return from;

	Path * path=0;
	while ( (from != 0) && !(path=from->Select(data,size,caseless)))
	{
		from = from->Next;
	}
	from = path;

	return from;
}

Path * Path::Containing(Path *from, char *data, int size, bool caseless)
{
	if (data == 0) return 0;

	if (Count > 0)
	{
		Path * path=0;
		while ( (from != 0) && !(path=from->Containing(data,size,caseless)))
		{
			from = from->Next;
		}
		from = path;
	}

	if (!from && Contains(data,size,caseless))
	{
		from = this;
	}

	return from;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Path * Path::FirstSibling(bool active)
{

	Path * path	= this;
	Path * found =0;

	if (!active || IsActivated())
		found = this;

	while ((path = path->PrevSibling(active)) != 0)
		found = path;

	if (found)
		return found;	

	path = this;
	found = path->NextSibling(active);

	if (found)
		return found;	

	if (!active || IsActivated())
		return this;

	return 0;
}

Path * Path::LastSibling(bool active)
{
	Path * path	= (Path*)this;
	Path * found = 0;

	if (!active || IsActivated())
		found = this;

	while ((path = path->NextSibling(active)) != 0)
		found = path;

	if (found)
		return found;	

	path = (Path*)this;
	found = path->PrevSibling(active);

	if (found)
		return found;	

	if (!active || IsActivated())
		return (Path*)this;

	return 0;
}

Path * Path::NextSibling(bool active)
{
	Path *path = Next;
	if (active)
	{
		while ( path != 0 && path->IsEmpty() )
			path = path->Next;
	}
	return path;
}

Path * Path::PrevSibling(bool active)
{
	Path *path = Prev;
	if (active)
	{
		while ( path != 0 && path->IsEmpty() )
			path = path->Prev;
	}
	return path;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Path::Tokenise(const char *tokens, bool keep)
{

	int count=0;
	char *p = (char *)tokens;
	while (*p != 0)
	{
		count += Split(p,1,keep);
		++p;
	}

	return count;
}

int Path::Split(char *data, int size, bool keep, bool nested)
{

	Path *path;

	if (!HasActivated())
	{

		path = new Path();
		path->Assign(((Sequence&)*this));	
		Insert(path,0,PLACE_AFTER);
		return path->Span(data,size,keep);
	}
	else
	{
		path = First->FirstSibling();
	}

	int count=0;
	while (path)
	{
		if (!path->HasActivated())
		{
			Path * split = path;
			if (nested)
			{
				Path *split = new Path();
				split->Assign(((Sequence&)*path));	
				path->Insert(split,0,PLACE_AFTER);
			}

			count += split->Span(data,size,keep);
		}
		else
		{

			count += path->Split(data,size,keep,nested);
		}

		path = path->NextSibling();
	}

	return count;
}

int Path::Span(char *data, int size, bool keep)
{

	int count=0;

	if (IsActivated())
	{
		int index=0;

		if (!HasActivated())
		{
			if (keep)
				index = IndexOf(size,data,size);
			else
				index = IndexOf(data,size);

			Path *path = this;
			while (index != -1)
			{
				if (keep)
				{

					Path *divide = new Path();
					divide->Data = path->Data + index;
					divide->Size = path->Size - index;
					path->Size -= divide->Size;

					if (Parent)
					{
						Parent->Insert(divide,path);
					}
					else
					{
						divide->Next = path->Next;
						divide->Prev = path;
						path->Next = divide;
					}

					path = divide;

					++count;

					index = path->IndexOf(size,data,size);

				}
				else
				{

					if (index == 0)
					{

						path->Data += size;
						path->Size -= size;
					}
					else
					if (index == ( path->Size - size ))
					{

						path->Size -= size;
					}
					else
					{

						Path *divide = new Path();
						divide->Data = path->Data + index + size;
						divide->Size = path->Size - (index + size);
						path->Size -= divide->Size + size;

						if (Parent)
						{
							Parent->Insert(divide,path);
						}
						else
						{
							divide->Next = path->Next;
							divide->Prev = path;
							path->Next = divide;
						}

						path = divide;

						++count;
					}

					index = path->IndexOf(data,size);
				}
			}
		}
		else
		{

			count += Split(data,size,keep);
		}
	}

	return count;

}

int Path::Span(int index, bool keep)
{
	if (IsActivated() && index > 0 && index < Size)
	{
		if (!HasActivated())
		{

			Path *path = new Path();
			path->Data = Data + index;
			path->Size = Size - index;
			Size -= path->Size;

			if (!keep)
			{
				++path->Data;
				--path->Size;
			}

			if (Parent)
			{
				Parent->Insert(path,this,PLACE_AFTER);
			}
			else
			{
				path->Next = this->Next;
				path->Prev = this;
				this->Next = path;
			}

			return 1;
		}
		else
		{

			return Split(index,keep);
		}
	}

	return 0;

}

int Path::Split(int index, bool keep, bool nested)
{
	if (index > 0 && index < ((Sequence&)*this).Size)
	{

		Path *path;

		if (!HasActivated())
		{
			path = new Path();
			path->Assign(((Sequence&)*this));	
			Insert(path,0,PLACE_AFTER);
		}
		else
		{
			path = First->FirstSibling();
		}

		while (path)
		{
			if (index > (path->Data-Data) && index < (path->Data-Data+path->Size))
			{
				int relativeIndex = index-(path->Data-Data);

				if (!path->HasActivated())
				{
					Path * split = path;
					if (nested)
					{
						split = new Path();
						split->Assign(((Sequence&)*path));	
						path->Insert(split,0,PLACE_AFTER);
					}

					return split->Span(index,keep);	
				}
				else
				{

					return path->Split(relativeIndex,keep,nested);
				}
			}

			path = path->NextSibling();
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Path::Span(Reason::Language::Regex::Regex & regex, bool keep)
{

	int count=0;
	regex.Match(0,Data,Size);
	for (int m=0;m<regex.Matches.Length();++m)
	{
		count += Span(regex.Matches[m][0],keep);
	}
	return count;
}

int Path::Split(Reason::Language::Regex::Regex & regex, bool keep, bool nested)
{

	regex.Match(0,Data,Size);

	int count = 0;
	Path *path;
	if (!HasActivated())
	{
		path = new Path();
		path->Assign(((Sequence&)*this));
		Insert(path,0,PLACE_AFTER);

		for (int m=0;m<regex.Matches.Length();++m)
		{
			count += path->Span(regex.Matches[m][0].Data,regex.Matches[m][0].Size,keep);
			path = path->LastSibling();
		}

		return count;
	}
	else
	{
		path = First->FirstSibling();
	}

	while (path)
	{
		if (!path->HasActivated())
		{
			Path * split = path;
			if (nested)
			{
				split = new Path();
				split->Assign(((Sequence&)*path));
				path->Insert(split,0,PLACE_AFTER);				
			}

			for (int m=0;m<regex.Matches.Length();++m)
			{
				count += split->Span(regex.Matches[m][0].Data,regex.Matches[m][0].Size,keep);
				split = split->LastSibling();
			}
		}
		else
		{

			for (int m=0;m<regex.Matches.Length();++m)
				count += path->Split(regex.Matches[m][0].Data,regex.Matches[m][0].Size,keep,nested);
		}

		path = path->NextSibling();
	}	
	return count;
}

Reason::System::Superstring Path::Join(char * data, int size, bool nested)
{
	Reason::System::Superstring join;
	if (HasActivated())
	{
		Path *path = First->FirstSibling();
		while (path)
		{
			if (path->HasActivated() && nested)
			{
				if (!join.IsEmpty())
					join.Append(data,size);
				join.Append(path->Join(data,size,nested));
			}
			else
			{
				if (!join.IsEmpty())
					join.Append(data,size);
				join.Append(path->Data,path->Size);
			}

			path = path->NextSibling();
		}	
	}

	return join;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PathIndex::PathIndex():
	Left(0),Right(0),Parent(0),Descendant(0),Ancestor(0),Children(0)
{

}

PathIndex::~PathIndex()
{

	Destroy();
}

void PathIndex::Destroy()
{
	if (Left)
	{
		delete Left;
	}

	if (Right)
	{
		delete Right;
	}

	if (Descendant)
	{
		delete Descendant;
	}

	Left = 0;
	Right = 0;
	Parent = 0;
	Descendant = 0;
	Ancestor = 0;
	Children = 0;

	List.Release();
}

void PathIndex::Print(String & string)
{

	PathIndex *index = this;

	string >> index->Key;

	while (index->Ancestor != 0)
	{
		index = index->Ancestor;
		string >> index->Key;
	}
}

PathIndex * PathIndex::Attach(Object * object, Path * path, bool caseless)
{	

	PathIndex *index=this;

	while (path && !path->IsEmpty())
	{
		Substring sequence = *path;

		if (index->Key.IsEmpty())
		{
			index->Key = sequence;
		}

		int dif = sequence.Compare(&index->Key,caseless);

		if (dif == 0)
		{
			if (path->Next == 0)
			{
				index->List.Append(object);
				return index;
			}
			else
			{
				if (index->Descendant == 0)
				{
					index->Descendant = new PathIndex();
					index->Descendant->Ancestor = index;
				}

				++index->Children;
				index = index->Descendant;
				path = path->Next;
			}
		}
		else
		{
			if (dif > 0)
			{

				if (index->Right == 0)
				{
					index->Right = new PathIndex();
					index->Right->Ancestor = index->Ancestor;
					index->Right->Parent = index;
				}

				index = index->Right;
			}
			else

			{

				if (index->Left == 0)
				{
					index->Left = new PathIndex();
					index->Left->Ancestor = index->Ancestor;
					index->Left->Parent = index;
				}

				index = index->Left;
			}
		}
	}

	return 0;

}

PathIndex *PathIndex::Select(Path * path, bool caseless)
{
	if (Key.Size == 0) return 0;
	if (path == 0) return 0;

	int dif = path->Compare(&Key,caseless);

	PathIndex *index=0;
	if (dif == 0)
	{
		if (path->Next == 0)
		{
			index = this;
		}
		else
		if (Descendant)
		{
			index = Descendant->Select(path->Next,caseless);
		}
	}
	else
	if (dif > 0)
	{

		if (Right != 0)
		{
			index = Right->Select(path,caseless);
		}
	}
	else

	{

		if (Left != 0)
		{
			index = Left->Select(path,caseless);
		}
	}

	return index;

}

PathIndex * PathIndex::FirstSibling()
{

	PathIndex * index = this;
	PathIndex * first;
	while ((first = index->PrevSibling()) != 0)
	{
		index = first;
	}

	return index;
}

PathIndex * PathIndex::LastSibling()
{

	PathIndex * index = this;
	PathIndex * last;
	while ((last = index->NextSibling()) != 0)
	{
		index = last;
	}

	return index;
}

PathIndex * PathIndex::FirstDescendant()
{

	PathIndex * index = this->Descendant;
	if (index)
	{
		index = index->FirstSibling();
	}
	return index;
}

PathIndex * PathIndex::LastDescendant()
{

	PathIndex * index = this->Descendant;
	if (index)
	{
		index = index->LastSibling();
	}
	return index;
}

PathIndex * PathIndex::NextSibling()
{

	PathIndex * index = this;

	if (index->Right)
	{
		index = index->Right;
		while (index->Left != 0)
		{
			index = index->Left;
		}
	}
	else
	if (index->Parent)
	{
		while (index)
		{
			if (index->Parent)
			{
				if (index->Parent->Right != index)
				{
					index = index->Parent;
					break;
				}
				else
				{
					index = index->Parent;
				}
			}
			else
			{
				index = 0;
				break;
			}
		}
	}
	else
	{
		index = 0;
	}

	return index;
}

PathIndex * PathIndex::PrevSibling()
{

	PathIndex * index = this;

	if (index->Left)
	{
		index = index->Left;
		while (index->Right != 0)
		{
			index = index->Right;
		}
	}
	else
	if (index->Parent)
	{
		while (index)
		{
			if (index->Parent)
			{
				if (index->Parent->Left != index)
				{
					index = index->Parent;
					break;
				}
				else
				{
					index = index->Parent;
				}
			}
			else
			{
				index = 0;
				break;
			}
		}
	}
	else
	{
		index = 0;
	}

	return index;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PathIndex::Enumerator::Enumerator(PathIndex * index):
	Root(index),EnumerandIndex(0),Enumerand(0),EnumerandNext(0),EnumerandPrev(0),EnumerandDirection(0)	
{
}

PathIndex::Enumerator::Enumerator():
	Root(0),EnumerandIndex(0),Enumerand(0),EnumerandNext(0),EnumerandPrev(0),EnumerandDirection(0)	
{
}

PathIndex::Enumerator::~Enumerator()
{
}

bool PathIndex::Enumerator::Move()
{
	return Move(1);
}

bool PathIndex::Enumerator::Move(int amount)
{
	amount *= EnumerandDirection;
	if (amount > 0)
	{
		if (EnumerandNext == 0)
		{
			Enumerand = Next();
		}
		else
		{
			Enumerand = EnumerandNext;
			EnumerandNext = 0;
		}

		++EnumerandIndex;
	}
	else
	if (amount < 0)
	{
		if (EnumerandPrev == 0)
		{
			Enumerand = Prev();
		}
		else
		{
			Enumerand = EnumerandPrev;
			EnumerandPrev = 0;
		}

		--EnumerandIndex;
	}

	return Enumerand != 0;
}

bool PathIndex::Enumerator::Forward()
{
	Enumerand = (PathIndex*) Root;

	while (Enumerand)
	{
		while (Enumerand->Left)
		{
			Enumerand = Enumerand->Left;
		}

		if (Enumerand->List.Count != 0)
			break;

		if (Enumerand->Descendant)
		{
			Enumerand = Enumerand->Descendant;
		}
		else
		{
			Enumerand = 0;
			break;
		}
	}

	EnumerandIndex = 0;
	EnumerandDirection = 1;
	return Enumerand != 0;
}

bool PathIndex::Enumerator::Reverse()
{

	Enumerand = (PathIndex*) Root;

	while (Enumerand)
	{
		while (Enumerand->Right)
		{
			Enumerand = Enumerand->Right;
		}

		if (Enumerand->Descendant)
		{
			Enumerand = Enumerand->Descendant;
		}
		else
		{
			Enumerand=0;
			break;
		}
	}

	EnumerandDirection = -1;
	return Enumerand != 0;
}

PathIndex * PathIndex::Enumerator::Next()
{

	PathIndex *pathIndex = Enumerand;
	while(pathIndex)
	{
		if (pathIndex->Descendant)
		{
			pathIndex = pathIndex->FirstDescendant();
		}
		else
		{
			PathIndex *next = pathIndex->NextSibling();
			if (next)
			{
				pathIndex = next;
			}
			else
			{
				while (pathIndex)
				{

					pathIndex = pathIndex->Ancestor;
					if (pathIndex && (next=pathIndex->NextSibling()) != 0)
					{
						pathIndex = next;
						break;
					}
				}
			}
		}

		if (pathIndex && !pathIndex->List.IsEmpty())
			break;
	}
	return pathIndex;
}

PathIndex * PathIndex::Enumerator::Prev()
{

	PathIndex *pathIndex = Enumerand;
	while(pathIndex)
	{
		if (pathIndex->Descendant)
		{
			pathIndex = pathIndex->LastDescendant();
		}
		else
		{
			PathIndex *prev = pathIndex->PrevSibling();
			if (prev)
			{
				pathIndex = prev;
			}
			else
			{
				while (pathIndex)
				{
					pathIndex = pathIndex->Ancestor;
					if (pathIndex && (prev=pathIndex->PrevSibling()) != 0)
					{
						pathIndex = prev;
						break;
					}
				}
			}
		}

		if (pathIndex && !pathIndex->List.IsEmpty())
			break;
	}
	return pathIndex;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
