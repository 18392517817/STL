#pragma once


//��һ��������

//�ڴ����ʧ�ܺ�����handler����
typedef void(*ALLOC_FUN)();
template<int init>
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