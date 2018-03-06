/************************************************************************/
/* 
	�ź�����;�������߳�
	�ź������Լ�����Դ��ǰʣ����,�����ݵ�ǰʣ��������Ƚ�,�������ź����Ǵ��ڴ���״̬����δ����״̬��
*/
/************************************************************************/

#include <stdio.h>
#include <process.h>
#include <windows.h>
long g_nNum;
unsigned int __stdcall Fun(void *pPM);
const int THREAD_NUM = 100;
//�ź�����ؼ���
HANDLE            g_hThreadParameter;
CRITICAL_SECTION  g_csThreadCode;

int main()
{
	printf("     �����߳�ͬ���ź���Semaphore\n");

	//��ʼ���ź����͹ؼ���
	g_hThreadParameter = CreateSemaphore(NULL, 0, 1, NULL);//��ǰ����Դ����������ͬʱ����
	InitializeCriticalSection(&g_csThreadCode);


	DWORD  dwTime = ::GetTickCount();


	HANDLE  handle[THREAD_NUM];	
	g_nNum = 0;
	int i = 0;
	while (i < THREAD_NUM) 
	{
		handle[i] = (HANDLE)_beginthreadex(NULL, 0, Fun, &i, 0, NULL);
		WaitForSingleObject(g_hThreadParameter, INFINITE);//�ȴ��ź���>0
		++i;
	}
	
	// WaitForMultipleObjects ������
	int tempNumThreads = THREAD_NUM; 
	int tempMax = 0; 
	while( tempNumThreads >= MAXIMUM_WAIT_OBJECTS ) 
	{ 
		tempNumThreads -= MAXIMUM_WAIT_OBJECTS; 
		WaitForMultipleObjects( MAXIMUM_WAIT_OBJECTS, &handle[ tempMax], TRUE, INFINITE); 
		tempMax += MAXIMUM_WAIT_OBJECTS; 
	} 
	WaitForMultipleObjects( tempNumThreads, &handle[ tempMax ], TRUE, INFINITE);

	//�����ź����͹ؼ���
	DeleteCriticalSection(&g_csThreadCode);
	CloseHandle(g_hThreadParameter);
	for (i = 0; i < THREAD_NUM; i++)
		CloseHandle(handle[i]);

	printf("��ʱ %d \n",::GetTickCount() - dwTime);

	getchar();
	return 0;
}

unsigned int __stdcall Fun(void *pPM)
{
	int nThreadNum = *(int *)pPM;
	//�ź���++����仰����������߳��ǲ����ģ�ִ�н���������
	ReleaseSemaphore(g_hThreadParameter, 1, NULL);
	
	Sleep(50);//some work should to do

	EnterCriticalSection(&g_csThreadCode);
	++g_nNum;
	Sleep(0);//some work should to do
	printf("�̱߳��Ϊ%d  ȫ����ԴֵΪ%d\n", nThreadNum, g_nNum);
	LeaveCriticalSection(&g_csThreadCode);

	// ������߳̽����ˣ��Ż������һ���̡߳���ʱ�ǳ���
	//ReleaseSemaphore(g_hThreadParameter, 1, NULL);
	return 0;
}