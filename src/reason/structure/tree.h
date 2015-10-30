
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
#ifndef STRUCTURE_TREE_H
#define STRUCTURE_TREE_H

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "reason/structure/iterator.h"
#include "reason/structure/disposable.h"
#include "reason/structure/array.h"
#include "reason/structure/policy.h"
#include "reason/system/interface.h"
#include "reason/system/timer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Reason { namespace Structure { namespace Abstract {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_>
class Tree
{
public:

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & iterand)=0;

	virtual Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Remove(Iterand<_Kind_> & iterand)=0;

	virtual Iterand<_Kind_> Delete(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Delete(Iterand<_Kind_> & iterand)=0;

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference kind)=0;
	virtual Iterand<_Kind_> Select(Iterand<_Kind_> & iterand)=0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 

class BinaryTree : public Iterable<_Kind_>, public Disposable, public Comparable
{
public:

	static Identity Instance;

	Identity & Identify() {return Instance;}

public:

	class Node: public Variable<_Kind_>, public Reason::System::Disposable
	{
	public:

		virtual typename Type<_Kind_>::Pointer Pointer() {return Variable<_Kind_>::Pointer();}
		virtual typename Type<_Kind_>::Reference Reference() {return Variable<_Kind_>::Reference();}
		virtual typename Template<_Kind_>::Reference operator () (void) {return Variable<_Kind_>::operator ()();}

	public:

		Node * Parent;
		Node * Left;
		Node * Right;

		int Option;

		enum NodeDirection
		{
			DIRECTION_NONE		=0,
			DIRECTION_LEFT		=-1,
			DIRECTION_RIGHT		=1,
		};

		Node(Node * node, int option=0):Variable<_Kind_>(node->Kind),Parent(0),Left(0),Right(0),Option(option)
		{

		}

		Node(typename Template<_Kind_>::ConstantReference type, int option=0):Variable<_Kind_>(type),Parent(0),Left(0),Right(0),Option(option)
		{
		}

		Node():Parent(0),Left(0),Right(0),Option(0)
		{
		}

		virtual ~Node()
		{	
		}

		int Compare(typename Template<_Kind_>::ConstantReference type, int comparitor = COMPARE_GENERAL)
		{
			return Reason::Structure::Comparer<_Kind_>::Compare(this->Kind,type,comparitor);
		}

		int Compare(Iterand<_Kind_> iterand, int comparitor = COMPARE_GENERAL)
		{

			return Reason::Structure::Comparer<_Kind_>::Compare(this->Kind,((Node*)&iterand)->Kind,comparitor);
		}

		int Compare(Node * node, int comparitor = COMPARE_GENERAL)
		{

			return Reason::Structure::Comparer<_Kind_>::Compare(this->Kind,node->Kind,comparitor);
		}

		void Release()
		{
			if (Left)
			{
				Left->Release();
				delete Left;
			}

			if (Right)
			{
				Right->Release();
				delete Right;
			}

			Parent = 0;
			Left = 0;
			Right = 0;
		}

		void Destroy()
		{
			if (Left)
			{
				Left->Destroy();
				delete Left;
			}

			if (Right)
			{
				Right->Destroy();
				delete Right;
			}

			Parent = 0;
			Left = 0;
			Right = 0;

			Reason::Structure::Disposer<_Kind_>::Destroy((*this)());
		}

		bool IsLeft() {return (Parent)?Parent->Left==this:false;}
		bool IsRight() {return (Parent)?Parent->Right==this:false;}

		int Direction() 
		{

			if (!Parent) return 0;
			if (Parent->Left == this) return -1;
			if (Parent->Right == this) return 1;
			return 0;
		}

		Node * Child(int direction)
		{
			return (direction == 0)?0:((direction < 0)?Left:Right);
		}

		Node * Child(Node * node)
		{

			if (node==Left) return Left;
			if (node==Right) return Right;
			return 0;
		}

		Node * Sibling(int direction)
		{
			return (direction == 0)?0:((direction < 0)?Right:Left);
		}

		Node * Sibling(Node * node)
		{

			if (node==Left) return Right;
			if (node==Right) return Left;

			OutputTrap();
			return 0;
		}

		friend class BinaryTree;
	};

	class NodeIteration : public Iteration<_Kind_>
	{
	public:
		enum NodeOrder
		{
			ITERATE_INORDER,
			ITERATE_PREORDER,
			ITERATE_POSTORDER,

			ITERATE_DEPTHFIRST,
			ITERATE_BREADTHFIRST,
		};

		Node * First;
		Node * Last;
		int Order;

		NodeIteration(Node * node, int order=ITERATE_INORDER):Order(order),First(0),Last(0)
		{
		}

		NodeIteration(int order=ITERATE_INORDER):Order(order),First(0),Last(0)
		{
		}

		NodeIteration(Reason::Structure::Iterable<_Kind_> * iterable, int order=ITERATE_INORDER):Iteration<_Kind_>(iterable),Order(order),First(0),Last(0)
		{
		}

		Iteration<_Kind_> * Clone()
		{

			return this;
		}

		void Release(Iterand<_Kind_> & iterand)
		{
			Node * node = (Node*)&iterand;

			if (this->Base)
			{
				this->Base->Dispose(iterand());
			}
			else
			{

				Structure::Disposer<_Kind_>::Release(iterand());
			}

			delete node;
		}

		void Destroy(Iterand<_Kind_> & iterand)
		{
			Node * node = (Node*)&iterand;

			if (this->Base)
			{
				this->Base->Dispose(iterand());
			}
			else
			{

				Structure::Disposer<_Kind_>::Destroy(iterand());
			}

			delete node;
		}

		void Forward(Iterand<_Kind_> & iterand)
		{
			iterand.Forward();
			Node * node = 0;

			if (First)
				node = First;
			else
				switch(Order)
				{
				case NodeIteration::ITERATE_INORDER:
					{
						node = ((BinaryTree*)this->Iterable())->Root;
						while (node && node->Left) 
							node = node->Left;		
					}
					break;
				case NodeIteration::ITERATE_PREORDER:
					{
						node = ((BinaryTree*)this->Iterable())->Root;
					}
					break;
				case NodeIteration::ITERATE_POSTORDER:
					{
						node = ((BinaryTree*)this->Iterable())->Root;
						while (node && (node->Left || node->Right))
						{
							if (node->Left)
								node = node->Left;
							else
							if (node->Right)
								node = node->Right;
						}
					}
				}

			iterand.Variant(node);
		}

		void Reverse(Iterand<_Kind_> & iterand)
		{
			iterand.Reverse();

			Node * node = 0;
			if (Last)
				node = Last;
			else
				switch(Order)
				{
				case NodeIteration::ITERATE_INORDER:
					{
						node = ((BinaryTree*)this->Iterable())->Root;
						while (node && node->Right) 
							node = node->Right;
					}
					break;
				case NodeIteration::ITERATE_PREORDER:
					{
						node = ((BinaryTree*)this->Iterable())->Root;
						while (node && (node->Right || node->Left))
						{
							if (node->Right)
								node = node->Right;
							else
							if (node->Left)
								node = node->Left;
						}
					}
					break;
				case NodeIteration::ITERATE_POSTORDER:
					{
						node = ((BinaryTree*)this->Iterable())->Root;
					}
					break;
				}

			iterand.Variant(node);
		}

		void Move(Iterand<_Kind_> & iterand, int amount)
		{
			if (!Iterable)
			{

				iterand.Option |= Iterand<_Kind_>::STATE_INVALID;
				return;
			}

			amount *= iterand.Move();

			Node * node = (Node*)&iterand;
			if (node)
			{
				if (amount > 0)
				{
					if (node == Last)
						node = 0;

					while(node && node != Last && amount-- > 0)
						node = NodeIteration::Forward(node,Order);		

				}
				else
				if (amount < 0)
				{
					if (node == First)
						node = 0;

					while (node && node != First && amount++ < 0)
						node = NodeIteration::Reverse(node,Order);
				}
			}

			iterand.Variant(node);
		}

		static Node * Forward(Node * node, int order)
		{

			if (order == NodeIteration::ITERATE_INORDER)
			{			

				if (node->Right)
				{
					node = node->Right;
					while (node->Left) 
						node = node->Left;
				}
				else
				if (node->Parent)
				{
					while(node->Parent && node->IsRight())
						node = node->Parent;

					node = node->Parent;
				}
				else
				{
					node = 0;
				}
			}
			else
			if (order == NodeIteration::ITERATE_PREORDER)
			{

				if (node)
				{
					if (node->Left)
					{
						node = node->Left;
					}
					else
					if (node->Right)
					{
						node = node->Right;
					}
					else
					{
						while(node)
						{
							while (node && node->IsRight())
								node = node->Parent;

							if (node && node->IsLeft())
							{
								node = node->Parent;
								if (node->Right)
								{
									node = node->Right;
									break;
								}
							}
							else
							{
								node = 0;
							}
						}

					}
				}
			}
			else
			if (order == NodeIteration::ITERATE_POSTORDER)
			{

				if (node)
				{
					if (node->Parent)
					{
						if (node->IsLeft())
						{
							node = node->Parent;
							if (node->Right)
							{
								node = node->Right;
								while (node->Left || node->Right)
								{
									if (node->Left) 
										node = node->Left;
									else
										node = node->Right;
								}
							}
						}
						else
						if (node->IsRight())
						{
							node = node->Parent;
						}
					}
					else
					{
						node = 0;
					}
				}
			}
			else
			{
				node = 0;
			}	

			return node;	
		}

		static Node * Reverse(Node * node, int order)
		{
			if (order == NodeIteration::ITERATE_INORDER)
			{

				if (node->Left)
				{
					node = node->Left;
					while (node->Right) 
						node = node->Right;
				}
				else
				if (node->Parent)
				{
					while (node->Parent && node->IsLeft())
						node = node->Parent;

					node = node->Parent;
				}
				else
				{
					node = 0;
				}
			}
			else

			if (order == NodeIteration::ITERATE_PREORDER)
			{

				if (node)
				{
					if (node->Parent)
					{
						if (node->IsRight())
						{
							node = node->Parent;
							if (node->Left)
							{
								node = node->Left;
								while (node->Right || node->Left)
								{
									if (node->Right)
										node = node->Right;
									else
										node = node->Left;
								}
							}
						}
						else
						{
							node = node->Parent;
						}
					}
					else
					{
						node = 0;
					}
				}
			}
			else
			if (order == NodeIteration::ITERATE_POSTORDER)
			{

				if (node)
				{
					if (node->Right)
					{
						node = node->Right;
					}
					else
					if (node->Left)
					{
						node = node->Left;
					}
					else
					{
						while(node)
						{
							while (node && node->IsLeft())
								node = node->Parent;

							if (node && node->IsRight())
							{
								node = node->Parent;
								if (node->Left)
								{
									node = node->Left;
									break;
								}
							}
							else
							{
								node = 0;
							}
						}
					}
				}
			}
			else
			{
				node = 0;
			}

			return node;		
		}
	};

	Structure::Iterator<_Kind_> Iterate()
	{
		return Structure::Iterator<_Kind_>(new NodeIteration(this));
	}

	Structure::Iterator<_Kind_> IteratePreorder()
	{
		return Structure::Iterator<_Kind_>(new NodeIteration(this,NodeIteration::ITERATE_PREORDER));
	}

	Structure::Iterator<_Kind_> IteratePostorder()
	{
		return Structure::Iterator<_Kind_>(new NodeIteration(this,NodeIteration::ITERATE_POSTORDER));
	}

	virtual Node * New()
	{
		void * data = Iterable<_Kind_>::New(sizeof(Node));
		return new (data) Node();
	}

	virtual Node * New(typename Template<_Kind_>::ConstantReference kind)
	{
		void * data = Iterable<_Kind_>::New(sizeof(Node));
		return new (data) Node(kind);
	}

	virtual Node * New(Node * node)
	{
		void * data = Iterable<_Kind_>::New(sizeof(Node));
		return new (data) Node(node);
	}

	using Iterable<_Kind_>::Del;
	virtual void Del(Node * node)
	{
		node->~Node();
		Iterable<_Kind_>::Del((void*)node);
	}

	using Iterable<_Kind_>::Compare;
	virtual int Compare(Iterand<_Kind_> & left, Iterand<_Kind_> & right)
	{
		return Iterable<_Kind_>::Compare(left,right);
	}

	virtual int Compare(Node * left, Node * right)
	{
		return Iterable<_Kind_>::Compare((*left)(),(*right)());
	}

	using Iterable<_Kind_>::Dispose;
	virtual void Dispose(Iterand<_Kind_> & iterand)
	{
		Node * node = (Node*) &iterand;
		Dispose(node);
	}

	virtual void Dispose(Node * node)
	{
		 Initial->Base->Dispose((*node)());
		 Del(node);
	}

	virtual void Release(Iterand<_Kind_> & iterand)
	{
		if (iterand.Proxy == 0 && iterand.Iteration && iterand.Iteration->Iterable() == this)
		{
			Reason::Structure::Disposer<_Kind_>::Release(iterand());
			Node * node = (Node*) &iterand;
			Del(node);
		}
	}

	virtual void Destroy(Iterand<_Kind_> & iterand)
	{
		if (iterand.Proxy == 0 && iterand.Iteration && iterand.Iteration->Iterable() == this)
		{
			Reason::Structure::Disposer<_Kind_>::Destroy(iterand());
			Node * node = (Node*) &iterand;
			Del(node);
		}
	}	

public:

	Node * Root;
	int Count;

	BinaryTree(const BinaryTree & tree):
		Iterable<_Kind_>(new _Policy_()),Root(0),Count(0)
	{
		operator = (tree);
	}

	BinaryTree(Iterable<_Kind_> & iterable):
		Iterable<_Kind_>(new _Policy_()),Root(0),Count(0)
	{
		operator = (iterable);
	}

	BinaryTree(typename Reason::Structure::Comparer<_Kind_>::CompareFunction compare):
		Iterable<_Kind_>(new _Policy_()),Root(0),Count(0)
	{
		this->Initial->Base = new Default<_Kind_,CompareCallback<_Kind_> >();
		((CompareCallback<_Kind_> &)this->Initial->Base->Compare).Callback = Compare;

	}	

	BinaryTree(int comparitor):
		Iterable<_Kind_>(new _Policy_()),Root(0),Count(0)
	{
		this->Initial->Base->Compare.Option = comparitor;
	}

	BinaryTree():
		Iterable<_Kind_>(new _Policy_()),Root(0),Count(0)
	{

	}

	~BinaryTree()
	{
		Release();
	}

	BinaryTree & operator = (const Iterable<_Kind_> & iterable)
	{
		Iterand<_Kind_> iterand = ((Iterable<_Kind_> &)iterable).Forward();
		while (iterand != 0)
		{	
			Insert(iterand());
			++iterand;
		}

		return *this;	
	}

	BinaryTree & operator = (const BinaryTree & tree)
	{
		if (&tree == this) return *this;

		this->Initial->Base->Compare.Option = tree.Initial->Base->Compare.Option;

		if (Count == 0)
		{

			Node * from = tree.Root;
			if (from)
			{
				Count = tree.Count;

				Reason::Structure::Array<Node*> arrayFrom;
				Reason::Structure::Array<Node*> arrayTo;		

				Root = New(from);
				Node * to = Root;		

				while(from && to)
				{
					if (from->Right)
					{
						to->Right = New(from->Right);
						arrayTo.Push(to->Right);
						arrayFrom.Push(from->Right);
					}

					if (from->Left)
					{
						to->Left = New(from->Left);
						arrayTo.Push(to->Left);
						arrayFrom.Push(from->Left);
					}	

					if (arrayTo.Size > 0 && arrayFrom.Size > 0)
					{
						to = arrayTo.Pop()();
						from = arrayFrom.Pop()();
					}
					else
					{
						to = 0;
						from = 0;
					}
				}
			}

			return *this;
		}
		else
		{
			Release();
			return operator = ((Iterable<_Kind_>&)tree);
		}

	}

	BinaryTree & operator = (typename Template<_Kind_>::ConstantReference type) {Insert(type);return *this;}
	BinaryTree & operator , (typename Template<_Kind_>::ConstantReference type) {Insert(type);return *this;}

	BinaryTree & operator << (typename Template<_Kind_>::ConstantReference type) {Insert(type);return *this;};

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind, bool unique=false)
	{
		Node * node = New(kind);
		return Insert(Iterand<_Kind_>::Put(node),unique);
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & kind, bool unique=false)
	{
		if (!kind) return Iterand<_Kind_>::Null();

		Node * node = (Node*) this->Put(kind);
		if (!node) node = New(kind());

		Node * place = this->Root;

		if (place == 0)
		{
			this->Root = node;
			++Count;
			return Iterand<_Kind_>(this->Initial,node);
		}

		int result;
		while (place)
		{

			result = rand();
			if (result <= 0)
			{
				if (place->Right == 0)
				{
					place->Right = node;
					node->Parent = place;
					++Count;
					return Iterand<_Kind_>(this->Initial,node);
				}
				else
				{
					place = place->Right;
				}
			}
			else

			{
				if (place->Left == 0)
				{
					place->Left = node;
					node->Parent = place;
					++Count;
					return Iterand<_Kind_>(this->Initial,node);
				}
				else
				{
					place = place->Left;
				}
			}

		}

		return Iterand<_Kind_>::Null();
	}

	virtual void Release()
	{
		if (Root)
		{	
			Root->Release();
			Del(Root);
			Root=0;
			Count=0;
		}
	}

	virtual void Destroy()
	{
		if (Root)
		{
			Root->Destroy();
			Del(Root);
			Root=0;
			Count=0;
		}
	}

	Iterand<_Kind_> Rotate(Iterand<_Kind_> & kind, int amount)
	{

		if (amount == 0 || !kind) return Iterand<_Kind_>::Null();

		Node * node = (Node*)this->Get(kind);
		if (!node) return Iterand<_Kind_>::Null();

		Node * place = 0;

		while (amount)
		{			
			if (amount < 0)		
			{

				place = node->Right;
				if (!place) return Iterand<_Kind_>::Null();

				node->Right = place->Left;
				if (node->Right != 0)
					node->Right->Parent = node;

				place->Parent = node->Parent;
				if (place->Parent)
				{
					if (node == node->Parent->Left) 
						place->Parent->Left = place;
					else
						place->Parent->Right = place;
				}
				else
				{
					this->Root = place;
				}

				place->Left = node;
				node->Parent = place;

				++amount;
			}
			else
			if (amount > 0)		
			{

				place = node->Left;
				if (!place) return Iterand<_Kind_>::Null();

				node->Left = place->Right;
				if (node->Left != 0)
					node->Left->Parent = node;

				place->Parent = node->Parent;
				if (place->Parent)
				{
					if (node == node->Parent->Left) 
						place->Parent->Left = place;
					else
						place->Parent->Right = place;
				}
				else
				{
					this->Root = place;
				}

				place->Right = node;
				node->Parent = place;

				--amount;
			}

		}

		return Iterand<_Kind_>::Get(place);

	}	

};

template<typename _Kind_, typename _Policy_>
Identity BinaryTree<_Kind_,_Policy_>::Instance;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 
class SearchTree : public BinaryTree<_Kind_,_Policy_>
{
public:

	static Identity Instance;

	Identity & Identify() {return Instance;}

public:

	typedef typename BinaryTree<_Kind_>::Node Node;

	using BinaryTree<_Kind_>::Iterate;

	Structure::Iterator<_Kind_> Iterate(int from, int to)
	{	
		Structure::Iterator<_Kind_> iterator(new NodeIteration(this));
		Iterate(iterator,from,to);
		return iterator;
	}

	Structure::Iterator<_Kind_> Iterate(Iterand<_Kind_> & from, Iterand<_Kind_> & to)
	{	
		Structure::Iterator<_Kind_> iterator(new NodeIteration(this));
		Iterate(iterator,from,to);
		return iterator;
	}

protected:

	void Iterate(Structure::Iterator<_Kind_> & iterator, int from, int to)
	{
		if (from < to)
		{
			Structure::Iteration<_Kind_> * iteration = iterator.Iterand.Iteration;

			int index=0;
			for(iterator.Forward();iterator.Has();iterator.Move(),++index)
			{
				if (index == from)
				{
					((NodeIteration*)iteration)->First = (Node*)&iterator.Iterand;
				}
				else
				if (index == to)
				{
					((NodeIteration*)iteration)->Last = (Node*)&iterator.Iterand;
					break;
				}
			}
		}
		else
		{
			iterator = Structure::Iterator<_Kind_>();
		}	
	}

	void Iterate(Structure::Iterator<_Kind_> & iterator, Iterand<_Kind_> & from, Iterand<_Kind_> & to)
	{

		Structure::Iteration<_Kind_> * iteration = iterator.Iterand.Iteration;

		Iterand<_Kind_> first = Select(from);
		Iterand<_Kind_> last = Select(to);

		if (first && last)
		{
			((NodeIteration*)iteration)->First = (Node*)&first;
			((NodeIteration*)iteration)->Last = (Node*)&last;
		}
		else
		{
			iterator = Structure::Iterator<_Kind_>();
		}
	}	

public:

	SearchTree(const SearchTree & tree):BinaryTree<_Kind_>(tree)
	{
	}

	SearchTree(Iterable<_Kind_> & iterable):BinaryTree<_Kind_>(iterable)
	{
	}

	SearchTree(typename Reason::Structure::Comparer<_Kind_>::CompareFunction compare):
		BinaryTree<_Kind_>(compare)
	{
	}	

	SearchTree(int comparitor):BinaryTree<_Kind_>(comparitor) 
	{
	}

	SearchTree()
	{

	}

	~SearchTree()
	{
	}

	SearchTree & operator = (const Iterable<_Kind_> & iterable)
	{
		Iterand<_Kind_> iterand = ((Iterable<_Kind_> &)iterable).Forward();
		while (iterand != 0)
		{	
			Insert(iterand());
			++iterand;
		}

		return *this;	
	}

	SearchTree & operator = (const SearchTree & tree)
	{
		BinaryTree<_Kind_>::operator = (tree);
		return *this;		
	}

	virtual Iterand<_Kind_> Minimum()
	{
		Node * node = this->Root;
		while(node != 0 && node->Left)
			node = node->Left;

		return Iterand<_Kind_>(this->Initial,node);
	}

	virtual Iterand<_Kind_> Maximum()
	{
		Node * node = this->Root;
		while(node != 0 && node->Right)
			node = node->Right;

		return Iterand<_Kind_>(this->Initial,node);
	}

	virtual Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference kind, bool unique=false)
	{
		Node * node = New(kind);
		return Insert(Iterand<_Kind_>::Put(node),unique);
	}

	virtual Iterand<_Kind_> Insert(Iterand<_Kind_> & kind, bool unique=false)
	{

		if (!kind) return Iterand<_Kind_>::Null();

		Node * node = (Node*) this->Put(kind);
		if (!node) node = New(kind());

		Node * place = this->Root;

		if (place == 0)
		{
			this->Root = node;
			++Count;
			return Iterand<_Kind_>(this->Initial,node);
		}

		int result;
		while (place)
		{

			if ((result = this->Compare(place,node)) == 0 && unique)
			{			

				Dispose(node);
				return Iterand<_Kind_>(this->Initial,place).Unique();
			}

			if (result <= 0)
			{
				if (place->Right == 0)
				{
					place->Right = node;
					node->Parent = place;
					++Count;
					return Iterand<_Kind_>(this->Initial,node);
				}
				else
				{
					place = place->Right;
				}
			}
			else

			{
				if (place->Left == 0)
				{
					place->Left = node;
					node->Parent = place;
					++Count;
					return Iterand<_Kind_>(this->Initial,node);
				}
				else
				{
					place = place->Left;
				}
			}

		}

		return Iterand<_Kind_>::Null();
	}

	virtual Iterand<_Kind_> Update(typename Template<_Kind_>::ConstantReference kind)
	{
		Node * node = New(kind);
		return Update(Iterand<_Kind_>::Put(node));
	}

	virtual Iterand<_Kind_> Update(Iterand<_Kind_> & kind)
	{

		Iterand<_Kind_> select = Select(kind);
		if (select)
		{

			Node * update = (Node*)&kind;
			Node * node = (Node*)&select;
			if (node->Parent)
			{
				if (node->Parent->Left == node)
					node->Parent->Left = update;
				else
				if (node->Parent->Right == node)
					node->Parent->Right = update;
			}
			update->Parent = node->Parent;
			update->Left = node->Left;
			update->Right = node->Right;

			Dispose(node);

			return Iterand<_Kind_>(this->Initial, update);

		}
		else
		{
			Iterand<_Kind_> insert = Insert(kind);
			return insert;
		}
	}

	virtual Iterand<_Kind_> Remove(typename Template<_Kind_>::ConstantReference kind)
	{
		Iterand<_Kind_> select = Select(kind);
		if (select)
		{
			Iterand<_Kind_> remove = Remove(select);
			return remove;
		}

		return Iterand<_Kind_>::Null();
	}

    virtual Iterand<_Kind_> Remove(Iterand<_Kind_> & kind)
	{
		if (!kind) return Iterand<_Kind_>::Null();
		Node * node = (Node*)this->Get(kind);
		if (!node) return Remove(kind());

		Node * parent = node->Parent;
		Node * replace=0;

		if (node->Left == 0)
		{
			replace = node->Right;
		}
		else
		if (node->Right == 0)
		{
			replace = node->Left;
		}
		else
		if (node->Left->Right == 0)
		{
			replace = node->Left;
		}
		else
		if (node->Right->Left == 0)
		{
			replace = node->Right;
		}
		else
		{

			replace = node->Right->Left;
			while (replace->Left != 0)
				replace = replace->Left;

			replace->Parent->Left = replace->Right;
			if (replace->Right)
			{
				replace->Right->Parent = replace->Parent;
				replace->Right = 0;			
			}
		}

		if (parent)
		{

			if (parent->Left == node) 
				parent->Left = replace;
			else
				parent->Right = replace;
		}
		else
		{
			this->Root = replace;
		}

		if (replace)
		{		
			replace->Parent = parent;

			if (node->Left && node->Left != replace) 
			{
				replace->Left = node->Left;
				replace->Left->Parent = replace;	
			}

			if (node->Right && node->Right != replace) 
			{
				replace->Right = node->Right;
				replace->Right->Parent = replace;
			}							
		}

		--this->Count;

		node->Parent = node->Left = node->Right = 0;
		return Iterand<_Kind_>(this->Initial,node).Dispose(DISPOSE_POLICY);
	}

	virtual Iterand<_Kind_> Delete(typename Template<_Kind_>::ConstantReference type)
	{
		Iterand<_Kind_> select = Select(type);
		if (select)
		{			
			Iterand<_Kind_> remove = Remove(select);

			remove.Dispose(DISPOSE_DESTROY);
			return remove;
		}

		return Iterand<_Kind_>::Null();
	}

	virtual Iterand<_Kind_> Delete(Iterand<_Kind_> & iterand)
	{
		Iterand<_Kind_> remove = Remove(iterand);
		if (remove)
		{
			remove.Dispose(DISPOSE_DESTROY);
			return remove;
		}

		return Iterand<_Kind_>::Null();
	}

	virtual Iterand<_Kind_> Select(typename Template<_Kind_>::ConstantReference type)
	{

		Iterand<_Kind_> root;
		return Select(root,type);
	}

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> & iterand, typename Template<_Kind_>::ConstantReference kind)
	{

		Node * from = (Node*)Get(iterand);

		Node * node = (from)?from:this->Root;

		int result;
		while (node != 0)
		{
			result = Compare((*node)(),kind);

			if (result == 0)
				break;
			else
			if (result > 0)
				node = (from)?NodeIteration::Reverse(node,NodeIteration::ITERATE_INORDER):node->Left;
			else
			if (result < 0)
				node = (from)?NodeIteration::Forward(node,NodeIteration::ITERATE_INORDER):node->Right;
		}

		return Iterand<_Kind_>(this->Initial,node);
	}

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> & kind)
	{
		if (!kind) return Iterand<_Kind_>::Null();

		Node * node = this->Root;

		int result;
		while (node != 0)
		{
			result = Compare((*node)(),kind());

			if (result == 0)
				break;
			else
			if (result > 0)
				node = node->Left;
			else
			if (result < 0)
				node = node->Right;
		}

		return Iterand<_Kind_>(this->Initial,node);
	}

};

template<typename _Kind_, typename _Policy_>
Identity SearchTree<_Kind_,_Policy_>::Instance;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 
class OrderedTree : public SearchTree<_Kind_,_Policy_>
{
public:

};

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 
class UnorderedTree : public BinaryTree<_Kind_,_Policy_>
{
public:

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 
class RedBlackTree : public SearchTree<_Kind_,_Policy_>
{
public:

	enum NodeColors
	{
		COLOR_RED,
		COLOR_BLACK,
	};

	typedef typename SearchTree<_Kind_>::Node Node;

	using SearchTree<_Kind_,_Policy_>::New;
	virtual Node * New()
	{
		void * data = Iterable<_Kind_>::New(sizeof(Node));
		return new (data) Node();
	}

	virtual Node * New(typename Template<_Kind_>::ConstantReference kind)
	{

		void * data = Iterable<_Kind_>::New(sizeof(Node));
		return new (data) Node(kind);
	}

	virtual Node * New(Node * node)
	{
		void * data = Iterable<_Kind_>::New(sizeof(Node));
		return new (data) Node(node);
	}

public:

	RedBlackTree(const RedBlackTree & tree)
	{
		operator = (tree);
	}

	RedBlackTree(Iterable<_Kind_> & iterable)
	{
		operator = (iterable);
	}

	RedBlackTree(int comparitor=SearchTree<_Kind_>::COMPARE_GENERAL):SearchTree<_Kind_>(comparitor)
	{
	}

	~RedBlackTree()
	{	
	}

	using SearchTree<_Kind_>::operator =;

	Iterand<_Kind_> Insert(typename Template<_Kind_>::ConstantReference type, bool unique=false)
	{
		Node * node = New(type);
		Iterand<_Kind_> insert = Insert(Iterand<_Kind_>::Put(node),unique);
		return insert;
	}

	Iterand<_Kind_> Insert(Iterand<_Kind_> & iterand, bool unique=false)
	{
		Iterand<_Kind_> insert = SearchTree<_Kind_>::Insert(iterand,unique);
		if (insert)
		{
			Node * node = (Node*) &insert;

			if (node->Parent)
			{

				InsertRebalance(node);
			}
			else
			{

				node->Option = COLOR_BLACK;
			}

			return Iterand<_Kind_>(this->Initial,node);
		}

		return insert;
	}

	using SearchTree<_Kind_>::Remove;
	Iterand<_Kind_> Remove(Iterand<_Kind_> & kind)
	{

		if (!kind) return Iterand<_Kind_>::Null();

		Node * node = (Node*)this->Get(kind);
		if (!node) 
		{
			return Remove(kind());
		}

		Node * parent = (Node*)node->Parent;
		Node * replace=0;
		Node * balance=0;
		short color = node->Option;
		int direction=0;

		if (node->Left == 0)
		{
			replace = (Node*)node->Right;
		}
		else
		if (node->Right == 0)
		{
			replace = (Node*)node->Left;
		}
		else
		if (node->Left->Right == 0)
		{
			balance = replace = (Node*)node->Left;
			direction = Node::DIRECTION_LEFT;
		}
		else
		if (node->Right->Left == 0)
		{
			balance = replace = (Node*)node->Right;
			direction = Node::DIRECTION_RIGHT;
		}
		else
		{

			replace = (Node*)node->Right->Left;
			while (replace->Left != 0)
				replace = (Node*)replace->Left;

			balance = (Node*)replace->Parent;
			direction = Node::DIRECTION_LEFT;

			replace->Parent->Left = replace->Right;
			if (replace->Right)
			{
				replace->Right->Parent = replace->Parent;
				replace->Right = 0;
			}
		}

		if (parent && replace)
		{

			if (parent->Left == node) 
				parent->Left = replace;
			else
				parent->Right = replace;			

			replace->Parent = parent;

			if (node->Left && node->Left != replace) 
			{
				replace->Left = node->Left;
				replace->Left->Parent = replace;	
			}

			if (node->Right && node->Right != replace) 
			{
				replace->Right = node->Right;
				replace->Right->Parent = replace;
			}
		}
		else
		if (parent)
		{

			balance = parent;
			direction = node->Direction();
			if (parent->Left == node) 
				parent->Left = 0;
			else
				parent->Right = 0;

			replace = node;
		}
		else
		{

			if (replace)
			{
				replace->Parent = 0;
				if (node->Left && node->Left != replace) 
				{
					replace->Left = node->Left;
					replace->Left->Parent = replace;	
				}

				if (node->Right && node->Right != replace) 
				{
					replace->Right = node->Right;
					replace->Right->Parent = replace;
				}
			}

			this->Root = replace;
		}

		if (replace)
		{			

			if (replace->Option == COLOR_BLACK)
			{
				replace->Option = color;
				RemoveRebalance(balance,direction);
			}
			else
			{
				replace->Option = color;
			}
		}

		--this->Count;	

		node->Parent = node->Left = node->Right = 0;
		return Iterand<_Kind_>(this->Initial,node).Dispose(DISPOSE_POLICY);

	}

private:

	void InsertRebalance(Node * node)
	{

		if (node == 0) return;
		node->Option = COLOR_RED;

		Node * parent = (Node *)node->Parent;
		Node * ancestor	= (Node *)parent->Parent;

		Node * relative	= 0;
		if (ancestor) 
			relative = (Node *)ancestor->Sibling(parent);

		while (node != this->Root && parent != 0 && parent->Option == COLOR_RED && ancestor != 0)
		{

			if (relative == 0 || relative->Option == COLOR_BLACK)
			{

				int nodeDirection = node->Direction();
				int parentDirection = parent->Direction();
				if (nodeDirection != parentDirection)
				{

					this->Rotate(Iterand<_Kind_>::Get(parent), -nodeDirection);

					node = parent;
					parent = (Node *)node->Parent;
				}

				ancestor->Option = COLOR_RED;
				this->Rotate(Iterand<_Kind_>::Get(ancestor), - parentDirection);
				parent->Option = COLOR_BLACK;

			}
			else
			{

				relative->Option = COLOR_BLACK;
				parent->Option = COLOR_BLACK;
				ancestor->Option = COLOR_RED;

				node = ancestor;
			}

			parent = (Node *)node->Parent;

			if (parent) 
				ancestor = (Node *)parent->Parent;
			else
				ancestor = 0;

			if (ancestor) 
				relative = (Node *)ancestor->Sibling(parent);
			else
				relative = 0;

		}

		((Node *)this->Root)->Option = COLOR_BLACK;
	}

	void RemoveRebalance(Node * parent, int direction)
	{

		if (!parent) return;

		Node * node = (Node *)parent->Child(direction);

		Node * sibling = (Node *)parent->Sibling(node);

		while ( node != this->Root && (node == 0 || node->Option == COLOR_BLACK) )
		{
			if (sibling->Option == COLOR_RED)
			{

				sibling->Option = COLOR_BLACK;
				parent->Option = COLOR_RED;
				this->Rotate(Iterand<_Kind_>::Get(parent),direction);

				sibling = (Node *)parent->Sibling(node);
			}

			if ((sibling->Right == 0 && sibling->Left == 0 ) ||
				((sibling->Right != 0 && ((Node*)sibling->Right)->Option == COLOR_BLACK) &&
				 (sibling->Left != 0 && ((Node*)sibling->Left)->Option == COLOR_BLACK)))
			{

				sibling->Option = COLOR_RED;
				node = parent;
			}
			else
			{

				if (sibling->Child(direction) != 0 && (sibling->Child(-direction) == 0 || ((Node *)sibling->Child(-direction))->Option == COLOR_BLACK))
				{

					((Node *)sibling->Child(direction))->Option = COLOR_BLACK;
					sibling->Option = COLOR_RED;
					this->Rotate(Iterand<_Kind_>::Get(sibling),-direction);

					sibling = (Node *)parent->Sibling(node);
				}

				sibling->Option = parent->Option;
				parent->Option = COLOR_BLACK;
				if (sibling->Child(-direction))
				{
					((Node *)sibling->Child(-direction))->Option = COLOR_BLACK;
					this->Rotate(Iterand<_Kind_>::Get(parent),direction);
				}

				node = (Node *)this->Root;
			}

			if ((parent=(Node*)node->Parent) != 0)
			{
				direction = node->Direction();
				sibling	= (parent)?(Node *)parent->Child(-direction):(Node *)0;
			}
		}

		if (node) node->Option = COLOR_BLACK;
	}

public:

	void Verify()
	{

		int depth=0;
		int count=0;
		Verify((Node*)this->Root, depth, count);
		OutputAssert(this->Count==count);
		OutputMessage("RedBlackTree::Verify - Depth %d, Count %d\n",depth,count);
	}

	void Verify(Node * node, int & depth, int & count)
	{
		if (node)
		{
			++count;

			if (((Node *)node)->Option == COLOR_RED)
			{

				OutputAssert(node->Left == 0 || ((Node *)node->Left)->Option == COLOR_BLACK);
				OutputAssert(node->Right == 0 || (((Node *)node->Right)->Option == COLOR_BLACK));
				OutputAssert(node->Parent == 0 || (((Node *)node->Parent)->Option == COLOR_BLACK));
			}
			else
			if (((Node *)node)->Option == COLOR_BLACK)
			{

				if (node->Parent)
				{
					OutputAssert(node->Parent->Sibling(node) != 0);
				}

				++depth;
			}

			int left=0;
			int right=0;

			Verify((Node*)node->Left,left,count);
			Verify((Node*)node->Right,right,count);

			OutputAssert(left == right);

			depth += left;
		}
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 
class AvlTree: public SearchTree<_Kind_,_Policy_>
{
public:

	typedef typename SearchTree<_Kind_>::Node Node;

public:

	using SearchTree<_Kind_>::Insert;
	Iterand<_Kind_> Insert(Iterand<_Kind_> & iterand, bool unique=false)
	{
		Iterand<_Kind_> insert = SearchTree<_Kind_>::Insert(iterand,unique);
		if (insert)
		{
			Node * heights = (Node*) &insert;
			HeightsRebalance(heights);
		}

		return insert;
	}

	using SearchTree<_Kind_>::Remove;
	Iterand<_Kind_> Remove(Iterand<_Kind_> & kind)
	{

		if (!kind) return Iterand<_Kind_>::Null();

		Node * node = (Node*)this->Get(kind);
		if (!node) return Remove(kind());

		Node * heights = node->Parent;
		int option = node->Option;

		Node * parent = node->Parent;
		Node * replace=0;

		if (node->Left == 0)
		{
			replace = node->Right;
		}
		else
		if (node->Right == 0)
		{
			replace = node->Left;
		}
		else
		if (node->Left->Right == 0)
		{
			replace = node->Left;
		}
		else
		if (node->Right->Left == 0)
		{
			replace = node->Right;
		}
		else
		{
			replace = node->Right->Left;
			while (replace->Left != 0)
				replace = replace->Left;

			replace->Parent->Left = replace->Right;
			if (replace->Right)
			{
				replace->Right->Parent = replace->Parent;
				replace->Right = 0;			
			}
		}

		if (parent)
		{
			if (parent->Left == node) 
				parent->Left = replace;
			else
				parent->Right = replace;
		}
		else
		{
			this->Root = replace;
		}

		if (replace)
		{	

			if (node != replace->Parent)
			{
				heights = replace->Parent;
				replace->Option = option;
			}
			else
			{
				heights = replace;
			}

			replace->Parent = parent;

			if (node->Left && node->Left != replace) 
			{
				replace->Left = node->Left;
				replace->Left->Parent = replace;	
			}

			if (node->Right && node->Right != replace) 
			{
				replace->Right = node->Right;
				replace->Right->Parent = replace;
			}

		}

		--this->Count;

		HeightsRebalance(heights);	

		node->Parent = node->Left = node->Right = 0;
		return Iterand<_Kind_>(this->Initial,node).Dispose(DISPOSE_POLICY);
	}

protected:

	int Heights(Node * node, int & left, int & right)
	{

		left = right = 0;

		if (node->Left)
		{
			left = node->Left->Option;
			if (node->Right)
			{
				right = node->Right->Option;
				return 1 + max(left,right);
			}
			return 1 + left;
		}
		else
		if (node->Right)
		{
			right = node->Right->Option;
			if (node->Left)
			{
				left = node->Left->Option;
				return 1 + max(left,right);
			}
			return 1 + right;
		}

		return 0;	
	}

	void HeightsRebalance(Node * node)
	{
		int left = 0, right = 0;
		Iterand<_Kind_> rotate;
		Node * sibling;

		while (node)
		{
			node->Option = Heights(node,left,right);			

			int direction = 0;		
			int diff = left - right;
			if (diff > 1)
				direction = Node::DIRECTION_RIGHT;
			else 
			if (diff < -1)
				direction = Node::DIRECTION_LEFT;

			if (direction)			
			{

				sibling = node->Sibling(direction);

				Heights(sibling,left,right);

				if (direction == Node::DIRECTION_RIGHT && right > left ||
					direction == Node::DIRECTION_LEFT && left > right)
				{
					rotate = Iterand<_Kind_>::Get(sibling);
					Rotate(rotate,sibling->Direction());
					sibling->Option = Heights(sibling,left,right);
					sibling = node->Sibling(direction);
					sibling->Option = Heights(sibling,left,right);
				}

				rotate = Iterand<_Kind_>::Get(node);

				Rotate(rotate,direction);				
			}
			else
			{
				node = node->Parent;	
			}		
		}

	}

public:

	void Verify()
	{

		int depth=0;
		int count=0;
		int avl = Verify((Node*)this->Root, depth, count);
		OutputAssert(this->Count==count);
		OutputAssert(depth == 0 || avl == depth-1);
		OutputMessage("AvlTree::Verify - Avl %d, Depth %d, Count %d\n",avl,depth,count);
	}

	int Verify(Node * node, int & depth, int & count)
	{
		if (!node) return 0;

		++count;

		if (node->Left || node->Right)
		{

			int left=0;
			int right=0;

			int avlleft = Verify(node->Left,left,count);
			int avlright = Verify(node->Right,right,count);

			OutputAssert(abs(avlleft-avlright) <= 1);

			depth = 1 + max(left,right);

			return 1 + max(avlleft,avlright);
		}
		else
		{
			++depth;
			return 0;
		}
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_, typename _Policy_=Default<_Kind_> > 
class Treap : public SearchTree<_Kind_,_Policy_>
{
public:

	typedef typename SearchTree<_Kind_,_Policy_>::Node Node;

public:

	enum HeapTypes
	{
		HEAP_MIN,
		HEAP_MAX
	};

	int Heap;

	Treap(int heap=HEAP_MAX):Heap(heap)
	{

	}

	using SearchTree<_Kind_>::Insert;
	Iterand<_Kind_> Insert(Iterand<_Kind_> & iterand, bool unique=false)
	{
		Iterand<_Kind_> insert = SearchTree<_Kind_>::Insert(iterand,unique);
		if (insert)
		{
			Node * node = (Node*) &insert;
			Heapify(node);
		}

		return insert;
	}	

public:

	void Heapify(Node * node)
	{
		if (!node)
			return;

	}

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename _Kind_> 
class SplayTree : public SearchTree<_Kind_>
{
public:

	typedef typename SearchTree<_Kind_>::Node Node;

public:

	enum SplayTypes
	{
		SPLAY_DEFAULT,
		SPLAY_LAZY,
		SPLAY_DELTA,
		SPLAY_WEIGHT,	
	};

	enum SplayTarget
	{
		TARGET_INSERT,
		TARGET_REMOVE,
		TARGET_SELECT,
	};

	int Strategy;

	SplayTree(int strategy=SPLAY_DEFAULT):Strategy(strategy)
	{	
	}

	using SearchTree<_Kind_>::Insert;
	Iterand<_Kind_> Insert(Iterand<_Kind_> & iterand, bool unique=false)
	{
		Iterand<_Kind_> insert = SearchTree<_Kind_>::Insert(iterand,unique);
		if (insert)
		{
			Node * node = (Node*) &insert;
			Splay(node,TARGET_INSERT);
		}

		return insert;
	}

	using SearchTree<_Kind_>::Remove;
	Iterand<_Kind_> Remove(Iterand<_Kind_> & kind)
	{

		if (!kind) return Iterand<_Kind_>::Null();

		Node * node = (Node*)this->Get(kind);
		if (!node) return Remove(kind());

		Node * splay = node->Parent;
		int option = node->Option;

		Node * parent = node->Parent;
		Node * replace=0;

		if (node->Left == 0)
		{
			replace = node->Right;
		}
		else
		if (node->Right == 0)
		{
			replace = node->Left;
		}
		else
		if (node->Left->Right == 0)
		{
			replace = node->Left;
		}
		else
		if (node->Right->Left == 0)
		{
			replace = node->Right;
		}
		else
		{
			replace = node->Right->Left;
			while (replace->Left != 0)
				replace = replace->Left;

			replace->Parent->Left = replace->Right;
			if (replace->Right)
			{
				replace->Right->Parent = replace->Parent;
				replace->Right = 0;			
			}
		}

		if (parent)
		{
			if (parent->Left == node) 
				parent->Left = replace;
			else
				parent->Right = replace;
		}
		else
		{
			this->Root = replace;
		}

		if (replace)
		{	

			if (node != replace->Parent)
			{
				splay = replace->Parent;
				replace->Option = option;
			}
			else
			{
				splay = replace;
			}

			replace->Parent = parent;

			if (node->Left && node->Left != replace) 
			{
				replace->Left = node->Left;
				replace->Left->Parent = replace;	
			}

			if (node->Right && node->Right != replace) 
			{
				replace->Right = node->Right;
				replace->Right->Parent = replace;
			}

		}

		--this->Count;

		if (Strategy != SPLAY_WEIGHT)
			Splay(parent,TARGET_REMOVE);
		else
			Splay(splay,TARGET_REMOVE);	

		node->Parent = node->Left = node->Right = 0;
		return Iterand<_Kind_>(this->Initial,node).Dispose(DISPOSE_POLICY);	
	}

	using SearchTree<_Kind_>::Select;
	virtual Iterand<_Kind_> Select(Iterand<_Kind_> & from, typename Template<_Kind_>::ConstantReference kind)
	{
		Iterand<_Kind_> select = SearchTree<_Kind_>::Select(from,kind);
		if (select)
		{
			Node * splay = (Node*) &select;
			Splay(splay,TARGET_SELECT);
		}

		return select;
	}

	virtual Iterand<_Kind_> Select(Iterand<_Kind_> & kind)
	{
		Iterand<_Kind_> select = SearchTree<_Kind_>::Select(kind);
		if (select)
		{
			Node * splay = (Node*) &select;
			Splay(splay,TARGET_SELECT);
		}

		return select;	
	}	

protected:

	int Weights(Node * node, int & left, int & right)
	{

		left = right = 0;

		if (node->Left)
		{
			left = node->Left->Option;
			if (node->Right)
			{
				right = node->Right->Option;
				return 1 + left + right;
			}
			return 1 + left;
		}
		else
		if (node->Right)
		{
			right = node->Right->Option;
			if (node->Left)
			{
				left = node->Left->Option;
				return 1 + left + right;
			}
			return 1 + right;
		}

		return 1;	
	}

	void Splay(Node * node, int target)
	{
		if (!node)
			return;

		switch (Strategy)
		{
			case SPLAY_LAZY:
			{

				if (target == TARGET_SELECT)
					return;
			}
			case SPLAY_DEFAULT:
			{
				Iterand<_Kind_> rotate;
				Node * parent;
				int direction = 0;

				while (node)
				{
					parent = node->Parent;		
					if (!parent)
						break;

					direction = node->Direction();

					if (parent != this->Root && parent->Direction() == direction)
					{

						rotate = Iterand<_Kind_>::Get(parent->Parent);
						Rotate(rotate,-direction);					
					}

					rotate = Iterand<_Kind_>::Get(parent);
					if (!Rotate(rotate,-direction))
						node = parent;
				}	

			}
			break;
			case SPLAY_DELTA:
			{

				int milliseconds = Timer::Milliseconds();
				float ndelta = (node->Option)?(milliseconds - node->Option):0.0;
				node->Option = milliseconds;

				Iterand<_Kind_> rotate;
				Node * parent;

				int direction;
				float delta;

				while (node)
				{
					parent = node->Parent;		
					if (!parent)
						break;	

					direction = node->Direction();

					delta = 0.0;

					{

						float pdelta = node->Option - parent->Option;

							delta = ndelta / pdelta;	
					}

					if (delta > 0.0 && delta < (float)0.5)
					{	

						if (parent != this->Root && parent->Direction() == direction)
						{
							rotate = Iterand<_Kind_>::Get(parent->Parent);
							Rotate(rotate,-direction);					
						}						

						rotate = Iterand<_Kind_>::Get(parent);
						if (!Rotate(rotate,-direction))

							node = parent;
					}
					else
					{
						node = parent;					

					}
				}

			}
			break;
			case SPLAY_WEIGHT:
			{

				static float log2 = log(2.0);
				int height = (int) log((float)this->Count)/log2;

				int minimum = height;

				int maximum = height<<1;

				int left = 0, right = 0;

				Iterand<_Kind_> rotate;

				Node * grand;
				Node * parent;
				Node * sibling;

				int direction = 0;
				int diff = 0;

				while (node)
				{					

					node->Option = Weights(node,left,right);	

					parent = node->Parent;
					if (!parent)
						break;

					direction = node->Direction();

					sibling = parent->Sibling(direction);										
					diff = (sibling)?sibling->Option - node->Option:-node->Option;

					diff = abs(diff);

					if (target != TARGET_INSERT || diff < maximum)

					{																				

						if (diff > minimum)

						{																			

							if (direction == Node::DIRECTION_LEFT && right > left ||
								direction == Node::DIRECTION_RIGHT && left > right)
							{

								rotate = Iterand<_Kind_>::Get(node);
								Rotate(rotate,direction);
								node->Option = Weights(node,left,right);

								node = node->Parent;
								node->Option = Weights(node,left,right);
							}

						}

						rotate = Iterand<_Kind_>::Get(parent);
						if (Rotate(rotate,-direction))
						{

							parent->Option = Weights(parent,left,right);

						}
						else
						{
							node = parent;
						}

					}
					else
					{

						node = parent;					
					}

				}	
			}
		}

	}		

public:

	void Verify()
	{

		int depth=0;
		int count=0;
		int splay = Verify((Node*)this->Root, depth, count);
		OutputAssert(this->Count==count);
		OutputMessage("SplayTree::Verify - Splay %d, Depth %d, Count %d\n",splay,depth,count);
	}

	int Verify(Node * node, int & depth, int & count)
	{
		if (!node) return 0;

		++count;

		if (node->Left || node->Right)
		{

			int left=0;
			int right=0;

			int splayleft = Verify(node->Left,left,count);
			int splayright = Verify(node->Right,right,count);

			depth = 1 + max(left,right);

			return node->Option;
		}
		else
		{
			++depth;
			return node->Option;
		}
	}	

};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}}}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

