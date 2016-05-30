#pragma once
#include<assert.h>
#include<iostream>
using namespace std;
#include"Alloc.h"

template <class T,class Alloc=Malloc_Alloc>
class Vector
{
public:
	typedef T ValueType;
	typedef ValueType * Pointer;
	typedef ValueType * Iterator;
	typedef size_t SizeType;
protected:
	//专属空间配置器，一次配置一个元素的大小
	typedef Simple_Alloc<ValueType, Alloc> Data_Allocator;

protected:
	Iterator _start;
	Iterator _finish;
	Iterator _end_of_storage;
	//扩容函数
	void _CheckExpan()
	{
		if (_finish == _end_of_storage)
		{
			const SizeType old_size = Size();
			const SizeType len = old_size != 0 ? 2 * old_size : 1;
			T *tmp = new T[len];
			if (_start)
			{
				memcpy(tmp, _start, sizeof(T)*old_size);
			}
			_start = tmp;
			_finish = _start + old_size;
			_end_of_storage = _start + len;
		}
	}

public:
	Iterator Begin(){ return _start; }
	Iterator End(){ return _finish; }
	SizeType Size() { return SizeType(End() - Begin()); }
	SizeType MaxSize() { return SizeType(-1) / sizeof(T); }
	SizeType Capacity() { return SizeType(_end_of_storage - Begin()); }

	bool Empty()const{ return Begin() == End(); }
	ValueType & operator[](SizeType n){ return *(Begin() + n); }
public:

	Vector() :_start(NULL), _finish(NULL), _end_of_storage(NULL)
	{}

	void PushBack(const T & x)
	{
		_CheckExpan();
		assert(_finish != _end_of_storage);

		*_finish = x;
		++_finish;
	}
	void PopBack()
	{
		--_finish;	 
	}

	Iterator Erase(Iterator pos)
	{
		Iterator begin = pos + 1;
		while (begin != _finish)
		{
			*(begin - 1) = *begin;
			++begin;
		}
		--_finish;
		return pos;
	}
	 
};

void VectorTest1()
{
	Vector<int> v;
	v.PushBack(1);
	v.PushBack(2);
	v.PushBack(3);
	v.PushBack(4);
	cout << v.Size() << " " << v.Capacity() << endl;
	//cout<<v.MaxSize() << endl;
	v.PushBack(5);
	cout << v.Size() << " " << v.Capacity() << endl;
	Vector<int>::Iterator it = v.Begin();
	while (it != v.End())
	{
		cout << *it << " ";
		++it;
	}
	cout << endl;
	v.PopBack();

	Vector<int> v2(v);
	it = v2.Begin();
	while (it != v2.End())
	{
		cout << *it << " ";
		++it;
	}
	//cout << *it << endl;
	cout << endl;
}

void VectorTest2()
{
	Vector<int> v;
	v.PushBack(1);
	v.PushBack(2);
	v.PushBack(3);
	v.PushBack(4);
	//v.PushBack(5);
	Vector<int>::Iterator it = v.Begin();
	while (it != v.End())
	{
		cout << *it << " ";
		++it;
	}
	cout << endl;

	Vector<int>::Iterator ret = find(v.Begin(), v.End(), 3);
	if (ret)
	{
		cout << *ret << endl;
	}
	it = v.Begin();
	while (it != v.End())
	{
		cout << *it << " ";
		++it;
	}
	cout << endl;

}
void Vector_Test1()
{
 

}