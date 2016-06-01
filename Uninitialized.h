#pragma once
#include"Type_Traits.h"
#include"Algobase.h"
#include"Construct.h"
/*
内存基本处理工具，
*/
/////////////////////////////////////////////////////////////////////////
//Uninitialized_Fill_n()函数
template <class ForwardIterator, class Size, class T>
inline ForwardIterator Uninitialized_Fill_n(ForwardIterator first, Size n,
	const T& x) {
	//?Value_Type(first) 类型萃取 Itearator.h
	return __Uninitialized_Fill_n(first, n, x, Value_Type(first));
}

template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __Uninitialized_Fill_n(ForwardIterator first, Size n,
	const T& x, T1*) {
	typedef typename __Type_Traits<T1>::Is_POD_Type is_POD;
	return __Uninitialized_Fill_n_aux(first, n, x, is_POD());

}


template <class ForwardIterator, class Size, class T>
inline ForwardIterator
__Uninitialized_Fill_n_aux(ForwardIterator first, Size n,
const T& x, __True_Type) {
	return Fill_n(first, n, x);
}

template <class ForwardIterator, class Size, class T>
ForwardIterator
__Uninitialized_Fill_n_aux(ForwardIterator first, Size n,
const T& x, __False_Type) {
	ForwardIterator cur = first;
	 
		for (; n > 0; --n, ++cur)
		Construct(&*cur, x);
		return cur;
	 
}
///////////////////////////////////////////////////////////////////////
//Uninitialized_Copy()类函数
template <class InputIterator, class ForwardIterator>
inline ForwardIterator
Uninitialized_Copy(InputIterator first, InputIterator last,
ForwardIterator result) {
	return __Uninitialized_Copy(first, last, result, Value_Type(result));
}
//特化
inline char* Uninitialized_Copy(const char* first, const char* last,
	char* result) {
	memmove(result, first, last - first);
	return result + (last - first);
}

inline wchar_t* Uninitialized_Copy(const wchar_t* first, const wchar_t* last,
	wchar_t* result) {
	memmove(result, first, sizeof(wchar_t)* (last - first));
	return result + (last - first);
	
}
template <class InputIterator, class ForwardIterator>
inline ForwardIterator
__Uninitialized_Copy_aux(InputIterator first, InputIterator last,
ForwardIterator result,
__True_Type) {
	return Copy(first, last, result);
	
}

template <class InputIterator, class ForwardIterator>
ForwardIterator
__Uninitialized_Copy_aux(InputIterator first, InputIterator last,
ForwardIterator result,
__False_Type) {
	ForwardIterator cur = result;
	 
		for (; first != last; ++first, ++cur)
		Construct(&*cur, *first);
		return cur;
	 
	 Destroy(result, cur);
}


template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator
__Uninitialized_Copy(InputIterator first, InputIterator last,
ForwardIterator result, T*) {
	typedef typename __Type_Traits<T>::Is_POD_Type is_POD;
	return __Uninitialized_Copy_aux(first, last, result, is_POD());

}

