
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

#include "reason/structure/objects/objects.h"
#include "reason/structure/array.h"
#include "reason/system/string.h"
#include "reason/system/output.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace Reason::Structure;
using namespace Reason::Structure::Objects;

namespace Reason { namespace Structure { namespace Objects {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Entry::Entry(Reason::System::Object *object, int key):
	Object(object),Next(0),Prev(0),Key(key)
{

}

Entry::Entry(Reason::System::Object *object):
	Object(object),Next(0),Prev(0),Key(0)
{

}

Entry::Entry():
	Object(0),Next(0),Prev(0),Key(0)
{

}

Entry::~Entry()
{

	Release();
}

void Entry::Release()
{

	Object	=0;
	Next	=0;
	Prev	=0;
	Key		=0;
}

void Entry::Destroy()
{

	delete Object;
	Release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Entry::Auto::Auto(Reason::System::Object *object, int key):
	Entry(object,key)
{

}

Entry::Auto::Auto(Reason::System::Object *object):
	Entry(object)
{

}

Entry::Auto::Auto()
{
}

Entry::Auto::~Auto()
{

	Destroy();
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

List::List(int order, int comparitor):
	Order(order),Comparison(comparitor),Count(0),First(0),Last(0)
{

}

List::~List()
{

	Release();
}

Reason::System::Object * List::operator [] (int index)
{
	return ObjectAt(index);
}

Reason::System::Object * List::ObjectAt(int index)
{
	return EntryAt(index)->Object;
}

Entry * List::EntryAt(int index)
{
	OutputAssert(index < Count && index > -1);

	Entry *entry = First;
	for (int i=0;i<index;++i)
		entry = entry->Next;

	return entry;
}

void List::Remove(Reason::System::Object * object)
{
	Entry * entry = SelectEntry(object,Comparable::COMPARE_INSTANCE);
	if (entry)
	{
		RemoveEntry(entry);
		delete entry;
	}
}

void List::RemoveEntry(Entry * entry)
{
	if (Count == 1)
	{
		First	=0;
		Last	=0;
	}
	else
	if (entry == First)
	{
		First->Next->Prev = 0;
		First = First->Next;
	}
	else
	if (entry == Last)
	{
		Last->Prev->Next = 0;
		Last = Last->Prev;
	}
	else
	{
		entry->Next->Prev = entry->Prev;
		entry->Prev->Next = entry->Next;
	}

	entry->Prev = 0;
	entry->Next = 0;

	--Count;
}

void List::Delete(Reason::System::Object *object)
{
	Remove(object);
	delete object;
}

void List::DeleteEntry(Entry *entry)
{
	RemoveEntry(entry);
	delete entry;
}

void List::Replace(Reason::System::Object *object, Reason::System::Object *replacement)
{
	if (Select(object,COMPARE_INSTANCE)==0)
		return;

	Insert(replacement,object);
	Remove(object);
}

void List::Release()
{
	Entry *entry = First;
	while (entry != 0)
	{
		entry = entry->Next;

		delete First;
		First = entry;
	}

	Count = 0;
	First = 0;
	Last  = 0;
}

void List::Destroy()
{
	Entry *entry = First;
	while (entry != 0)
	{
		entry = entry->Next;

		First->Destroy();
		delete First;
		First = entry;
	}

	Count = 0;
	First = 0;
	Last  = 0;
}

void List::Print(String & string)
{
	string << "<hv:object-vector count=\"" << Count << "\">\n";

	Entry * P = First;
	while ( P != 0)
	{
		string << "<hv:object-entry>";
		P->Object->Print(string);
		string << "</hv:object-entry>\n";

		P = P->Next;
	}

	string << "</hv:object-vector>";
}

void List::InsertEntry(Entry  *entry, Entry * index, int placement)
{
	OutputAssert(entry != 0);
	OutputAssert(entry->Object != 0);

	if (index == 0)
	{
		switch (placement)
		{
		case PLACE_BEFORE:

			index = First;
		break;
		case PLACE_AFTER:

			index = Last;
		break;
		case PLACE_RANDOM:

			index = EntryAt(rand()%Length());
		break;
		}
	}
	else
	{
		if (placement == PLACE_RANDOM)
		{
			OutputError("List::Insert - Random placement cannot be used in conjunction with a index.\n");
		}
	}

	if (index == 0)
	{

		entry->Next = 0;
		entry->Prev = 0;
		First = Last = entry;
	}
	else
	{
		if (placement == PLACE_AFTER)
		{

			if (index == Last)
			{
				OutputAssert(entry->Next == 0);
				Last = entry;
			}
			else
			{
				entry->Next			= index->Next;
				entry->Next->Prev	= entry;
			}

			entry->Prev			= index;
			entry->Prev->Next	= entry;
		}
		else
		{

			if (index == First)
			{
				OutputAssert(entry->Prev == 0);
				First = entry;
			}
			else
			{
				entry->Prev			= index->Prev;
				entry->Prev->Next	= entry;
			}

			entry->Next			= index;
			entry->Next->Prev	= entry;
		}
	}

	#ifdef _DEBUG
	if (entry != Last)
		OutputAssert(entry->Next != 0);

	if (entry != First)
		OutputAssert(entry->Prev != 0);
	#endif

	++Count;
}

void List::InsertEntry(Entry *entry, int placement)
{

	if (Order == ORDER_DEFAULT)
	{
		InsertEntry(entry,0,placement);
	}
	else
	if (Order == ORDER_ASCENDING)
	{

		Entry *index = First;
		while ( index != 0 && index->Object->Compare(entry->Object,Comparison) < 0)
		{
			index = index->Next;
		}

		if (index == 0)
		{

			InsertEntry(entry,0,PLACE_AFTER);
		}
		else
		{

			InsertEntry(entry,index,PLACE_BEFORE);
		}
	}
	else
	if (Order == ORDER_DESCENDING)
	{

		Entry *index = First;
		while (index != 0 && index->Object->Compare(entry->Object,Comparison) > 0)
		{
			index = index->Next;
		}

		if (index == 0)
		{

			InsertEntry(entry,0,PLACE_AFTER);
		}
		else
		{

			InsertEntry(entry,index,PLACE_BEFORE);
		}
	}
	else
	{

		OutputError("List:Insert - Invalid order was specified, object not added to list.\n");
	}
}

Reason::System::Object *List::Select(Reason::System::Object *object,int comparitor)
{
	Entry *entry = SelectEntry(object,comparitor);
	return (entry==0)?0:entry->Object;
}

Entry * List::SelectEntry(Reason::System::Object *object,int comparitor)
{
	if (Order == ORDER_DEFAULT || comparitor != Comparison)
	{

		Entry *entry = First;
		while (entry != 0)
		{
			if (entry->Object->Equals(object,comparitor))
				return entry;

			entry = entry->Next;
		}
		return 0;
	}
	else
	{

		int result	=0;		
		int first	=0;
		int last	=Count-1;
		int middle	=0;		
		int offset	=0;		

		Entry *pivot = First;

		while (first <= last)
		{

			offset = middle;
			middle = (first+last)>>1;
			offset = abs(middle-offset);

			if (result < 0)
			{
				for(int i=offset;i>0;--i)
					pivot = pivot->Prev;
			}
			else

			{
				for(int i=offset;i>0;--i)
					pivot = pivot->Next;
			}

			if (pivot == 0 || (result = pivot->Object->Compare(object,comparitor)) == 0)
			{
				return pivot;
			}
			else
			{

				result=(result>0)?((Order==ORDER_ASCENDING)?-1:1):((Order==ORDER_DESCENDING)?-1:1);

				switch(result)
				{
				case  1:first = middle+1;break;
				case -1:last  = middle-1;break;
				}
			}
		}

		return 0;
	}
}

void List::Union(List &list)
{

	Entry *P = list.First;

	while (P != 0)
	{
		if (Select(P->Object,COMPARE_INSTANCE)==0)
		{
			Append(P->Object);
		}

		P = P->Next;
	}
}

void List::Intersection(List &list)
{
	OutputWarning("List::Intersection - Unsafe method, potential memory leak.\n");

	Entry *P = First;

	while (P != 0)
	{
		if (list.Select(P->Object,COMPARE_INSTANCE) == 0)
		{
			Reason::System::Object * D = P->Object;
			P = P->Next;
			Remove(D);
		}
		else
		{
			P = P->Next;
		}
	}
}

void List::Difference(List &list)
{
	OutputWarning("List::Difference - Unsafe method, potential memory leak.\n");

	Entry *P = list.First;

	while (P != 0)
	{
		Reason::System::Object *D = Select(P->Object,COMPARE_INSTANCE);
		if (D != 0)
		{
			Remove(D);
		}

		P = P->Next;
	}
}

void List::TakeEntry(Entry *entry, List &from)
{
	from.RemoveEntry(entry);
	AppendEntry(entry);
}

void List::Take(Reason::System::Object *object, List &from)
{
	TakeEntry(SelectEntry(object),from);
}

void List::Take(List &from)
{
	First	= from.First;
	Last	= from.Last;
	Count	= from.Count;

	from.First	= 0;
	from.Last	= 0;
	from.Count	= 0;
}

void List::Rotate(int rotation)
{

	rotation = rotation % Count;

	Entry * entry;

	if (rotation > 0)
	{

		while (rotation > 0)
		{

			entry = Last;
			RemoveEntry(entry);
			PrependEntry(entry);
			-- rotation;
		}
	}
	else
	if (rotation < 0)
	{

		while (rotation < 0)
		{

			entry = First;
			RemoveEntry(entry);
			AppendEntry(entry);
			++rotation;
		}
	}

}

void List::Shuffle(int seed )
{

	if (Count > 2)
	{

		Entry * entry = Last;
		Entry * index;

		int remaining = Count-1;

		srand(seed);

		while (remaining > 0 && (index = entry->Prev) != 0)
		{
			SwapEntry(entry,EntryAt(rand()%remaining));
			entry = index;
			-- remaining;
		}
	}
}

void List::Sort(int order)
{

	if (order == ORDER_DEFAULT)
	{
		order = ORDER_ASCENDING;
	}
	else
	if (order != ORDER_ASCENDING && order != ORDER_DESCENDING)
	{
		OutputError("List:Sort - Sort failed, an invalid order was specified.\n");
		return;
	}

	if (!First || !Last)
		return;

	Entry * root = 0;
	Entry * node = 0;

	Entry * min = 0;
	Entry * max = 0;

	Entry * entry = 0;

	root = First;
	First = First->Next;
	root->Prev = 0;
	root->Next = 0;

	min = max = root;

	while (First)
	{
		entry = First;
		First = First->Next;
		entry->Prev = entry->Next = 0;

		node = root;

		if (min)
		{
			int compare = entry->Compare(min);
			if (compare == 0)
			{
				node = min;
			}
			else
			if (compare < 0)
			{
				min->Prev = entry;
				min = entry;
				continue;
			}
		}

		if (max)
		{
			int compare = entry->Compare(max);
			if (compare == 0)
			{
				node = max;
			}
			else
			if (compare > 0)
			{
				max->Next = entry;
				max = entry;
				continue;
			}
		}

		while (node)
		{

			int compare = entry->Compare(node);
			if (compare == 0)
			{
				if (order == ORDER_ASCENDING)
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
				else
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
			}
			else
			if (compare < 0)
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

	Reason::Structure::Array<Entry *> stack;

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

void List::Swap(int i,int j)
{
	SwapEntry(EntryAt(i),EntryAt(j));
}

void List::Swap(Reason::System::Object * i, Reason::System::Object * j)
{
	SwapEntry(SelectEntry(i),SelectEntry(j));
}

void List::SwapEntry(Entry * i, Entry * j)
{
	if (i != j)
	{

		Entry *ii = i->Prev;
		Entry *jj = j->Prev;

		if (ii == j)
		{
			RemoveEntry(i);
			InsertEntry(i,j,PLACE_BEFORE);
		}
		else
		if (jj == i)
		{
			RemoveEntry(j);
			InsertEntry(j,i,PLACE_BEFORE);
		}
		else
		{

			RemoveEntry(j);
			InsertEntry(j,ii,(ii)?PLACE_AFTER:PLACE_BEFORE);
			RemoveEntry(i);
			InsertEntry(i,jj,(jj)?PLACE_AFTER:PLACE_BEFORE);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Array::Array():Data(0),Size(0),Allocated(0)
{
}

Array::~Array()
{

	if (Data)
		delete [] Data;
}

void Array::Reallocate(int amount)
{

	if (Allocated)
	{
		OutputAssert((Size > 0 && Data != 0) || (Size == 0));

		if (amount == 0)
		{

			amount = (int) Allocated * (1 + Increment()) + 1;
			Object ** array = new Object * [amount];

			memcpy(array,Data,sizeof(Object*) * Size);
			delete [] Data;

			Data = array;
			Allocated = amount;
		}
		else
		if (amount > 0)
		{
			if (amount < Size)
			{
				OutputError("Array::Reallocate - Cannot allocate less storage than required.\n");
				return;
			}

			Object ** array = new Object * [amount];
			memcpy(array,Data,sizeof(Object*) * Size);
			delete [] Data;

			Data = array;
			Allocated = amount;
		}
		else
		if (amount < 0)
		{
			OutputError("Array::Reallocate - Cannot allocate a negative amount of storage.\n");
			return;
		}
	}
	else
	{
		Allocate(amount);
	}
}

void Array::Resize(int amount)
{
	if (Size+amount < 0)
	{
		Size=0;
	}
	else
	if (Data+Size+amount > Data+Allocated)
	{
		Reserve(amount);
		Size+=amount;
	}
}

void Array::Reserve(int amount)
{
	if (amount != 0)
	{

		amount = (Allocated+amount > Size)?Allocated+amount:Size;
	}

	Reallocate(amount);
}

void Array::Allocate(int amount)
{
	if (!Allocated)
	{
		OutputAssert((Size > 0 && Data != 0) || (Size == 0));

		if (amount == 0)
		{
			Allocated = (int) Size * (1 + Increment()) + 1;
		}
		else
		if (amount > 0)
		{
			if (amount < Size)
			{
				OutputError("Array::Allocate - Cannot allocate less storage than required.\n");
				OutputAssert(amount < Size);
				return;
			}

			Allocated = amount;
		}
		else

		{
			OutputError("Array::Allocate - Cannot allocate a negative amount of storage.\n");
			OutputAssert(amount >= 0);
			return;
		}

		Data = new Object * [Allocated];
		Size = 0;
	}
	else
	{
		OutputError("Array::Allocate - Array is already allocated, use Reallocate().\n");
	}
}

void Array::Displace(int amount)
{
	if (amount > 0)
	{

		memmove(Data+amount,Data, sizeof(Reason::System::Object*) * Size-amount);

		#ifdef _DEBUG
			memset(Data,0,sizeof(Reason::System::Object*) * amount);
		#endif
	}
	else
	if (amount < 0)
	{

		OutputMessage("Array::Displace - Negative amount, request to truncate data will not be honoured.\n");
	}
}

void Array::Insert(Object* object, int index)
{

	OutputAssert(index >= 0 && index <= Size);

	if (Size == Allocated)
	{
		Reallocate(0);
	}

	memcpy(Data+index+1,Data+index,(Size-index));
	Data[index]=object;
	++Size;
}

void Array::Insert(Object* object)
{
	Insert(object,Size);
}

void Array::Append(Object* object)
{
	if (Allocated == 0)
	{
		Reallocate(4);
	}
	else
	if (Size == Allocated)
	{
		Reallocate(0);
	}

	Data[Size++] = object;
}

void Array::Prepend(Object* object)
{
	if (Allocated == 0)
	{
		Reallocate(4);
	}
	else
	if (Size == Allocated)
	{
		Reallocate(0);
	}

	Displace(1);
	Data[0] = object;
	++Size;
}

void Array::Remove(Object* object)
{

	for (int i=Length()-1;i>-1;--i)
	{
		if (Data[i] == object)
		{
			RemoveAt(i); 
			break;
		}
	}
}

Object * Array::RemoveAt(int index)
{
	Object * object = Data[index];
	memmove(Data+index,Data+index+1,(sizeof(Object*) * (Size-index-1)));
	--Size;
	return object;
}

void Array::Delete(Object *object)
{

	for (int i=Length()-1;i>-1;--i)
	{
		if (Data[i] == object)
			return DeleteAt(i);
	}
}

void Array::DeleteAt(int index)
{
	Object *object = Data[index];
	delete object;
	memmove(Data+index,Data+index+1,(sizeof(Object*) * (Size-index-1)));
	--Size;
}

void Array::Release()
{

	for (int i=0;i<Length();++i)
	{
		Data[i]=0;
	}
}

void Array::Destroy()
{

	for (int i=0;i<Length();++i)
	{
		delete Data[i];
		Data[i]=0;
	}
}

void Array::Print(String & string)
{
	string << "<hv:object-heap size=\"" << Size << "\" allocated=\"" << Allocated << "\">\n";

	for (int i=0;i<Size;++i)
	{
		string << "<hv:object-entry>";
		Data[i]->Print(string);
		string << "</hv:object-entry>\n";
	}

	string << "</hv:object-heap>";
}

Reason::System::Object * Array::ObjectAt(int index)
{
	OutputAssert(index > -1 && index < Size);
	return Data[index];
}

int Array::IndexOf(Reason::System::Object *object)
{
	for (int i=0;i<Size;++i)
	{
		if (Data[i] == object)
			return i;
	}
	return -1;
}

void Array::Swap(int i,int j)
{
	OutputAssert(i >= 0 && i < Size && j >= 0 && j < Size);
	if (i != j)
	{
		Reason::System::Object * entry = Data[i];
		Data[i] = Data[j];
		Data[j] = entry;
	}
}

void Array::Rotate(int rotation)
{

	rotation = rotation % Size;
	Reason::System::Object * object;

	if (rotation > 0)
	{

		while (rotation > 0)
		{

			object = Data[Size-1];
			RemoveAt(Size-1);
			Prepend(object);
			-- rotation;
		}
	}
	else
	if (rotation < 0)
	{

		while (rotation < 0)
		{

			object = Data[0];
			RemoveAt(0);
			Append(object);
			++rotation;
		}
	}

}

void Array::Shuffle(int seed)
{

	if (Size > 2)
	{

		srand(seed);

		for (int i=Size-1;i>0;--i)
		{
			Swap(i,rand()%i);
		}
	}
}

void Array::Take(Array & array)
{
	Take(array,0,array.Size);
}

void Array::Take(Array & array, int offset, int amount)
{
	OutputAssert(array.Size-offset >= amount);

	if (Remaining() <= amount)
		Reallocate(Size + (int)(Allocated*Increment())+amount);

	memcpy(Data+Size,array.Data+offset,amount);
	memmove(array.Data+offset,array.Data+offset+amount,array.Size - (offset+amount));
	array.Size -= amount;
	Size += amount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Enumeration::Enumeration(int order, int comparitor):
	List(order,comparitor),EnumeratorIndex(0),Enumerator(0),EnumeratorDirection(0)
{

}

Enumeration::~Enumeration()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Entry::Enumerator::Enumerator(Entry *first,Entry *last,int size):
	Enumerand(0),EnumerandFirst(first),EnumerandLast(last),EnumerandIndex(0),EnumerandCount(size),EnumerandDirection(0)
{
}

Entry::Enumerator::Enumerator(List & list):
	Enumerand(0),EnumerandFirst(list.First),EnumerandLast(list.Last),EnumerandIndex(0),EnumerandCount(list.Count),EnumerandDirection(0)
{
}

Entry::Enumerator::Enumerator():
	Enumerand(0),EnumerandFirst(0),EnumerandLast(0),EnumerandIndex(0),EnumerandCount(0),EnumerandDirection(0)
{
}

Entry::Enumerator::~Enumerator()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////

Stack::Stack()
{

}

Stack::~Stack()
{

}

void Stack::Push(Entry *entry)
{
	AppendEntry(entry);
}

Reason::System::Object * Stack::Peek()
{
	if (Last)
		return Last->Object;
	else
		return 0;
}

Reason::System::Object * Stack::Pop()
{
	if (Last)
	{
		Entry * entry = Last;

		if (Last->Prev != 0)
		{
			Last->Prev->Next = 0;
			Last = Last->Prev;
		}
		else
		{
			Last	=0;
			First	=0;
		}

		Reason::System::Object * object = entry->Object;
		delete entry;

		--Count;
		return object;
	}

	return 0;
}

Entry * Stack::SelectEntry(Reason::System::Object *object,int comparitor)
{

	Entry *entry = Last;
	while (entry != 0)
	{
		if (entry->Object->Equals(object,comparitor))
			return entry;

		entry = entry->Prev;
	}

	return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Hashtable::Hashtable():
	Modified(0),Resized(0),Count(0),Entries(0),Capacity(0),Threshold(0.75)
{

}

Hashtable::Hashtable(int capacity) :
	Modified(0),Resized(0),Count(0),Entries(0),Capacity(0),Threshold(0.75)
{
	Allocate(capacity);
}

Hashtable::~Hashtable()
{

	Release();
	if (Entries)
		delete [] Entries;
}

void Hashtable::Allocate(int amount)
{
	if (!Capacity)
	{
		OutputAssert(Entries == 0 && Capacity == 0);
		Entries = new Entry * [amount];
		memset(Entries,0, amount * sizeof(Entry *));
		Capacity = amount;
	}
}

void Hashtable::Release()
{
	Entry *entry;
	for (int i=0;i<Capacity;++i)
	{
		while (Entries[i] != 0)
		{
			entry = Entries[i];
			Entries[i] = entry->Next;
			delete entry;
		}
	}

	Count=0;
	Resized=0;
	Modified=0;
}

void Hashtable::Destroy()
{
	Entry *entry;
	for (int i=0;i<Capacity;++i)
	{
		while (Entries[i] != 0)
		{
			entry = Entries[i];
			Entries[i] = entry->Next;

			entry->Destroy();
			delete entry;
		}
	}

	Count=0;
	Resized=0;
	Modified=0;

	if (Entries)
		delete [] Entries;
	Entries=0;
	Capacity=0;
}

void Hashtable::Insert(Reason::System::Object *object, int key)
{
	InsertEntry(new Entry(object,key));
}

void Hashtable::InsertEntry(Entry *entry)
{
	if (Capacity == 0)
	{
		Allocate(31);
	}
	else
	if (Count == (int)(Threshold*Capacity))
	{
		Reallocate();
	}

	if (entry->Key==0)
		entry->Key = entry->Object->Hash();

	int index = (unsigned int) entry->Key % Capacity;

	entry->Next = Entries[index];
	Entries[index] = entry;

	++Count;
	++Modified;
}

void Hashtable::Remove(Reason::System::Object *object, int key)
{
	Entry *entry = SelectEntry(object,key,COMPARE_INSTANCE);
	if (entry)
	{
        RemoveEntry(entry);
		delete entry;
	}
}

void Hashtable::RemoveEntry(Entry * entry)
{
	if (entry->Prev != 0)
	{
		entry->Prev = entry->Next;
	}
	else
	{
		Entries[(unsigned int)entry->Key % Capacity] = entry->Next;
	}

	entry->Prev = 0;
	entry->Next = 0;

	--Count;
	++Modified;
}

void Hashtable::Delete(Reason::System::Object *object, int key)
{
	Remove(object,key);
	delete object;
}

void Hashtable::DeleteEntry(Entry *entry)
{
	RemoveEntry(entry);
	delete entry;
}

Reason::System::Object * Hashtable::Select(Reason::System::Object *object, int key, int comparitor)
{
	Entry * entry = SelectEntry(object,key,comparitor);
	return(entry==0)?0:entry->Object;
}

Entry * Hashtable::SelectEntry(Reason::System::Object *object, int key, int comparitor)
{
	if (Count==0) return 0;

	if (key==0) key = object->Hash();

	Entry *entry = Entries[(unsigned int)key % Capacity];

	while (entry != 0)
	{
		if (entry->Key == key && entry->Object->Compare(object,comparitor)==0)
			break;

		entry = entry->Next;
	}

	return entry;	
}

Reason::System::Object * Hashtable::Match(int key, Reason::System::Object *index)
{
	Entry *entry=0;

	if (index != 0)
		entry = SelectEntry(index,key,COMPARE_INSTANCE);

	entry = MatchEntry(key,entry);

	return (entry==0)?0:entry->Object;
}

Entry * Hashtable::MatchEntry(int key, Entry *index)
{
	if (Count==0) return 0;

	if (index == 0)
	{
        index = Entries[((unsigned int)key % Capacity)];
	}
	else
	{
		index = index->Next;
	}

	while (index != 0)
	{
		if (index->Key == key)
			break;

		index = index->Next;
	}

	return index;
}

void Hashtable::Reallocate(int amount)
{

	if (amount < 0)
		return;

	if (Capacity)
	{

		amount = (amount==0)?(int)(Capacity * 2+1):Capacity + amount;

		++Resized;

		Entry ** storage = new Entry* [amount];
		memset(storage,0, amount * sizeof(Entry *));

		Entry *entry;
		int index;

		for (int i=0;i < Capacity;++i)
		{
			while (Entries[i] != 0)
			{
				entry = Entries[i];
				Entries[i] = Entries[i]->Next;

				index = (unsigned int)entry->Key % amount;

				entry->Next = storage[index];
				storage[index] = entry;
			}
		}

		Capacity = amount;
		delete [] Entries;
		Entries = storage;

	}
	else
	{
		Allocate(amount);
	}
}

void Hashtable::Print(String & string)
{
	string << "<hv:object-storage count=\"" << Count << "\" capacity=\"" << Capacity << "\">\n";

	Reason::Structure::Objects::Entry * entry;
	for (int i=0;i<Capacity;++i)
	{
		if ((entry = Entries[i]) != 0)
		{
			do
			{
				string << "<hv:object-entry>";
				entry->Object->Print(string);
				string << "</hv:object-entry>\n";
			}
			while ((entry = entry->Next) != 0);
		}
	}

	string << "</hv:object-storage>";
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

Identity Mapped::Instance;

Mapped::Mapped(Reason::System::Object *key, Reason::System::Object *value)
{
	Key = key;
	Value = value;
}

Mapped::Mapped():Value(0)
{

}

Mapped::~Mapped()
{

}

int Mapped::Compare(Reason::System::Object *object, int comparitor)
{
	if (object->InstanceOf(this))
	{
		return Key->Compare(((Mapped*)object)->Key,comparitor) && Value->Compare(((Mapped*)object)->Value,comparitor);
	}
	else
	if (object->InstanceOf(Key))
	{
		return Key->Compare(object,comparitor);
	}
	else
	if (object->InstanceOf(Value))
	{
		return Value->Compare(object,comparitor);
	}
	else
	{
		OutputMessage("ObjectPair::Compare - ERROR: Invalid object type for comparison.\n");
		return Identity::Error;
	}
}

void Mapped::Release()
{
	Key	=0;
	Value	=0;
}

void Mapped::Destroy()
{
	delete Key;
	delete Value;
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

Ring::Ring()
{
	Count		=0;
	Sentinel	=0;
}

Ring::~Ring()
{
	if (Sentinel)
	{
		Entry *entry = Sentinel;
		Sentinel->Prev->Next = 0;	
		while (entry != 0)
		{
			entry = entry->Next;
			Sentinel->Destroy();
			delete Sentinel;
			Sentinel = entry;
		}
	}
}

void Ring::Insert(Entry *entry,bool first)
{
	if (!first)
	{

		Insert(entry,Sentinel,true);
	}
	else
	{

		Insert(entry,Sentinel);
		Sentinel = Sentinel->Next;
	}
}

void Ring::Insert(Entry *entry,Entry *index, bool before)
{
	if (index == 0)
	{
		index = Sentinel;
	}

	if (index == 0)
	{

		entry->Next = entry;
		entry->Prev = entry;
		Sentinel = entry;
	}
	else
	{
		if (!before)
		{

			entry->Next			= index->Next;
			entry->Next->Prev	= entry;
			entry->Prev			= index;
			entry->Prev->Next	= entry;
		}
		else
		{

			entry->Prev			= index->Prev;
			entry->Prev->Next	= entry;
			entry->Next			= index;
			entry->Next->Prev	= entry;
		}
	}

	++Count;
}

void Ring::Remove(Reason::System::Object *object)
{
	Entry * entry = Select(object);
	if (entry)
	{
		Remove(entry);
		delete entry;
	}
}

void Ring::Remove(Entry *entry)
{
	if (Count == 1)
	{
		Sentinel=0;
	}
	else
	{
		if (Sentinel == entry)
			Sentinel = entry->Next;

		entry->Next->Prev = entry->Prev;
		entry->Prev->Next = entry->Next;
	}

	entry->Next=0;
	entry->Prev=0;

	--Count;

}

void Ring::Delete(Reason::System::Object *object)
{
	Remove(object);
	delete object;
}

void Ring::Delete(Entry *entry)
{
	Remove(entry);
	delete entry;
}

void Ring::Release()
{
	if (Sentinel)
	{
		Entry *P = Sentinel;
		Sentinel->Prev->Next = 0;	
		while (P != 0)
		{
			P = P->Next;
			delete Sentinel;
			Sentinel = P;
		}
	}

	Count = 0;
	Sentinel = 0;

}

void Ring::Destroy()
{
	if (Sentinel)
	{
		Entry *P = Sentinel;
		Sentinel->Prev->Next = 0;	
		while (P != 0)
		{
			P = P->Next;
			Sentinel->Destroy();
			delete Sentinel;
			Sentinel = P;
		}
	}

	Count = 0;
	Sentinel = 0;
}

Entry *Ring::Contains(Reason::System::Object *object)
{
	Entry* O = Sentinel;
	if (O)
	{
		do
		{
			if (O->Object == object)
				break;
		}
		while((O = O->Next) != Sentinel );
	}
	return O;
}

Entry *Ring::Select(Reason::System::Object *object,bool exact)
{
	Entry* O = Sentinel;
	if (O)
	{
		do
		{
			if (O->Object->Compare(object,exact) == 0)
				break;
		}
		while((O = O->Next) != Sentinel );
	}
	return O;
}

void Ring::Print(String & string)
{
	string << "<hv:object-ring count=\"" << Count << "\">\n";

	Entry * P = Sentinel;
	if (P)
	{
		do
		{
			string << "<hv:object-entry>";
			P->Object->Print(string);
			string << "</hv:object-entry>\n";
		}
		while ((P = P->Next) != Sentinel);
	}
	string << "</hv:object-ring>";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

