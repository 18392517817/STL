#pragma once

#define  __STL_TEMPLATE_NULL  template<>

struct __True_Type {
};

struct __False_Type {
};

/*如果至少满足下面3条里的一条：
显式(explict)定义了这四种函数。
类里有非静态非POD的数据成员。
有基类。
那么上面的四种函数是non-trivial函数，
比如叫non-trivial ctor、non-trivial copy…，
也就是说有意义的函数，里面有一下必要的操作，
比如类成员的初始化，释放内存等。*/
template <class type>
struct __Type_Traits {
	typedef __True_Type     this_dummy_member_must_be_first;

	// 构造函数(ctor)
	typedef __False_Type    Has_Trivial_Default_Constructor;
	//复制构造函数(copy)
	typedef __False_Type    Has_Trivial_Copy_Constructor;
	//	赋值函数(assignment)
	typedef __False_Type    Has_Trivial_Assignment_Operator;
	//	析构函数(dtor)
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
