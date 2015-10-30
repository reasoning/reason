
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
#ifndef SYSTEM_INDEX_H
#define SYSTEM_INDEX_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/system/string.h"
#include "reason/system/number.h"
#include "reason/generic/generic.h"
#include "reason/structure/iterator.h"
#include "reason/system/interface.h"
#include "reason/structure/list.h"
#include "reason/system/object.h"

using namespace Reason::Generic;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Structure {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Key_=Substring>
class Index : public Object
{

public:

	virtual Identity & Identify(){return Instance;};

	_Key_ Key;
	Reason::Structure::List<_Kind_> List;

	Index<_Kind_,_Key_> * Left;
	Index<_Kind_,_Key_> * Parent;
	Index<_Kind_,_Key_> * Right;

	Shared< Index<_Kind_,_Key_> * > Ancestor;		
	Index<_Kind_,_Key_> * Descendant;			

	Index(const Index & index):
		Left(0),Right(0),Parent(0),Descendant(0),Ancestor(0)
	{

	}

	Index():
		Left(0),Right(0),Parent(0),Descendant(0),Ancestor(0)
	{

	}

	~Index()
	{

		Destroy();
	}

	Index<_Kind_,_Key_> *Attach(typename Template<_Kind_>::ConstantReference kind, char * data, int size, bool caseless = false)
	{
		Substring substring(data,size);
		return Attach(kind,substring,caseless);
	}

	Index<_Kind_,_Key_> *Attach(typename Template<_Kind_>::ConstantReference kind, const char * data, bool caseless = false)
	{
		Substring substring(data);
		return Attach(kind,substring,caseless);
	}

	Index<_Kind_,_Key_> *Attach(typename Template<_Kind_>::ConstantReference kind, const Sequence &sequence, bool caseless = false)
	{

		Index<_Kind_,_Key_> *index=this;
		Substring substring = sequence;

		while (!substring.IsEmpty())
		{
			if (index->Key.IsEmpty())
			{
				index->Key = substring;

			}

			int dif = (caseless)?Character::Caseless(substring.CharAt(0))-Character::Caseless(index->Key.CharAt(0)):substring.CharAt(0)-index->Key.CharAt(0);
			if (dif == 0)
			{
				if (index->Key.Is(substring,caseless))
				{
					index->List.Append(kind);
					return index;
				}
				else
				if (substring.StartsWith(index->Key,caseless))
				{
					if (index->Descendant == 0)
					{
						index->Descendant = new Index<_Kind_,_Key_>();
						index->Descendant->Ancestor = index;
					}

					substring.Data += index->Key.Size;
					substring.Size -= index->Key.Size;

					index = index->Descendant;
				}
				else
				{

					int i = 0;
					int l = Number::Min(index->Key.Size,substring.Size);
					for (i=0; i < l && Character::Equals(index->Key.CharAt(i),substring.CharAt(i),caseless);++i);

					if (i < index->Key.Size)
					{

						Index * descendant = new Index();

						if (index->Descendant)
						{
							descendant->Descendant = index->Descendant;

							index->Descendant->Ancestor = descendant;
						}

						index->Descendant = descendant;
						descendant->Ancestor = index;

						descendant->List.Take(index->List);

						descendant->Key = _Key_(index->Key,i);
						index->Key = _Key_(index->Key,0,i);

					}
					else
					{

						if (index->Descendant == 0)
						{
							index->Descendant = new Index<_Kind_,_Key_>();
							index->Descendant->Ancestor = index;
						}

						substring.Data += i;
						substring.Size -= i;

						index = index->Descendant;
					}
				}
			}
			else
			{
				if (dif > 0)
				{

					if (index->Right == 0)
					{
						index->Right = new Index<_Kind_,_Key_>();
						index->Right->Ancestor = index->Ancestor;
						index->Right->Parent = index;
					}

					index = index->Right;
				}
				else

				{

					if (index->Left == 0)
					{
						index->Left = new Index<_Kind_,_Key_>();
						index->Left->Ancestor = index->Ancestor;
						index->Left->Parent = index;
					}

					index = index->Left;
				}
			}
		}

		return 0;
	}

	Index<_Kind_,_Key_> *Detach(Sequence &sequence, bool caseless = false)
	{

	}

	Index<_Kind_,_Key_> *Detach( char *data, int size, bool caseless = false)
	{
		Substring substring(data,size);
		return Detach(substring,caseless);
	}

	Index<_Kind_,_Key_> *Detach(const char * size, bool caseless = false)
	{
		Substring substring(size);
		return Detach(substring,caseless);
	}

	Index<_Kind_,_Key_> *Select(char *data, int size, bool caseless = false)
	{
		Substring substring(data,size);
		return Select(substring,caseless);
	}

	Index<_Kind_,_Key_> *Select(const char *data, bool caseless = false)
	{
		Substring substring(data);
		return Select(substring,caseless);
	}

	Index<_Kind_,_Key_> *Select(Sequence &sequence, bool caseless = false)
	{
		if (Key.Size == 0) return 0;
		if (sequence.Size == 0) return 0;

		Index<_Kind_,_Key_> *index=this;
		Substring substring = sequence;

		while (index)
		{
			int i = 0;
			int l = Number::Min(index->Key.Size,substring.Size);
			for (i=0; i < l && Character::Equals(index->Key.CharAt(i),substring.CharAt(i),caseless);++i);
			int dif = (caseless)?Character::Caseless(substring.CharAt(0))-Character::Caseless(index->Key.CharAt(0)):substring.CharAt(0)-index->Key.CharAt(0);

			if (dif == 0)
			{
				if (index->Key.Is(substring,caseless))
				{
					return index;
				}
				else
				if (index->Descendant)
				{
					substring.Data += i;
					substring.Size -= i;

					index = index->Descendant;
				}
				else
				{
					break;
				}
			}
			else
			if (dif > 0)
			{
				if (index->Right != 0)
				{
					index = index->Right;
				}
				else
				{
					break;
				}
			}
			else

			{
				if (index->Left != 0)
				{
					index = index->Left;
				}
				else
				{
					break;
				}
			}
		}

		return 0;
	}

	using Object::Print;
	void Print(String & string)
	{

		Index<_Kind_,_Key_> * index = this;

		string >> index->Key;

		while (index->Ancestor != 0)
		{
			index = index->Ancestor;
			string >> index->Key;
		}
	}

	Reason::System::Superstring Superstring()
	{
		Reason::System::Superstring superstring;
		Print(superstring);
		return superstring;
	}

	void Destroy()
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
		List.Release();
	}

	Index<_Kind_,_Key_> * FirstSibling()
	{

		Index<_Kind_,_Key_> * index = this;
		Index<_Kind_,_Key_> * first;
		while ((first = index->PrevSibling()) != 0)
		{
			index = first;
		}

		return index;
	}

	Index<_Kind_,_Key_> * LastSibling()
	{

		Index * index = this;
		Index * last;
		while ((last = index->NextSibling()) != 0)
		{
			index = last;
		}

		return index;
	}

	Index<_Kind_,_Key_> * FirstDescendant()
	{

		Index<_Kind_,_Key_> * index = this->Descendant;
		if (index)
		{
			index = index->FirstSibling();
		}
		return index;
	}

	Index<_Kind_,_Key_> * LastDescendant()
	{

		Index<_Kind_,_Key_> * index = this->Descendant;
		if (index)
		{
			index = index->LastSibling();
		}
		return index;
	}

	Index<_Kind_,_Key_> * NextSibling()
	{

		Index<_Kind_,_Key_> * index = this;

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

	Index<_Kind_,_Key_> * PrevSibling()
	{

		Index<_Kind_,_Key_> * index = this;

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
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Key_=Substring>
class Catalogue : public Reason::Structure::Index<_Kind_,_Key_>, public Reason::Structure::Iterable< Reason::Structure::Index<_Kind_,_Key_> >
{
public:

	static Identity Instance;

	Identity & Identify() {return Instance;}

public:

	int		Count;			
	int		Occurrences;	

	Catalogue():Count(0),Occurrences(0)
	{
	}

	~Catalogue()
	{
	}

	using Reason::Structure::Index<_Kind_,_Key_>::Attach;
	Reason::Structure::Index<_Kind_,_Key_> * Attach(typename Template<_Kind_>::ConstantReference kind, const Sequence &sequence, bool caseless = false)
	{
		Reason::Structure::Index<_Kind_,_Key_> *index = Index<_Kind_,_Key_>::Attach(kind, sequence,caseless);
		if (index)
		{
			if (index->List.Count == 1)
				++Count;

			++Occurrences;
		}

		return index;
	}

	void Destroy()
	{
		Index<_Kind_,_Key_>::Destroy();
		Count =0;
		Occurrences =0;
	}

public:

	class CatalogueIteration : public Reason::Structure::Iteration< Reason::Structure::Index<_Kind_,_Key_> >
	{
	public:

		CatalogueIteration() {}
		CatalogueIteration(Reason::Structure::Catalogue<_Kind_,_Key_> * catalogue):Reason::Structure::Iteration< Reason::Structure::Index<_Kind_,_Key_> >(catalogue) {}

		Iteration< Reason::Structure::Index<_Kind_,_Key_> > * Clone()
		{
			CatalogueIteration * iteration = new CatalogueIteration();
			*iteration = *this;
			return iteration;
		}

		void Forward(Iterand< Reason::Structure::Index<_Kind_,_Key_> > & iterand)
		{
			iterand.Forward();

			Reason::Structure::Catalogue<_Kind_,_Key_> * catalogue = (Reason::Structure::Catalogue<_Kind_,_Key_> *)this->Iterable();
			Reason::Structure::Index<_Kind_,_Key_> * index = (Reason::Structure::Index<_Kind_,_Key_> *)catalogue;
			while (index)
			{
				while (index->Left)
					index = index->Left;

				if (index->List.Count != 0)
					break;

				if (index->Descendant)
				{
					index = index->Descendant;
				}
				else
				{
					index = 0;
					break;
				}
			}

			iterand.Kind(index);
		}

		void Reverse(Iterand< Reason::Structure::Index<_Kind_,_Key_> > & iterand)
		{
			iterand.Option = Iterand< Reason::Structure::Index<_Kind_,_Key_> >::MODE_REVERSE;

			Reason::Structure::Catalogue<_Kind_,_Key_> * catalogue = (Reason::Structure::Catalogue<_Kind_,_Key_> *)this->Iterable();
			Reason::Structure::Index<_Kind_,_Key_> * index = (Reason::Structure::Index<_Kind_,_Key_> *)catalogue;
			while (index)
			{
				while (index->Left)
					index = index->Left;

				if (index->List.Count != 0)
					break;

				if (index->Descendant)
				{
					index = index->Descendant;
				}
				else
				{
					index = 0;
					break;
				}
			}

			iterand.Kind(index);
		}

		void Move(Iterand< Reason::Structure::Index<_Kind_,_Key_> > & iterand, int amount)
		{
			amount *= iterand.Move();

			Reason::Structure::Index<_Kind_,_Key_> * index = (Reason::Structure::Index<_Kind_,_Key_> *)&iterand;
			if (index)
			{
				if (amount > 0)
				{
					while(index)
					{
						if (index->Descendant)
						{
							index = index->FirstDescendant();
						}
						else
						{
							Reason::Structure::Index<_Kind_,_Key_> * next = index->NextSibling();
							if (next)
							{
								index = next;
							}
							else
							{
								while (index)
								{
									index = index->Ancestor;
									if (index && (next=index->NextSibling()) != 0)
									{
										index = next;
										break;
									}
								}
							}
						}

						if (index && !index->List.IsEmpty())
							break;
					}
				}
				else
				if (amount < 0)
				{

					while(index)
					{
						if (index->Descendant)
						{
							index = index->LastDescendant();
						}
						else
						{
							Reason::Structure::Index<_Kind_,_Key_> * prev = index->PrevSibling();
							if (prev)
							{
								index = prev;
							}
							else
							{
								while (index)
								{

									index = index->Ancestor;
									if (index && (prev=index->PrevSibling()) != 0)
									{
										index = prev;
										break;
									}
								}
							}
						}

						if (index && !index->List.IsEmpty())
							break;
					}
				}
			}

			iterand.Kind(index);
		}
	};

	Structure::Iterator< Reason::Structure::Index<_Kind_,_Key_> > Iterate()
	{
		return Structure::Iterator< Reason::Structure::Index<_Kind_,_Key_> >(new CatalogueIteration(this));
	}

};

template <typename _Kind_, typename _Key_>
Identity Catalogue<_Kind_,_Key_>::Instance;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

