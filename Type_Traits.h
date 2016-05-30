#pragma once

#define  __STL_TEMPLATE_NULL  template<>

struct __True_Type {
};

struct __False_Type {
};

template <class type>
struct __Type_Traits {
	typedef __True_Type     this_dummy_member_must_be_first;
	 
	typedef __False_Type    Has_Trivial_Default_Constructor;
	typedef __False_Type    Has_Trivial_Copy_Constructor;
	typedef __False_Type    Has_Trivial_Assignment_Operator;
	typedef __False_Type    Has_Trivial_Destructor;
	typedef __False_Type    Is_POD_Type;
};


__STL_TEMPLATE_NULL struct __Type_Traits<char> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};

__STL_TEMPLATE_NULL struct __Type_Traits<signed char> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};

__STL_TEMPLATE_NULL struct __Type_Traits<unsigned char> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};

__STL_TEMPLATE_NULL struct __Type_Traits<short> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};

__STL_TEMPLATE_NULL struct __Type_Traits<unsigned short> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};

__STL_TEMPLATE_NULL struct __Type_Traits<int> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};

__STL_TEMPLATE_NULL struct __Type_Traits<unsigned int> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};

__STL_TEMPLATE_NULL struct __Type_Traits<long> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};

__STL_TEMPLATE_NULL struct __Type_Traits<unsigned long> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};

__STL_TEMPLATE_NULL struct __Type_Traits<float> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};

__STL_TEMPLATE_NULL struct __Type_Traits<double> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};

__STL_TEMPLATE_NULL struct __Type_Traits<long double> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};

 
template <class T>
struct __Type_Traits<T*> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};

//原生指针设计
/*
template <class T>
struct __Type_Traits<T*> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};
*/
 template<>
struct __Type_Traits<char*> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};
template<>
struct __Type_Traits<signed char*> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};
template<>
struct __Type_Traits<unsigned char*> {
	typedef __True_Type    Has_Trivial_Default_Constructor;
	typedef __True_Type    Has_Trivial_Copy_Constructor;
	typedef __True_Type    Has_Trivial_Assignment_Operator;
	typedef __True_Type    Has_Trivial_Destructor;
	typedef __True_Type    Is_POD_Type;
};
