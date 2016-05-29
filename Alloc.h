#pragma once


//��һ��������

//�ڴ����ʧ�ܺ�����handler����
typedef void(*ALLOC_FUN)();
template<int inst>
class __Malloc_Alloc_Template
{
private:
	//�����ڴ治��
	//static void(*__Malloc_Alloc_Oom_Handler)();
	static ALLOC_FUN __Malloc_Alloc_Oom_Handler;
	
	static void* Oom_Malloc(size_t n)
	{
		//void(*my_malloc_handler)();
		ALLOC_FUN my_malloc_handler;
		void *result;

		//1������ɹ���ֱ�ӷ���
		//2������ʧ�ܣ�����Ƿ����my_malloc_handler
		//�У������Ժ��ٷ��䣬�����ظ�
		//�ޣ�ֱ�ӷ��أ���������exit(-1)
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

		//1������ɹ���ֱ�ӷ���
		//2������ʧ�ܣ�����Ƿ����my_malloc_handler
		//�У������Ժ��ٷ��䣬�����ظ�
		//�ޣ�ֱ�ӷ��أ���������exit(-1)
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
	//����C++��set_new_handler();
	//ָ���Լ���out-of-memory handler
	static void(*set_malloc_handler(void(*f)()))()
	{
		void(*old)() = __Malloc_Alloc_Oom_Handler;
		__Malloc_Alloc_Oom_Handler = f;
		return(old);
	}

};

//�����ڴ�ʧ�ܣ��������ľ������ָ��
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


 //�����ռ�������


template <bool threads, int inst>
class __Default_Alloc_Template {

private:
	// Really we should use static const int x = N
	// instead of enum { x = N }, but few compilers accept the former.
 
	enum { __ALIGN = 8 };//���л�׼ֵ
	enum { __MAX_BYTES = 128 };//���ֵ
	enum { __NFREELISTS = __MAX_BYTES / __ALIGN };//����ֵ
 
	static size_t ROUND_UP(size_t bytes) {
		return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
	}
 
	static  size_t FREELIST_INDEX(size_t bytes) {
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
	}
	union Obj {
		union Obj * Free_List_Link;//ָ����һ���ڴ���ָ��
		char Client_Data[1];    /* The client sees this.        */
	};
private:

	static Obj* volatile Free_List[__NFREELISTS];//��������

	// ��ȡ����ڴ���뵽����������
	static void *Refill(size_t n)
	{
		//����n ��bytes���ڴ�
		//����������ж��ٷ������

		int nobjs = 20;
		char* chunk = Chunk_Alloc(n,nobjs);

		//���ֻ���䵽һ�飬ֱ�ӷ�������ڴ�
		if (nobjs == 1)
			return chunk;

		Obj* result;
		Obj* cur;
		size_t index = FREELIST_INDEX(n);
		result = (Obj*)chunk; //������
		
		//��ʣ��Ŀ����ӵ�����������
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
	//���ڴ���з������ڴ�
	static char *Chunk_Alloc(size_t size, int &nobjs)
	{
		char* result;
		size_t bytesNeed = size*nobjs;
		size_t bytesLeft = _end_free - _start_free;

		//1���ڴ�ص��ڴ���㹻��bytesLeft>=bytesNeed,��ֱ�Ӵ��ڴ������ȡ
		//2���ڴ�����ڴ治�㣬����bytesLeft>=size,��ֱ����ȡ����
		//3���ڴ�����ڴ治�㣬���ϵͳ�ѷ������ڴ浽�ڴ��
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
			//���ڴ���л���ʣ���ڴ棬����ͷ�嵽���ʵ���������
			if (bytesLeft > 0)
			{
				size_t index = FREELIST_INDEX(bytesLeft);
				((Obj*)_start_free)->Free_List_Link = Free_List[index];
				Free_List[index] = (Obj*)_start_free;
				_start_free = NULL;
			}

			//��ϵͳ�ѷ�������+�ѷ����_heap_size/8���ڴ浽�ڴ��

			size_t bytesToGet = 2 * bytesNeed + ROUND_UP(_heap_size >> 4);
			_start_free = (char*)malloc(bytesToGet);

			//�����ϵͳ�����ڴ����ʧ�ܣ����Ե����������д�һ���Ľڵ��з���
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
				//���ĵ���
				//�ٵ�һ���������п���û�����ô�����ڴ棬

				_start_free = (char*)Malloc_Alloc::Allocate(bytesToGet);

			}
			_heap_size += bytesToGet;
			_end_free = _start_free += bytesToGet;

			//�ݹ���û�ȡ�ڴ�
			return Chunk_Alloc(size, nobjs);

		}
		return result;
	}

	// Chunk allocation state.
	static char *_start_free;//�ڴ�ؿ�ʼ�ĵط�
	static char *_end_free;//�ڴ�ؽ����ĵط�
	static size_t _heap_size;
public:

	/* n must be > 0      */
	static void * Allocate(size_t n)
	{
		
		/*
		//����stlԴ��
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

		//��n > 128 ��ֱ�ӵ���һ���ռ�������
		//�����ڶ����������л�ȡ

		if (n>__MAX_BYTES)
		{
			return Malloc_Alloc::Allocate(n);
		}
		
		size_t index = FREELIST_INDEX(n);
		void* ret = NULL;

		//1���������������û���ڴ棬����Refill()�������
		//2������У�����һ���ڵ���ڴ�
		
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
//static ��Ա�Ķ������ֵ�趨
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

 