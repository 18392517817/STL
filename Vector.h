#pragma once
#include<assert.h>
#include<iostream>
using namespace std;
#include"Alloc.h"
#include"Uninitialized.h"
#include"Construct.h"
#include"Algobase.h"
template <class T,class Alloc=Malloc_Alloc>
class Vector
{
public:
	//vector的嵌套型别定义
	typedef T Value_Type;
	typedef Value_Type * Pointer;
	typedef Value_Type * Iterator;
	typedef Value_Type & Reference;
	typedef ptrdiff_t Differnce_Type;
	typedef size_t Size_Type;

protected:
	//专属空间配置器，一次配置一个元素的大小
	typedef Simple_Alloc<Value_Type, Alloc> Data_Allocator;
	
	void Insert_aux(Iterator position, const T& x)
	{
		if (_finish != _end_of_storage)
		{
			//还有备用空间
			Construct(_finish, *(_finish - 1));
			++_finish;
			T x_copy = x;
			//?暂时留下   完成
			//把从倒数第二个值到pos位置的值向后移动
			//Copy_Backward(position, _finish - 2, _finish - 1);//Al 
			while (position != _finish)
				_finish = --_finish;
			*position = x_copy;
		}
		else 
		{
			const Size_Type old_size = size();
			const Size_Type len = old_size != 0 ? 2 * old_size : 1;
			Iterator new_start = Data_Allocator::Allocate(len);
			Iterator new_finish = new_start;
			//？明天再搞Uninitialized_Copy()  搞定了
			new_finish = Uninitialized_Copy(_start, position, new_start);
			Construct(new_finish, x);
			++new_finish;
			new_finish = Uninitialized_Copy(position, _finish, new_finish);

 
			Destroy(Begin(), End());
			 
			Deallocate();
			_start = new_start;
			_finish = new_finish;
			_end_of_storage = new_start + len;
		}
	}
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
	Size_Type Max(Size_Type one,Size_Type two)
	{
		return one > two ? one : two;
	}
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
	Reference operator[](Size_Type n){ return *(Begin() + n); }

public:
	Vector() :_start(NULL), _finish(NULL), _end_of_storage(NULL)
	{}
	//构造函数，允许指定Vector的大小n和初值value

	Vector(Size_Type n, const T&value){ Fill_Initialize(n, value); }
	Vector(int n, const T&value){ Fill_Initialize(n, value); }
	Vector(long n, const T&value){ Fill_Initialize(n, value); }
	explicit Vector(Size_Type n){ Fill_Initialize(n, T()); }
	void Insert(Iterator position,Size_Type n,const T&x);//函数代码多，放外面实现
	~Vector()
	{
		Destroy(_start,_finish);
		Deallocate();
	}
	Reference Front(){ return *Begin(); }//第一个元素
	Reference Back(){ return *End(); }//最后一个元素
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
		Destroy(_finish);
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
	//void Resize(Size_Type new_size,const T& x)
	//{
	//	if (new_size < size())
	//		Erase(Begin()+new_size,End());
	//	 

	//}
};
template <class T, class Malloc_Alloc>
void Vector<T, Malloc_Alloc>::Insert(Iterator position, Size_Type n, const T& x) 
{
	if (n != 0) 
	{
		if (Size_Type(_end_of_storage - _finish) >= n) 
		{
			T x_copy = x;
			const Size_Type elems_after = _finish - position;
			Iterator old_finish = _finish;
			if (elems_after > n)
			{
				Uninitialized_Copy(_finish - n, _finish, _finish);
				_finish += n;
				Copy_Backward(position, old_finish - n, old_finish);
				Fill(position, position + n, x_copy);
			}
			else 
			{
				Uninitialized_Fill_n(_finish, n - elems_after, x_copy);
				_finish += n - elems_after;
				Uninitialized_Copy(position, old_finish, _finish);
				_finish += elems_after;
				Fill(position, old_finish, x_copy);
			}
		}
		else 
		{
			const Size_Type old_size = Size();
			const Size_Type len = old_size + Max(old_size, n);// 
			Iterator new_start = Data_Allocator::Allocate(len);
			Iterator new_finish = new_start;
			 
				new_finish = Uninitialized_Copy(_start, position, new_start);
				new_finish = Uninitialized_Fill_n(new_finish, n, x);
				new_finish = Uninitialized_Copy(position, _finish, new_finish);
			 
#         ifdef  __STL_USE_EXCEPTIONS 
			catch (...) 
			{
				Destroy(new_start, new_finish);
				Data_Allocator::Deallocate(new_start, len);
				throw;
			}
#         endif /* __STL_USE_EXCEPTIONS */
			Destroy(_start, _finish);
			Deallocate();
			_start = new_start;
			_finish = new_finish;
			_end_of_storage = new_start + len;
		}
	}
}

///////////////////////////////////////////////////////////////////
//打印函数
void Print(Vector<int> &const v)
{
	Vector<int>::Iterator it = v.Begin();
	while (it != v.End())
	{
		cout << *it << " ";
		++it;
	}
	cout << endl;
}
void VectorTest1()
{
	Vector<int> v;
	v.PushBack(1);
	v.PushBack(2);
	v.PushBack(3);
	v.PushBack(4);
	v.PushBack(5);
	v.PopBack();
	Print(v);
	Vector<int> v1(4, 2);
	v1.PushBack(3);
	Print(v1);
 
}
//Insert()函数测试
void VectorTest2()
{ 
	Vector<int> v;
	v.PushBack(1);
	v.PushBack(2);
	v.PushBack(3);
	v.PushBack(4);
	v.PushBack(5);
	Print(v);
	v.Insert(v.Begin(),3,11);
	Print(v);
 
 

 
 
}
void Vector_Test()
{
	VectorTest2();
	VectorTest1();

}