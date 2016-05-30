#pragma once
#include<iostream>
using namespace std;
template<class T>
struct _ListNode
{
	typedef _ListNode<T>* Pointer;
	Pointer _prev;
	Pointer _next;
	T _data;

	_ListNode()
		:_next(NULL)
		,_prev(NULL)
	{}
	_ListNode(const T&x)
		:_data(x)
		,_next(NULL)
		,_prev(NULL)
	{}
};

template<class T>
class __List_Iterator
{
public:
    //typedef __List_Iterator<T> Iterator_Category;
	typedef  Bidirectional_Iterator_Tag Iterator_Category;

	typedef T Value_Type;
	typedef Value_Type * Pointer;
	typedef Value_Type & Reference;
	typedef ptrdiff_t Difference_Type;
	
	typedef size_t Size_Type;
	typedef __List_Iterator<T> Self;
	typedef _ListNode<T>* LinkType;

	LinkType _node;
	__List_Iterator(){}
	//__List_Iterator(const Iterator&x):_node(x){}
	//__List_Iterator(const LinkType&x) :_node(x){}
	__List_Iterator(LinkType node=NULL)
		:_node(node)
	{}
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
	}
	Pointer operator->()const
	{
		return &(operator*());
	}
	Self & operator++()
	{
		_node = _node->_next;
		return *this;
	}
	Self & operator++(int)
	{
		Self tmp(_node);
		_node = _node->_next;
		return tmp;
	}
	Self& operator--()
	{
		_node = _node->_prev;
		return *this;
	}
	Self& operator--(int)
	{
		Self tmp(_node);
		_node = _node->_prev;
		return tmp;
	}


};

template<class T>
class List
{ 
public:
	//typedef __List_Iterator<T> Iterator_Category;

	 typedef __List_Iterator<T> Iterator;

	typedef T Value_Type;
	typedef Value_Type * Pointer;
	typedef size_t Size_Type;
	typedef Value_Type & Reference;
	typedef ptrdiff_t Difference_Type;

	typedef _ListNode<T>* LinkType;
 
public:
 

	List()
		 
	{
		_head._prev = &_head;
		_head._next = &_head;
	}
	~List()
	{
		Clear();
	}
public:
	Iterator Insert(Iterator pos,const Value_Type &x)
	{
		LinkType tmp = new _ListNode<T>(x);
		LinkType prev = pos._node->_prev;//--pos
		LinkType cur = pos._node;

		prev->_next = tmp;
		tmp->_prev = prev;

		tmp->_next = cur;
		cur->_prev = tmp;

		//return  tmp ;
		return Iterator(tmp);
	}
	Iterator Erase(Iterator pos)
	{
		assert(pos!=End());

		LinkType prev = pos._node->_prev;
		LinkType next = pos._node->_next;

		prev->_next = next;
		next->_prev = prev;

		delete pos._node;

		return Iterator(next);
	}
	void PushBack(const T& x)
	{
		Insert(End(),x);
	}
	void PushFront(const T&x)
	{
		Insert(Begin(),x);
	}
	void PopBack()
	{
		Erase(--End());
	}
	void PopFront()
	{
		Erase(Begin());
	}
	Iterator Begin()
	{
		return _head._next;
	}
	Iterator End()
	{
		return &_head;
	}
	void Clear()
	{
		Iterator begin = Begin();
		while (begin!=End())
		{
			LinkType tmp = begin._node;
			++begin;
			delete tmp;
		}
	}
public:
	 
	
	 
	
private:
	_ListNode<T> _head;
};

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
	l.PushBack(3);
	l.PushBack(2);
	l.PushBack(5);
	l.PushBack(1);
	l.PushBack(9);
	l.PushBack(8);
	Print(l);
	//List<int>::Iterator it = l.Begin();
	// 
	//while (it!=l.End())
	//{
	//	cout << *it << " ";
	//	it++;
	//}
	cout << endl;

}
void ListTest2()
{

	List<int> l;
	l.PushBack(3);
	l.PushBack(2);
	l.PushBack(5);
	l.PushBack(1);
	l.PushBack(9);
	l.PushBack(8);

	List<int>::Iterator it = l.Begin();
	while (it != l.End())
	{
		cout << *it << " ";
		it++;
	}
	cout << endl;
	List<int> l2;
	l2.PushBack(6);
	l2.PushBack(9);
	l2.PushBack(2);
	l2.PushBack(4);
	l2.PushBack(19);
	l2.PushBack(28);

	List<int>::Iterator it2 = l2.Begin();
	while (it2 != l2.End())
	{
		cout << *it2 << " ";
		it2++;
	}
	cout << endl;

 
}

void TestList()
{
	ListTest1();
	//ListTest2();
}