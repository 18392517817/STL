#pragma once
/*
构造函数和析构基本工具：Construct()和Destory()函数
*/
#include"Type_Traits.h"
#include <new.h>//使用placement new

template <class T1, class T2>
inline void Construct(T1* p, const T2& value) {
	//placement new ;调用T1::T1(value)
	new (p)T1(value);
}

///////////////////////////////////////////////////////////////////////////
//以下是Destroy()第一版本，接受一个指针
template <class T>
inline void Destroy(T* Pointer) {
	//调用dtor ~T()
	Pointer->~T();
}
/*
在调用析构函数前，有必要解释下trivial destructor和non-trivial destructor。
如果用户不定义析构函数，那么编译器会自动合成析构函数，但是这个析构函数基本没什么用，所以称为
trivial destructor无意义的析构函数。
如果用户定义了析构函数，说明在释放对象所占用内存之前要做一些事情，就成为non-trivial destructor。
在C++中，如果只有基本数据类型，那么就不用调用析构函数，就认为析构函数是trivial destructor。
如果有像指针这样的数据类型，那么就有必要调用析构函数了，就认为析构函数是non-trivial destructor。

在STL中，调用析构函数时，都会用一个Has_Trivial_Destructor来判断对象是否含有trivial destructor，如果是，
那么不会调用析构函数，什么也不做
void __Destroy_aux(ForwardIterator, ForwardIterator, __True_Type)是空函数。
反之，则调用析构函数，释放对象。
__Destroy_aux(ForwardIterator first, ForwardIterator last, __False_Type)调用析构函数

结合下面的图来看代码：

特化
____>__Destroy_aux(.,_false_type)
|
|NO _false_type 进行析构
|
__泛化_>_Destroy()--has non-trival destructor?
|                  |
|                  |YES _true_type  不进行析构
|                  |
|                  |_特化_>_Destroy_aux(,_true_type)
Destroy()--
|
|
|
|------特化------>不进行析构
|  (char* ,char*)
|
|------特化------>不进行析构
|  (wchar* ,wchar*)
|
|
|------特化------>pointer->~T()
(T* pointer)


*/



//以下是Destroy()第二版本，接受两个迭代器，此函数设法找出元素的数值型别
//进而利用__Type_Traits<>求取最适当措施
template <class ForwardIterator>
inline void Destroy(ForwardIterator first, ForwardIterator last) {
	__Destroy(first, last, Value_Type(first));
}

//判断元素的数值型别（value type）是否有 trivial destructor
template <class ForwardIterator, class T>
inline void __Destroy(ForwardIterator first, ForwardIterator last, T*)
{
	//?__Type_Traits<T>::has_trivial_destructor
	typedef typename __Type_Traits<T>::Has_Trivial_Destructor trivial_destructor;
	
	__Destroy_aux(first, last, trivial_destructor());
}

//如果元素的数值型别（value type）有non-trivial destructor
template <class ForwardIterator>
inline void
__Destroy_aux(ForwardIterator first, ForwardIterator last, __False_Type)
{
	for (; first < last; ++first)
		Destroy(&*first);
}

//如果元素的数值型别（value type）有trivial destructor
template <class ForwardIterator>
inline void __Destroy_aux(ForwardIterator, ForwardIterator, __True_Type) {}

//以下是Destroy()第二版本针对迭代器char* 和wchar_t*的特化版
inline void Destroy(char*, char*) {}
inline void Destroy(wchar_t*, wchar_t*) {}

