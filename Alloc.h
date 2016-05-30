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
//���ڵ���׷�ݵ�trace log
inline static void __trace_debug(const char* function,
	const char* filename,int line,char* format, ...)
{
#ifdef __DEBUG__
	//������ú�������Ϣ
	fprintf(stdout,"��%s:%d��%s",GetFileName(filename).c_str(),line,function);
	//����û����trace��Ϣ
	va_list args;
	_crt_va_start(args,format);
	vfprintf(stdout,format,args);
	_crt_va_end(args);
#endif
}
/*va_list��������һ������������֪�������Ŀɱ�����б���ʵ����
һ���ַ���������va_list�ű�����Ϊ�ַ���ָ�룬���������������
һ��ָ������б���ַ���ָ����������磺va_list ap;//ap:arguement pointer
va_start(ap,v),���ĵ�һ��������ָ��ɱ�����ַ����ı�����
�ڶ��������ǿɱ���������ĵ�һ��������ͨ������ָ���ɱ�����б�
�в����ĸ�����
va_arg(ap,t),���ĵ�һ������ָ��ɱ�����ַ����ı������ڶ��������ǿɱ���������͡�
va_end(ap) ���ڽ���ſɱ�����ַ����ı�����գ���ֵΪNULL).
*/
#define __TRACE_DEBUG(...)\
	__trace_debug(__FUNCTION__,__FILE__,__LINE__,__VA_ARGS__);
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

		//1������ɹ���ֱ�ӷ���
		//2������ʧ�ܣ�����Ƿ����my_malloc_handler
		//�У������Ժ��ٷ��䣬�����ظ�
		//�ޣ�ֱ�ӷ��أ���������exit(-1)
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


///////////////////////////////////////////////////////////////////////

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
		__TRACE_DEBUG("(n:%u)\n", n);

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
		__TRACE_DEBUG("(size: %u, nobjs: %d)\n",size,nobjs);

		char* result;
		size_t bytesNeed = size*nobjs;
		size_t bytesLeft = _end_free - _start_free;//�ڴ��ʣ��ռ�

		//1���ڴ�ص��ڴ���㹻��bytesLeft>=bytesNeed,��ֱ�Ӵ��ڴ������ȡ
		//2���ڴ�����ڴ治�㣬����bytesLeft>=size,��ֱ����ȡ����
		//3���ڴ�����ڴ治�㣬���ϵͳ�ѷ������ڴ浽�ڴ��
		if (bytesLeft >= bytesNeed)
		{
			__TRACE_DEBUG("�ڴ���ڴ��������%d������\n",nobjs);

			result = _start_free;
			_start_free += bytesNeed;
			//return result;
		}
		else if (bytesLeft >= size)
		{
			__TRACE_DEBUG("�ڴ�ز�������%d������ֻ�ܷ���%d������\n",nobjs,bytesLeft/size);

			nobjs = bytesLeft / size;
			result = _start_free;
			_start_free += nobjs*size;
			//return result;
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

				__TRACE_DEBUG("���ڴ����ʣ��Ŀռ䣬�����Free_List[%d]\n",index);
			}

			//��ϵͳ�ѷ�������+�ѷ����_heap_size/8���ڴ浽�ڴ��
			 
			size_t bytes_To_Get = 2 * bytesNeed + ROUND_UP(_heap_size >> 4);
			_start_free = (char*)malloc(bytes_To_Get);

			__TRACE_DEBUG("�ڴ�ؿռ䲻�㣬ϵͳ�ѷ���%d bytes�ڴ�\n",bytes_To_Get);
			//�����ϵͳ�����ڴ����ʧ�ܣ����Ե����������д�һ���Ľڵ��з���
			if (_start_free == NULL)
			{
				__TRACE_DEBUG("ϵͳ����û�пռ䣬�뵽���������п���\n");
	
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
				//���ĵ���
				//�ٵ�һ���������п���û�����ô�����ڴ棬
				__TRACE_DEBUG("ϵͳ�Ѻ������������ڴ棬ת��һ���ռ�������\n");
				
				_end_free = 0;
				_start_free = (char*)Malloc_Alloc::Allocate(bytes_To_Get);

			}
			_heap_size +=bytes_To_Get;
			_end_free = _start_free +bytes_To_Get;

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
		__TRACE_DEBUG("(n:%u)\n", n);

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
			__TRACE_DEBUG("��������ȡ�ڴ棺Free_List[%d]\n",index);
	
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
//static ��Ա�Ķ������ֵ�趨
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
//�������ı�׼�ӿ�
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


//ͨ��__TRACE_DEBUG���׺в���

//�����ڴ��һ���������ռ�������

void Alloc_Test1()
{
	//����һ��������
	cout << "���Ե���һ���ռ������������ڴ�" << endl;
	char* p1 = Simple_Alloc<char, Malloc_Alloc>::Allocate(129);
	Simple_Alloc<char, Malloc_Alloc>::Deallocate(p1,129);

	cout << "���Ե��ö����ռ������������ڴ�" << endl;
	
	/*char* p2 = Simple_Alloc<char, Default_Alloc>::Allocate(128);
	char* p3 = Simple_Alloc<char, Default_Alloc>::Allocate(128);
	char* p4 = Simple_Alloc<char, Default_Alloc>::Allocate(128);
	char* p5 = Simple_Alloc<char, Default_Alloc>::Allocate(128);

	Simple_Alloc<char, Default_Alloc >::Deallocate(p2, 128);
	Simple_Alloc<char, Default_Alloc>::Deallocate(p3, 128);
	Simple_Alloc<char, Default_Alloc>::Deallocate(p4, 128);
	Simple_Alloc<char, Default_Alloc>::Deallocate(p5, 128);*/

	/*
		���Ե�1�η���
	��alloc.h:295��__Default_Alloc_Template<0,0>::Allocate(n:128)
	��alloc.h:175��__Default_Alloc_Template<0,0>::Refill(n:128)
	��alloc.h:207��__Default_Alloc_Template<0,0>::Chunk_Alloc(size: 128, nobjs: 20)
	��alloc.h:251��__Default_Alloc_Template<0,0>::Chunk_Alloc�ڴ�ؿռ䲻�㣬ϵͳ�ѷ���5120 bytes�ڴ�
	��alloc.h:207��__Default_Alloc_Template<0,0>::Chunk_Alloc(size: 128, nobjs: 20)
	��alloc.h:218��__Default_Alloc_Template<0,0>::Chunk_Alloc�ڴ���ڴ��������20������
	*/
	/*
		���Ե�21�η���
	��alloc.h:295��__Default_Alloc_Template<0,0>::Allocate(n:128)
	��alloc.h:175��__Default_Alloc_Template<0,0>::Refill(n:128)
	��alloc.h:207��__Default_Alloc_Template<0,0>::Chunk_Alloc(size: 128, nobjs: 20)
	��alloc.h:218��__Default_Alloc_Template<0,0>::Chunk_Alloc�ڴ���ڴ��������20������
	*/
	for (int i = 0; i < 41;++i)
	{
		printf("���Ե�%d�η���\n",i+1);
		char* p = Simple_Alloc<char, Default_Alloc>::Allocate(128);
	}
	 
	/* 
		���Ե�41�η���
	��alloc.h:295��__Default_Alloc_Template<0,0>::Allocate(n:128)
	��alloc.h:175��__Default_Alloc_Template<0,0>::Refill(n:128)
	��alloc.h:207��__Default_Alloc_Template<0,0>::Chunk_Alloc(size: 128, nobjs: 20)
	��alloc.h:251��__Default_Alloc_Template<0,0>::Chunk_Alloc�ڴ�ؿռ䲻�㣬ϵͳ�ѷ���5440 bytes�ڴ�
	��alloc.h:207��__Default_Alloc_Template<0,0>::Chunk_Alloc(size: 128, nobjs: 20)
	��alloc.h:218��__Default_Alloc_Template<0,0>::Chunk_Alloc�ڴ���ڴ��������20������
	*/
 
}

void Alloc_Test2()
{
	cout << "�����ڴ�ؿռ䲻�����"<<endl;
	char *p1 = Simple_Alloc<char, Default_Alloc>::Allocate(8);
	char *p2 = Simple_Alloc<char, Default_Alloc>::Allocate(8);
	cout << "�����ڴ�ؿռ䲻�㣬ϵͳ�ѽ��з���" << endl;
	char *p3 = Simple_Alloc<char, Default_Alloc>::Allocate(12);

}

void Alloc_Test3()
{
	cout << "����ϵͳ���ڴ�ľ�" << endl;
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