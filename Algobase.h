#pragma once

///////////////////////////////////////////////////
//Fill()类函数
template <class OutputIterator, class Size, class T>
OutputIterator Fill_n(OutputIterator first, Size n, const T& value) {
	for (; n > 0; --n, ++first)
		*first = value;
	return first;
}
//////////////////////////////////////////////////
//Copy()类函数
template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2 Copy_Backward(BidirectionalIterator1 first,
	BidirectionalIterator1 last,
	BidirectionalIterator2 result)
{
	return __Copy_Backward_Dispatch<BidirectionalIterator1,
		BidirectionalIterator2>()(first, last,
		result);
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
struct __Copy_Backward_Dispatch
{
	BidirectionalIterator2 operator()(BidirectionalIterator1 first,
	BidirectionalIterator1 last,
	BidirectionalIterator2 result) 
	{
		return __Copy_Backward(first, last, result);
	}
};
template <class BidirectionalIterator1, class BidirectionalIterator2>
inline BidirectionalIterator2 __Copy_Backward(BidirectionalIterator1 first,
	BidirectionalIterator1 last,
	BidirectionalIterator2 result) 
{
	while (first != last)
		*--result = *--last;
	return result;
}



template <class InputIterator, class OutputIterator>
inline OutputIterator Copy(InputIterator first, InputIterator last,
	OutputIterator result)
{
	return __Copy_Dispatch<InputIterator, OutputIterator>()(first, last, result);
}

inline char* Copy(const char* first, const char* last, char* result) {
	memmove(result, first, last - first);
	return result + (last - first);
}

inline wchar_t* Copy(const wchar_t* first, const wchar_t* last,
	wchar_t* result) {
	memmove(result, first, sizeof(wchar_t)* (last - first));
	return result + (last - first);
}


template <class InputIterator, class OutputIterator>
struct __Copy_Dispatch
{
	OutputIterator operator()(InputIterator first, InputIterator last,
	OutputIterator result) {
		return __Copy(first, last, result, Iterator_Category(first));
	}
};


template <class T>
struct __Copy_Dispatch<T*, T*>
{
	T* operator()(T* first, T* last, T* result) {
		typedef typename __Type_Traits<T>::Has_Trivial_Assignment_Operator t;
		return __Copy_t(first, last, result, t());
	}
};

template <class T>
struct __Copy_Dispatch<const T*, T*>
{
	T* operator()(const T* first, const T* last, T* result) {
		typedef typename __Type_Traits<T>::Has_Trivial_Assignment_Operator t;
		return __Copy_t(first, last, result, t());
	}
};


template <class T>
inline T* __Copy_t(const T* first, const T* last, T* result, __True_Type) {
	memmove(result, first, sizeof(T)* (last - first));
	return result + (last - first);
}

template <class T>
inline T* __Copy_t(const T* first, const T* last, T* result, __False_Type) {
	return __Copy_d(first, last, result, (ptrdiff_t*)0);
}

template <class RandomAccessIterator, class OutputIterator, class Distance>
inline OutputIterator
__Copy_d(RandomAccessIterator first, RandomAccessIterator last,
OutputIterator result, Distance*)
{
	for (Distance n = last - first; n > 0; --n, ++result, ++first)
		*result = *first;
	return result;
}
