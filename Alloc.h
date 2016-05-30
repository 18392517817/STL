#pragma once
//#include<stdarg.h>
//#define __DEBUG__
static string GetFileName(const string& path)
{
	char ch = '/';
#ifdef _WIN32
	ch = '\\';
#endif
	size_t pos = path.rfind(ch);
	if (pos == string::npos)
		return path;
	else
		return path.substr(pos+1);
}
//用于调试追溯的trace log
inline static void __trace_debug(const char* function,
	const char* filename,int line,char* format, ...)
{
#ifdef __DEBUG__
	//输出调用函数的信息
	fprintf(stdout,"【%s:%d】%s",GetFileName(filename).c_str(),line,function);
	//输出用户打的trace信息
	va_list args;
	_crt_va_start(args,format);
	vfprintf(stdout,format,args);
	_crt_va_end(args);
#endif
}
/*va_list用于声明一个变量，我们知道函数的可变参数列表其实就是
一个字符串，所以va_list才被声明为字符型指针，这个类型用于声明
一个指向参数列表的字符型指针变量，例如：va_list ap;//ap:arguement pointer
va_start(ap,v),它的第一个参数是指向可变参数字符串的变量，
第二个参数是可变参数函数的第一个参数，通常用于指定可变参数列表
中参数的个数。
va_arg(ap,t),它的第一个参数指向可变参数字符串的变量，第二个参数是可变参数的类型。
va_end(ap) 用于将存放可变参数字符串的变量清空（赋值为NULL).
*/
#define __TRACE_DEBUG(...)\
	__trace_debug(__FUNCTION__,__FILE__,__LINE__,__VA_ARGS__);
//第一级配置器

//内存分配失败后处理句柄handler类型
typedef void(*ALLOC_FUN)();

template<int inst>
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
				cerr<< "Out of Merroy" << endl;
				exit(-1);
			}
			 (*my_malloc_handler)();
			result = malloc(n);
			if (result)
				return(result);
		}
	}
	static void* Oom_Realloc(void * p, size_t n)
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
				cerr << "Out of Merroy" << endl;
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
		__TRACE_DEBUG("(n:%u)\n",n);

		void *result = malloc(n);
		if (0 == result)
			result = Oom_Malloc(n);
		return result;
	}

	static void Deallocate(void *p, size_t   n  )
	{
		__TRACE_DEBUG("(n:%u)\n", n);

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

typedef __Malloc_Alloc_Template<0> Malloc_Alloc;


///////////////////////////////////////////////////////////////////////

//二级空间配置器


template <bool threads, int inst>
class __Default_Alloc_Template {

private:
	// Really we should use static const int x = N
	// instead of enum { x = N }, but few compilers accept the former.
 
	enum { __ALIGN = 8 };//排列基准值
	enum { __MAX_BYTES = 128 };//最大值
	enum { __NFREELISTS = __MAX_BYTES / __ALIGN };//链表值
 
	static size_t ROUND_UP(size_t bytes) {
		return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
	}
 
	static  size_t FREELIST_INDEX(size_t bytes) {
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
	}
	union Obj {
		union Obj * Free_List_Link;//指向下一个内存块的指针
		char Client_Data[1];    /* The client sees this.        */
	};
private:

	static Obj* volatile Free_List[__NFREELISTS];//自由链表

	// 获取大块内存插入到自由链表中
	static void *Refill(size_t n)
	{
		__TRACE_DEBUG("(n:%u)\n", n);

		//分配n 个bytes的内存
		//如果不够，有多少分配多少

		int nobjs = 20;
		char* chunk = Chunk_Alloc(n,nobjs);

		//如果只分配到一块，直接返回这块内存
		if (nobjs == 1)
			return chunk;

		Obj* result;
		Obj* cur;
		size_t index = FREELIST_INDEX(n);
		result = (Obj*)chunk; //返回用
		
		//把剩余的块链接到自由链表上
		cur = (Obj*)(chunk+n);
		Free_List[index] = cur;
		for (int i = 2; i < nobjs;++i)
		{
			cur->Free_List_Link = (Obj*)(chunk+i*n);
			cur = cur->Free_List_Link;
		}
		cur->Free_List_Link = NULL;

		return result;
	}
	//从内存池中分配大块内存
	static char *Chunk_Alloc(size_t size, int &nobjs)
	{
		__TRACE_DEBUG("(size: %u, nobjs: %d)\n",size,nobjs);

		char* result;
		size_t bytesNeed = size*nobjs;
		size_t bytesLeft = _end_free - _start_free;//内存池剩余空间

		//1、内存池的内存池足够，bytesLeft>=bytesNeed,则直接从内存池中提取
		//2、内存池中内存不足，但够bytesLeft>=size,则直接提取出来
		//3、内存池中内存不足，则从系统堆分配大块内存到内存池
		if (bytesLeft >= bytesNeed)
		{
			__TRACE_DEBUG("内存池内存满足分配%d个对象\n",nobjs);

			result = _start_free;
			_start_free += bytesNeed;
			//return result;
		}
		else if (bytesLeft >= size)
		{
			__TRACE_DEBUG("内存池不够分配%d个对象，只能分配%d个对象\n",nobjs,bytesLeft/size);

			nobjs = bytesLeft / size;
			result = _start_free;
			_start_free += nobjs*size;
			//return result;
		}
		else
		{
			//若内存池中还有剩余内存，则将它头插到合适的自由链表
			if (bytesLeft > 0)
			{
				size_t index = FREELIST_INDEX(bytesLeft);
				((Obj*)_start_free)->Free_List_Link = Free_List[index];
				Free_List[index] = (Obj*)_start_free;
				_start_free = NULL;

				__TRACE_DEBUG("将内存池中剩余的空间，分配给Free_List[%d]\n",index);
			}

			//从系统堆分配两倍+已分配的_heap_size/8的内存到内存池
			 
			size_t bytes_To_Get = 2 * bytesNeed + ROUND_UP(_heap_size >> 4);
			_start_free = (char*)malloc(bytes_To_Get);

			__TRACE_DEBUG("内存池空间不足，系统堆分配%d bytes内存\n",bytes_To_Get);
			//如果在系统堆中内存分配失败，则尝试到自由链表中大一级的节点中分配
			if (_start_free == NULL)
			{
				__TRACE_DEBUG("系统堆已没有空间，请到自由链表中看看\n");
	
				for (int i = size; i <= __MAX_BYTES; i+=__ALIGN)
				{
					Obj* head = Free_List[FREELIST_INDEX(size)];
					if (head)
					{
						_start_free = (char*)head;
						head = head->Free_List_Link;
						_end_free = _start_free + i;
						return Chunk_Alloc(size, nobjs);
					}
				}
				//最后的稻草
				//再到一级配置器中看有没有设置处理的内存，
				__TRACE_DEBUG("系统堆和自由链表都无内存，转到一级空间配置器\n");
				
				_end_free = 0;
				_start_free = (char*)Malloc_Alloc::Allocate(bytes_To_Get);

			}
			_heap_size +=bytes_To_Get;
			_end_free = _start_free +bytes_To_Get;

			//递归调用获取内存
			return Chunk_Alloc(size, nobjs);

		}
		return result;
	}

	// Chunk allocation state.
	static char *_start_free;//内存池开始的地方
	static char *_end_free;//内存池结束的地方
	static size_t _heap_size;
public:

	/* n must be > 0      */
	static void * Allocate(size_t n)
	{
		__TRACE_DEBUG("(n:%u)\n", n);

		/*
		//仿照stl源码
		obj * volatile * my_free_list;
		obj *  result;
		//
		if (n > (size_t)__MAX_BYTES) {
			return(Malloc_Alloc::Allocate(n));
		}
		my_free_list = free_list + FREELIST_INDEX(n);
	
		result = *my_free_list;
		if (result == 0) {
			void *r = Refill(ROUND_UP(n));
			return r;
		}
		*my_free_list = result->Free_List_Link;
		return (result);
		*/

		//若n > 128 则直接调用一级空间配置器
		//否则在二级配置器中获取

		if (n>__MAX_BYTES)
		{
			return Malloc_Alloc::Allocate(n);
		}
		
		size_t index = FREELIST_INDEX(n);
		void* ret = NULL;

		//1、如果自由链表中没有内存，调用Refill()进行填充
		//2、如果有，返回一个节点块内存
		
		Obj* head = Free_List[index];
		if (head == NULL)
		{
			return Refill(n);
		}
		else
		{
			__TRACE_DEBUG("自由链表取内存：Free_List[%d]\n",index);
	
			Free_List[index] = head->Free_List_Link;
			return head;
		}
	}

	/* p may not be 0 */
	static void Deallocate(void *p, size_t n)
	{
		__TRACE_DEBUG("(p: %p,n: %u)\n",p,n);

		/*obj *q = (obj *)p;
		obj * volatile * my_free_list;

		if (n > (size_t)__MAX_BYTES) {
			Malloc_Alloc::Deallocate(p, n);
			return;
		}
		my_free_list = free_list + FREELIST_INDEX(n);
	 
		q->Free_List_Link = *my_free_list;
		*my_free_list = q;*/
	
		if (n > __MAX_BYTES)
		{
			Malloc_Alloc::Deallocate(p,n);
		}
		else
		{
			size_t index = FREELIST_INDEX(n);
			Obj* tmp = (Obj*)p;
			tmp->Free_List_Link = Free_List[index];
			Free_List[index] = tmp;
		}
	}

	static void * Reallocate(void *p, size_t old_sz, size_t new_sz)
	{
		void *result;
		size_t copy_sz;

		if (old_sz>(size_t)__MAX_BYTES && new_sz>(size_t)__MAX_BYTES)
		{
			return (realloc(p,new_sz));
		}
		if (ROUND_UP(old_sz)==ROUND_UP(new_sz))
		{
			return p;
		}

		result = Allocate(new_sz);
		copy_sz = new_sz > old_sz ? old_sz : new_sz;
		memcpy(result,p,copy_sz);
		Deallocate(p,old_sz);
		return result;
	}
};
//static 成员的定义与初值设定
template<bool threads,int inst>
typename __Default_Alloc_Template<threads, inst>::Obj* volatile
__Default_Alloc_Template<threads, inst>\
::Free_List[__NFREELISTS] = {0};

template<bool threads, int inst>
char *__Default_Alloc_Template<threads, inst>::_start_free = 0;

template<bool threads, int inst>
char *__Default_Alloc_Template<threads, inst>::_end_free = 0;

template<bool threads, int inst>
size_t __Default_Alloc_Template<threads, inst>::_heap_size = 0;

typedef __Default_Alloc_Template<0, 0> Default_Alloc;

///////////////////////////////////////////////////////////////////////
//配置器的标准接口
template<class T, class Alloc>
class Simple_Alloc
{

public:
	static T *Allocate(size_t n)
	{
		return 0 == n ? 0 : (T*)Alloc::Allocate(n * sizeof (T));
	}
	static T *Allocate(void)
	{
		return (T*)Alloc::Allocate(sizeof (T));
	}
	static void Deallocate(T *p, size_t n)
	{
		if (0 != n) Alloc::Deallocate(p, n * sizeof (T));
	}
	static void Deallocate(T *p)
	{
		Alloc::Deallocate(p, sizeof (T));
	}
};


//通过__TRACE_DEBUG做白盒测试

//测试内存池一级、二级空间配置器

void Alloc_Test1()
{
	//测试一级配置器
	cout << "测试调用一级空间配置器分配内存" << endl;
	char* p1 = Simple_Alloc<char, Malloc_Alloc>::Allocate(129);
	Simple_Alloc<char, Malloc_Alloc>::Deallocate(p1,129);

	cout << "测试调用二级空间配置器分配内存" << endl;
	
	/*char* p2 = Simple_Alloc<char, Default_Alloc>::Allocate(128);
	char* p3 = Simple_Alloc<char, Default_Alloc>::Allocate(128);
	char* p4 = Simple_Alloc<char, Default_Alloc>::Allocate(128);
	char* p5 = Simple_Alloc<char, Default_Alloc>::Allocate(128);

	Simple_Alloc<char, Default_Alloc >::Deallocate(p2, 128);
	Simple_Alloc<char, Default_Alloc>::Deallocate(p3, 128);
	Simple_Alloc<char, Default_Alloc>::Deallocate(p4, 128);
	Simple_Alloc<char, Default_Alloc>::Deallocate(p5, 128);*/

	/*
		测试第1次分配
	【alloc.h:295】__Default_Alloc_Template<0,0>::Allocate(n:128)
	【alloc.h:175】__Default_Alloc_Template<0,0>::Refill(n:128)
	【alloc.h:207】__Default_Alloc_Template<0,0>::Chunk_Alloc(size: 128, nobjs: 20)
	【alloc.h:251】__Default_Alloc_Template<0,0>::Chunk_Alloc内存池空间不足，系统堆分配5120 bytes内存
	【alloc.h:207】__Default_Alloc_Template<0,0>::Chunk_Alloc(size: 128, nobjs: 20)
	【alloc.h:218】__Default_Alloc_Template<0,0>::Chunk_Alloc内存池内存满足分配20个对象
	*/
	/*
		测试第21次分配
	【alloc.h:295】__Default_Alloc_Template<0,0>::Allocate(n:128)
	【alloc.h:175】__Default_Alloc_Template<0,0>::Refill(n:128)
	【alloc.h:207】__Default_Alloc_Template<0,0>::Chunk_Alloc(size: 128, nobjs: 20)
	【alloc.h:218】__Default_Alloc_Template<0,0>::Chunk_Alloc内存池内存满足分配20个对象
	*/
	for (int i = 0; i < 41;++i)
	{
		printf("测试第%d次分配\n",i+1);
		char* p = Simple_Alloc<char, Default_Alloc>::Allocate(128);
	}
	 
	/* 
		测试第41次分配
	【alloc.h:295】__Default_Alloc_Template<0,0>::Allocate(n:128)
	【alloc.h:175】__Default_Alloc_Template<0,0>::Refill(n:128)
	【alloc.h:207】__Default_Alloc_Template<0,0>::Chunk_Alloc(size: 128, nobjs: 20)
	【alloc.h:251】__Default_Alloc_Template<0,0>::Chunk_Alloc内存池空间不足，系统堆分配5440 bytes内存
	【alloc.h:207】__Default_Alloc_Template<0,0>::Chunk_Alloc(size: 128, nobjs: 20)
	【alloc.h:218】__Default_Alloc_Template<0,0>::Chunk_Alloc内存池内存满足分配20个对象
	*/
 
}

void Alloc_Test2()
{
	cout << "测试内存池空间不足分配"<<endl;
	char *p1 = Simple_Alloc<char, Default_Alloc>::Allocate(8);
	char *p2 = Simple_Alloc<char, Default_Alloc>::Allocate(8);
	cout << "测试内存池空间不足，系统堆进行分配" << endl;
	char *p3 = Simple_Alloc<char, Default_Alloc>::Allocate(12);

}

void Alloc_Test3()
{
	cout << "测试系统堆内存耗尽" << endl;
	Simple_Alloc<char, Default_Alloc>::Allocate(1024 * 1024 * 1024);
	Simple_Alloc<char, Default_Alloc>::Allocate(1024 * 1024 * 512);
	Simple_Alloc<char, Default_Alloc>::Allocate(1024 * 1024*200);
	 
	for (int i = 0; i < 100000   ;++i)
	{
		char *p1 = Simple_Alloc<char, Default_Alloc>::Allocate(128);

	}

}
void Alloc_Test()
{
	Alloc_Test1();
	Alloc_Test2();
	Alloc_Test3();
}