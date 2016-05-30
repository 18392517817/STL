#pragma once
//ֻ��������
struct Input_Iterator_Tag {};
//ֻд������
struct Output_Iterator_Tag {};
//ǰ�������
struct Forward_Iterator_Tag : public Input_Iterator_Tag {};
//˫�������
struct Bidirectional_Iterator_Tag : public Forward_Iterator_Tag {};
//���������
struct Random_Access_Iterator_Tag : public Bidirectional_Iterator_Tag {};

template <class T, class Distance> struct Input_Iterator {
	typedef Input_Iterator_Tag Iterator_Category;
	typedef T                  Value_Type;
	typedef Distance           Difference_Type;
	typedef T*                 Pointer;
	typedef T&                 Reference;
};

struct Output_Iterator {
	typedef Output_Iterator_Tag Iterator_Category;
	typedef void                Value_Type;
	typedef void                Difference_Type;
	typedef void                Pointer;
	typedef void                Reference;
};

template <class T, class Distance> struct Forward_Iterator {
	typedef Forward_Iterator_Tag Iterator_Category;
	typedef T                    Value_Type;
	typedef Distance             Difference_Type;
	typedef T*                   Pointer;
	typedef T&                   Reference;
};


template <class T, class Distance> struct Bidirectional_Iterator {
	typedef Bidirectional_Iterator_Tag Iterator_Category;
	typedef T                          Value_Type;
	typedef Distance                   Difference_Type;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};

template <class T, class Distance> struct Random_Access_Iterator {
	typedef Random_Access_Iterator_Tag Iterator_Category;
	typedef T                          Value_Type;
	typedef Distance                   Difference_Type;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};

/////////////////////////////////////////////////////////////////////////////////////
template <class Category, class T, class Distance = ptrdiff_t,
class Pointer = T*, class Reference = T& >
struct Iterator {
	typedef Category  Iterator_Category;//����������
	typedef T         Value_Type;       //��������ָ�������� 
	typedef Distance  Difference_Type;	//����������֮�����
	typedef Pointer   Pointer;			//��������ָ�����ָ��	
	typedef Reference Reference;		//��������ָ���������
};


//Traits ����һ̨��������ȡ������եȡ����������������

template <class Iterator>
struct Iterator_Traits
{
	typedef typename Iterator::Iterator_Category Iterator_Category;
	typedef typename Iterator::Value_Type        Value_Type;
	typedef typename Iterator::Difference_Type   Difference_Type;
	typedef typename Iterator::Pointer           Pointer;
	typedef typename Iterator::Reference         Reference;
};

//ƫ�ػ�ԭ��ָ������

template <class T>
struct Iterator_Traits<T*> {
	typedef Random_Access_Iterator_Tag Iterator_Category;
	typedef T                          Value_Type;
	typedef ptrdiff_t                  Difference_Type;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};


//ƫ�ػ�constԭ��ָ������

template <class T>
struct Iterator_Traits<const T*> {
	typedef Random_Access_Iterator_Tag Iterator_Category;
	typedef T                          Value_Type;
	typedef ptrdiff_t                  Difference_Type;
	typedef const T*                   Pointer;
	typedef const T&                   Reference;
};
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
//Value_type()������ȡ��ʵ�� 
template <class T, class Distance>
inline T* Value_Type(const Input_Iterator<T, Distance>&) {
	return (T*)(0);
}

template <class T, class Distance>
inline T* Value_Type(const Forward_Iterator<T, Distance>&) {
	return (T*)(0);
}

template <class T, class Distance>
inline T* Value_Type(const Bidirectional_Iterator<T, Distance>&) {
	return (T*)(0);
}

template <class T, class Distance>
inline T* Value_Type(const Random_Access_Iterator<T, Distance>&) {
	return (T*)(0);
}

template <class T>
inline T* Value_Type(const T*) { return (T*)(0); }



////////////////////////////////////////////////////////
//Distance��ʵ��
template <class InputIterator>
inline typename Iterator_Traits<InputIterator>::Difference_Type
__Distance(InputIterator first, InputIterator last, Input_Iterator_Tag) {
	Iterator_Traits<InputIterator>::Difference_Type n = 0;
	while (first != last) {
		++first; ++n;
	}
	return n;
}

template <class RandomAccessIterator>
inline typename Iterator_Traits<RandomAccessIterator>::Difference_Type
__Distance(RandomAccessIterator first, RandomAccessIterator last,
Random_Access_Iterator_Tag) {
	return last - first;
}

template <class InputIterator>
inline typename Iterator_Traits<InputIterator>::Difference_Type
Distance(InputIterator first, InputIterator last)
{
	typedef typename Iterator_Traits<InputIterator>::Iterator_Category category;
	return __Distance(first, last, category());
	 //return __Distance(first,last,Iterator_Traits<InputIterator>::Iterator_Category() );

}

///////////////////////////////////////////////////////////////////////////////
//Advance��ʵ��

template <class InputIterator, class Distance>
inline void __Advance(InputIterator& i, Distance n, Input_Iterator_Tag) {
	while (n--) ++i;
}

template <class BidirectionalIterator, class Distance>
inline void __Advance(BidirectionalIterator& i, Distance n,
	Bidirectional_Iterator_Tag)
{
	if (n >= 0)
		while (n--) ++i;
	else
		while (n++) --i;
}

template <class RandomAccessIterator, class Distance>
inline void __Advance(RandomAccessIterator& i, Distance n,
	Random_Access_Iterator_Tag) {
	i += n;
}

template <class InputIterator, class Distance>
inline void Advance(InputIterator& i, Distance n) 
{
	/*__Advance(i, n, Iterator_Category(i));*/
	__Advance(i, n, Iterator_Traits<InputIterator>::Iterator_Category());
}
#include"List.h"
#include"Vector.h"
void Iterator_Test1()
{
	List<int>l;
	l.PushBack(1);
	l.PushBack(2);
	l.PushBack(3);
	l.PushBack(4);
	cout << "List Distance:" << Distance(l.Begin(),l.End())<<endl;
	 
	Vector<int> v;
	v.PushBack(1);
	v.PushBack(2);
	v.PushBack(3);
	v.PushBack(4);
	v.PushBack(5);
	v.PushBack(6);
	cout << "Vector Distance:" << Distance(v.Begin(), v.End()) << endl;

}


void Iterator_Test()
{
	Iterator_Test1();
}

