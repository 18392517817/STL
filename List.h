#pragma once
#include<iostream>
using namespace std;
#include"Alloc.h"
#include"Construct.h"
template<class T>
struct __List_Node
{
	/*typedef __List_Node<T>* Pointer;
	Pointer _prev;
	Pointer _next;
	T _data;

	__List_Node()
		:_next(NULL)
		,_prev(NULL)
	{}
	__List_Node(const T&x)
		:_data(x)
		,_next(NULL)
		,_prev(NULL)
	{}*/

	typedef void* Void_Pointer;
	Void_Pointer _next;
	Void_Pointer _prev;
	T _data;
};

template<class T,class Ref,class Ptr>
class __List_Iterator
{
public:
	
	typedef __List_Iterator<T,T&,T*>   Iterator;
	typedef __List_Iterator<T, Ref, Ptr> Self;

	typedef  Bidirectional_Iterator_Tag Iterator_Category;
	typedef T Value_Type;
	typedef Ptr Pointer;
	typedef Ref Reference;
	typedef ptrdiff_t Difference_Type;
	typedef size_t Size_Type;

	typedef __List_Node<T>* Link_Type;

	Link_Type _node;//普通指针，指向list节点
	
	//constructor()
	__List_Iterator(){}
	__List_Iterator(Link_Type x):_node(x){}
    __List_Iterator(const Iterator& x):_node(x._node){}
	//__List_Iterator(const Link_Type&x) :_node(x){}
	/*
	__List_Iterator(Link_Type node=NULL)
		:_node(node)
	{}*/
	bool operator ==(const Self&x)const
	{
		return _node == x._node;
	}
	bool operator !=(const Self&x)const
	{
		return _node != x._node;
	}

	Reference operator*()const
	{
		return _node->_data;
		//return (*_node)._data;
	}
	Pointer operator->()const
	{
		return &(operator*());
	}
	Self & operator++()
	{
		_node = (Link_Type)_node->_next;
		return *this;

		/*_node = (Link_Type)((*_node)._next);
		return *this;*/
	}
	Self & operator++(int)
	{
		Self tmp(_node);
		_node =(Link_Type) _node->_next;
		return tmp;
	}
	Self& operator--()
	{
		_node = (Link_Type)_node->_prev;
		return *this;
	}
	Self& operator--(int)
	{
		Self tmp(_node);
		_node = _node->_prev;
		return tmp;
	}


};

template<class T,class Alloc=Malloc_Alloc>
class List
{ 
protected:
	typedef void* Void_Pointer;
	typedef __List_Node<T> List_Node;
	typedef Simple_Alloc<List_Node, Alloc> List_Node_Allocator;
public:
  
	typedef T Value_Type;
	typedef Value_Type * Pointer;
	typedef size_t Size_Type;
	typedef Value_Type & Reference;
	typedef ptrdiff_t Difference_Type;
	typedef List_Node* Link_Type;
public:
	typedef __List_Iterator<T, T&, T*> Iterator;
protected:
	Link_Type _node;//只要一个指针，就可以表示环双向链表

protected:
	//配置一个节点并回传
	Link_Type Get_Node(){ return List_Node_Allocator::Allocate(); }
	//释放一个节点
	void Put_Node(Link_Type p){ List_Node_Allocator::Deallocate(p); }
	
	//产生（配置并构造）一个节点，带有元素值
	Link_Type Create_Node(const T&x)
	{
		Link_Type p = Get_Node();
		Construct(&p->_data,x);
		return p;
	}
	//销毁（析构并释放）一个节点
	void Destroy_Node(Link_Type p)
	{
		Destroy(&p->_data);
		Put_Node(p);
	}
protected:
	//产生空节点
	void Empty_Initialize()
	{
		_node = Get_Node();
		_node->_next = _node;
		_node->_prev = _node;
	}
	 
public:
	List(){ Empty_Initialize(); }//参数空链表
 
	~List()
	{
		Clear();
	}
public:
	/*Iterator Insert(Iterator pos,const T &x)
	{
		Link_Type tmp = new __List_Node<T>(x);
		Link_Type prev = pos._node->_prev;//--pos
		Link_Type cur = pos._node;

		prev->_next = tmp;
		tmp->_prev = prev;

		tmp->_next = cur;
		cur->_prev = tmp;

		//return  tmp ;
		return Iterator(tmp);

	}*/
	Iterator Insert(Iterator pos, const T &x)
	{
		Link_Type tmp = Create_Node(x);
		//调整双向指针，是tmp插进去
		tmp->_next = pos._node ;
		tmp->_prev = pos._node->_prev;
		(Link_Type(pos._node->_prev))->_next = tmp;
		pos._node->_prev = tmp;

		return tmp;
	}
	/*Iterator Erase(Iterator pos)
	{
		assert(pos!=End());

		Link_Type prev = pos._node->_prev;
		Link_Type next = pos._node->_next;

		prev->_next = next;
		next->_prev = prev;

		delete pos._node;

		return Iterator(next);
	}*/
	Iterator Erase(Iterator position)
	{
		Link_Type next_node = Link_Type(position._node->_next);
		Link_Type prev_node = Link_Type(position._node->_prev);
		prev_node->_next = next_node;
		next_node->_prev = prev_node;
		Destroy_Node(position._node);
		return Iterator(next_node);
	}

	void PushBack(const T& x){	Insert(End(),x);}
	void PushFront(const T&x){	Insert(Begin(),x);}
	void PopBack(){	Erase(--End());	}
	void PopFront()	{Erase(Begin());}

	Iterator Begin(){return (Link_Type)((*_node)._next);}
	Iterator End(){	return  _node;}
	bool Empty()const{ return _node->_next == _node; }
	Reference Front(){ return *Begin(); }
	Reference Back(){ return *(--End()); }
	Size_Type Size()//const
	{
		Size_Type result = 0;
		result=Distance(Begin(),End());
		return result;
	}
	/*void Clear()
	{
		Iterator begin = Begin();
		while (begin!=End())
		{
			Link_Type tmp = begin._node;
			++begin;
			delete tmp;
		}
	}*/
	//清除所有节点
	void Clear()
	{
		Link_Type cur = (Link_Type)_node->_next;//Begin()
		while (cur!=_node)
		{
			Link_Type tmp = cur;
			cur = (Link_Type)cur->_next;
			Destroy_Node(tmp);
		}
		//恢复_node
		_node->_next = _node;
		_node->_prev = _node;
	}
public:
	//将数值为value所以元素删除
	void Remove(const T& value)
	{
		Iterator first = Begin();
		Iterator last = End();
		while (first!=last)
		{
			Iterator next = first;
			++next;
			if (*first == value)
				Erase(first);
			first = next;
		}
	}
	//移除数值相同的连续元素，仅保留一个
	void Unique()
	{
		Iterator first = Begin();
		Iterator last = End();
		while (first == last)return;

		Iterator next = first;
		while (++next!=last)
		{
			if (*first == *next)
				Erase(next);
			else
				first = next;//调整指针
			next = first;//修正区段范围
		}
	}


	 //将x合并到*this身上，前提是两个List都已经有序
	void  Merge(List<T, Alloc>& x)
	{
		Iterator first1 = Begin();
		Iterator last1 = End();
		Iterator first2 = x.Begin();
		Iterator last2 = x.End();
		while (first1 != last1 && first2 != last2)
		if (*first2 < *first1) 
		{
			Iterator next = first2;
			Transfer(first1, first2, ++next);
			first2 = next;
		}
		else
			++first1;
		if (first2 != last2)
			Transfer(last1, first2, last2);
	}

protected:
	//将[first,last)内所有元素移动到postition之前
	void Transfer(Iterator position, Iterator first, Iterator last) 
	{
		if (position != last)
		{
			(*(Link_Type((*last._node)._prev)))._next = position._node;
			(*(Link_Type((*first._node)._prev)))._next = last._node;
			(*(Link_Type((*position._node)._prev)))._next = first._node;
			Link_Type tmp = Link_Type((*position._node)._prev);
			(*position._node)._prev = (*last._node)._prev;
			(*last._node)._prev = (*first._node)._prev;
			(*first._node)._prev = tmp;
		}
	}
};
////////////////////////////////////////////////////////////////////////////

void Print(List<int>& l)
{
	List<int>::Iterator it = l.Begin();
	while (it != l.End())
	{
		cout << *it << " ";
		it++;
	}
	cout << endl;
}

void ListTest1()
{
	List<int> l;
	l.PushBack(1);
	l.PushBack(2);
	l.PushBack(3);
	l.PushBack(4);
	Print(l);
	l.PushFront(5);
	l.PushFront(6);
	l.PushFront(7);
	Print(l);
	l.PopBack();
	l.PopFront();
	Print(l);
	cout<<"数据个数："<<l.Size()<<endl;
	cout << "最后一个元素："<<l.Back() << endl;
	cout << " Begin()"<<*l.Begin() <<"  Empty()"<< l.Empty() \
		 <<" Front()"<< l.Front() << endl;
	l.Clear();
 
}
//Unique();Remove()
void ListTest2()
{

	List<int> l;
	l.PushBack(1);
	l.PushBack(1);
	l.PushBack(1);
	l.PushBack(3);
	l.PushBack(3);
	l.PushBack(3);
	l.PushBack(1);
	l.PushBack(1);
	l.PushBack(1);
	l.PushBack(3);

	Print(l);
	l.Unique();
	Print(l);
	l.PushBack(3);
	l.PushBack(3);
	l.PushBack(3);
	Print(l);
	l.Remove(3);
	Print(l);
 
}
//Merge()
void ListTest3()
{
	cout << "有序合并" << endl;
	List<int> l1;
	l1.PushBack(1);
	l1.PushBack(2);
	l1.PushBack(3);
	l1.PushBack(5);
	l1.PushBack(9);
	Print(l1);
	List<int> l2;
	l2.PushBack(2);
	l2.PushBack(4);
	l2.PushBack(6);
	l2.PushBack(7);
	l2.PushBack(8);
	Print(l2);
	l1.Merge(l2);
	Print(l1);
	cout << "未有序合并" <<endl;
	l1.PushFront(41);
	l2.PushBack(32);
	l2.PushBack(31);
	l2.PushBack(3);
	l2.PushBack(12);
	Print(l1);
	Print(l2);
	l1.Merge(l2);
	Print(l1);
}
void List_Test()
{
	//ListTest1();
	//ListTest2();
	ListTest3();
}