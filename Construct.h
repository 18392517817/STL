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

//以下是Destroy()第一版本，接受一个指针
template <class T>
inline void Destroy(T* pointer) {
	//调用dtor ~T()
	pointer->~T();
}

//以下是Destroy()第二版本，接受两个迭代器，此函数设法找出元素的数值型别
//进而利用__Type_Traits<>求取最适当措施
template <class ForwardIterator>
inline void Destroy(ForwardIterator first, ForwardIterator last) {
	__Destroy(first, last, value_type(first));
}

//判断元素的数值型别（value type）是否有 trivial destructor
template <class ForwardIterator, class T>
inline void __Destroy(ForwardIterator first, ForwardIterator last, T*) {
	//?__Type_Traits<T>::has_trivial_destructor
	typedef typename __Type_Traits<T>::Has_Trivial_Destructor trivial_destructor;
	
	__Destroy_aux(first, last, trivial_destructor());
}

//如果元素的数值型别（value type）有non-trivial destructor
template <class ForwardIterator>
inline void
__Destroy_aux(ForwardIterator first, ForwardIterator last, __False_Type) {
	for (; first < last; ++first)
		Destroy(&*first);
}

//如果元素的数值型别（value type）有trivial destructor
template <class ForwardIterator>
inline void __Destroy_aux(ForwardIterator, ForwardIterator, __True_Type) {}



//以下是Destroy()第二版本针对迭代器char* 和wchar_t*的特化版
inline void Destroy(char*, char*) {}
inline void Destroy(wchar_t*, wchar_t*) {}

 

 
