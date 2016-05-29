#pragma once


//第一级配置器

//内存分配失败后处理句柄handler类型
typedef void(*ALLOC_FUN)();
template<int init>
class __Malloc_Alloc_Template
{
private:
	//处理内存不足
	//static void(*__Malloc_Alloc_Oom_Handler)();
	static ALLOC_FUN __Malloc_Alloc_Oom_Handler;
	
	static void* Oom_Malloc(size_t n)
	{
		//void(*my_malloc_handler)();
		ALLOC_FUN my_malloc_handler;
		void *result;

		//1、分配成功，直接返回
		//2、分配失败，检查是否处理的my_malloc_handler
		//有，调用以后再分配，不断重复
		//无，直接返回，结束程序exit(-1)
		for (;;) {
			my_malloc_handler = __Malloc_Alloc_Oom_Handler;
			if (0 == my_malloc_handler)
			{
				cout << "Out of Merroy" << endl;
				exit(-1);
			}
			 (*my_malloc_handler)();
			result = malloc(n);
			if (result)
				return(result);
		}
	}
	static void* Oom_Realloc(void * p, size_t n)
	{//void(*my_malloc_handler)();
		ALLOC_FUN my_malloc_handler;
		void *result;

		//1、分配成功，直接返回
		//2、分配失败，检查是否处理的my_malloc_handler
		//有，调用以后再分配，不断重复
		//无，直接返回，结束程序exit(-1)
		for (;;) {
			my_malloc_handler = __Malloc_Alloc_Oom_Handler;
			if (0 == my_malloc_handler)
			{
				cout << "Out of Merroy" << endl;
				exit(-1);
			}
			(*my_malloc_handler)();
			result = realloc(p,n);//
			if (result)
				return(result);
		}
	}
public:
	static void * Allocate(size_t n)
	{
		void *result = malloc(n);
		if (0 == result)
			result = Oom_Malloc(n);
		return result;
	}

	static void Deallocate(void *p, size_t /* n */)
	{
		free(p);
	}

	static void * Reallocate(void *p, size_t /* old_sz */, size_t new_sz)
	{
		void * result = realloc(p, new_sz);
		if (0 == result) result = oom_realloc(p, new_sz);
		return result;
	}
	//仿真C++的set_new_handler();
	//指定自己的out-of-memory handler
	static void(*set_malloc_handler(void(*f)()))()
	{
		void(*old)() = __Malloc_Alloc_Oom_Handler;
		__Malloc_Alloc_Oom_Handler = f;
		return(old);
	}

};

//分配内存失败，处理函数的句柄函数指针
template<int inst>
ALLOC_FUN __Malloc_Alloc_Template<inst>::__Malloc_Alloc_Oom_Handler = 0;