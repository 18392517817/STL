#pragma once
/*
���캯���������������ߣ�Construct()��Destory()����
*/
#include"Type_Traits.h"
#include <new.h>//ʹ��placement new

template <class T1, class T2>
inline void Construct(T1* p, const T2& value) {
	//placement new ;����T1::T1(value)
	new (p)T1(value);
}

//������Destroy()��һ�汾������һ��ָ��
template <class T>
inline void Destroy(T* pointer) {
	//����dtor ~T()
	pointer->~T();
}

//������Destroy()�ڶ��汾�������������������˺����跨�ҳ�Ԫ�ص���ֵ�ͱ�
//��������__Type_Traits<>��ȡ���ʵ���ʩ
template <class ForwardIterator>
inline void Destroy(ForwardIterator first, ForwardIterator last) {
	__Destroy(first, last, value_type(first));
}

//�ж�Ԫ�ص���ֵ�ͱ�value type���Ƿ��� trivial destructor
template <class ForwardIterator, class T>
inline void __Destroy(ForwardIterator first, ForwardIterator last, T*) {
	//?__Type_Traits<T>::has_trivial_destructor
	typedef typename __Type_Traits<T>::Has_Trivial_Destructor trivial_destructor;
	
	__Destroy_aux(first, last, trivial_destructor());
}

//���Ԫ�ص���ֵ�ͱ�value type����non-trivial destructor
template <class ForwardIterator>
inline void
__Destroy_aux(ForwardIterator first, ForwardIterator last, __False_Type) {
	for (; first < last; ++first)
		Destroy(&*first);
}

//���Ԫ�ص���ֵ�ͱ�value type����trivial destructor
template <class ForwardIterator>
inline void __Destroy_aux(ForwardIterator, ForwardIterator, __True_Type) {}



//������Destroy()�ڶ��汾��Ե�����char* ��wchar_t*���ػ���
inline void Destroy(char*, char*) {}
inline void Destroy(wchar_t*, wchar_t*) {}

 

 
