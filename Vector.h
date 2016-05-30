#pragma once
#include<assert.h>
#include<iostream>
using namespace std;
#include"Alloc.h"
#include"Uninitialized.h"
template <class T,class Alloc=Malloc_Alloc>
class Vector
{
public:
	//vector的嵌套型别定义
	typedef T Value_Type;
	typedef Value_Type * Pointer;
	typedef Value_Type * Iterator;
	typedef size_t Size_Type;
	typedef Value_Type & Reference;
	typedef ptrdiff_t Differnce_Type;
	
protected:
	//专属空间配置器，一次配置一个元素的大小
	typedef Simple_Alloc<Value_Type, Alloc> Data_Allocator;
	//构造函数，允许指定Vector的大小n和初值value

	void Insert_aux(Iterator position, const T& x);
	void Deallocate() {
		if (_start) Data_Allocator::Deallocate(_start, _end_of_storage - _start);
	}
	//填充并初始化
	void Fill_Initialize(Size_Type n, const T& value) {
		_start = Allocate_And_Fill(n, value);
		_finish = _start + n;
		_end_of_storage = _finish;
	}
	//配置空间并填满内容
	Iterator Allocate_And_Fill(Size_Type n, const T& x) {
		Iterator result = Data_Allocator::Allocate(n);

		Uninitialized_Fill_n(result, n, x);//全局函数  见Uninitialized.h
		return result;

	}
protected:
	Iterator _start;
	Iterator _finish;
	Iterator _end_of_storage;
	//扩容函数
	void _CheckExpan()
	{
		if (_finish == _end_of_storage)
		{
			const Size_Type old_size = Size();
			const Size_Type len = old_size != 0 ? 2 * old_size : 1;
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
	Size_Type Size() { return Size_Type(End() - Begin()); }
	Size_Type MaxSize() { return Size_Type(-1) / sizeof(T); }
	Size_Type Capacity() { return Size_Type(_end_of_storage - Begin()); }
	bool Empty()const{ return Begin() == End(); }
	Value_Type & operator[](Size_Type n){ return *(Begin() + n); }

public:
	Vector() :_start(NULL), _finish(NULL), _end_of_storage(NULL)
	{}
	 Vector(Size_Type n, const T&value){ Fill_Initialize(n, value); }

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
	//Vector<int> v1(1,2);

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