#pragma once
#include"Type_Traits.h"
/*
内存基本处理工具，
*/
template <class ForwardIterator, class Size, class T>
inline ForwardIterator Uninitialized_Fill_n(ForwardIterator first, Size n,
	const T& x) {
	//?value_type(first) 类型萃取 Itearator.h
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


//暂时放在这里
template <class OutputIterator, class Size, class T>
OutputIterator Fill_n(OutputIterator first, Size n, const T& value) {
	for (; n > 0; --n, ++first)
		*first = value;
	return first;
}