#pragma once


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

typedef __Malloc_Alloc_Template<0> Malloc_Alloc;

//template<class T, class Alloc>
//class simple_alloc {
//
//public:
//	static T *allocate(size_t n)
//	{
//		return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof (T));
//	}
//	static T *allocate(void)
//	{
//		return (T*)Alloc::allocate(sizeof (T));
//	}
//	static void deallocate(T *p, size_t n)
//	{
//		if (0 != n) Alloc::deallocate(p, n * sizeof (T));
//	}
//	static void deallocate(T *p)
//	{
//		Alloc::deallocate(p, sizeof (T));
//	}
//};


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
		char* result;
		size_t bytesNeed = size*nobjs;
		size_t bytesLeft = _end_free - _start_free;

		//1、内存池的内存池足够，bytesLeft>=bytesNeed,则直接从内存池中提取
		//2、内存池中内存不足，但够bytesLeft>=size,则直接提取出来
		//3、内存池中内存不足，则从系统堆分配大块内存到内存池
		if (bytesLeft >= bytesNeed)
		{
			result = _start_free;
			_start_free += bytesNeed;
		}
		else if (bytesLeft >= size)
		{
			result = _start_free;
			nobjs = bytesLeft / size;
			_start_free += nobjs*size;
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
			}

			//从系统堆分配两倍+已分配的_heap_size/8的内存到内存池

			size_t bytesToGet = 2 * bytesNeed + ROUND_UP(_heap_size >> 4);
			_start_free = (char*)malloc(bytesToGet);

			//如果在系统堆中内存分配失败，则尝试到自由链表中大一级的节点中分配
			if (_start_free == NULL)
			{
				for (int i = size; i <= __MAX_BYTES; ++i)
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

				_start_free = (char*)Malloc_Alloc::Allocate(bytesToGet);

			}
			_heap_size += bytesToGet;
			_end_free = _start_free += bytesToGet;

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
			Free_List[index] = head->Free_LIst_Link;
			return head;
		}
	}

	/* p may not be 0 */
	static void Deallocate(void *p, size_t n)
	{
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
typename __Default_Alloc_Template<threads, inst>::Obj volatile
__Default_Alloc_Template<threads, inst>\
::Free_List[__Default_Alloc_Template<threads, inst>::__NFREELISTS] = {0};

template<bool threads, int inst>
char *__Default_Alloc_Template<threads, inst>::_start_free = 0;

template<bool threads, int inst>
char *__Default_Alloc_Template<threads, inst>::_end_free = 0;

template<bool threads, int inst>
size_t __Default_Alloc_Template<threads, inst>::_heap_size = 0;

typedef __Default_Alloc_Template<0, 0> Default_Alloc;

 