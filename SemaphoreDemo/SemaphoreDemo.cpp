/************************************************************************/
/* 
	信号量用途：调度线程
	信号量可以计算资源当前剩余量,并根据当前剩余量与零比较,来决定信号量是处于触发状态或是未触发状态，
*/
/************************************************************************/

#include <stdio.h>
#include <process.h>
#include <windows.h>
long g_nNum;
unsigned int __stdcall Fun(void *pPM);
const int THREAD_NUM = 100;
//信号量与关键段
HANDLE            g_hThreadParameter;
CRITICAL_SECTION  g_csThreadCode;

int main()
{
	printf("     经典线程同步信号量Semaphore\n");

	//初始化信号量和关键段
	g_hThreadParameter = CreateSemaphore(NULL, 0, 1, NULL);//当前个资源，最大允许个同时访问
	InitializeCriticalSection(&g_csThreadCode);


	DWORD  dwTime = ::GetTickCount();


	HANDLE  handle[THREAD_NUM];	
	g_nNum = 0;
	int i = 0;
	while (i < THREAD_NUM) 
	{
		handle[i] = (HANDLE)_beginthreadex(NULL, 0, Fun, &i, 0, NULL);
		WaitForSingleObject(g_hThreadParameter, INFINITE);//等待信号量>0
		++i;
	}
	
	// WaitForMultipleObjects 有上限
	int tempNumThreads = THREAD_NUM; 
	int tempMax = 0; 
	while( tempNumThreads >= MAXIMUM_WAIT_OBJECTS ) 
	{ 
		tempNumThreads -= MAXIMUM_WAIT_OBJECTS; 
		WaitForMultipleObjects( MAXIMUM_WAIT_OBJECTS, &handle[ tempMax], TRUE, INFINITE); 
		tempMax += MAXIMUM_WAIT_OBJECTS; 
	} 
	WaitForMultipleObjects( tempNumThreads, &handle[ tempMax ], TRUE, INFINITE);

	//销毁信号量和关键段
	DeleteCriticalSection(&g_csThreadCode);
	CloseHandle(g_hThreadParameter);
	for (i = 0; i < THREAD_NUM; i++)
		CloseHandle(handle[i]);

	printf("耗时 %d \n",::GetTickCount() - dwTime);

	getchar();
	return 0;
}

unsigned int __stdcall Fun(void *pPM)
{
	int nThreadNum = *(int *)pPM;
	//信号量++。这句话放这里，导致线程是并发的，执行结果是乱序的
	ReleaseSemaphore(g_hThreadParameter, 1, NULL);
	
	Sleep(50);//some work should to do

	EnterCriticalSection(&g_csThreadCode);
	++g_nNum;
	Sleep(0);//some work should to do
	printf("线程编号为%d  全局资源值为%d\n", nThreadNum, g_nNum);
	LeaveCriticalSection(&g_csThreadCode);

	// 放这里，线程结束了，才会调用下一个线程。耗时非常长
	//ReleaseSemaphore(g_hThreadParameter, 1, NULL);
	return 0;
}