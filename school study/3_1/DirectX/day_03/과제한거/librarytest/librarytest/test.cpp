#pragma comment(lib,"libProject.lib")
#include "libProject.h"
#include <stdio.h>
#include <Windows.h>
//������ ȣ���Ҷ� ���.
//extern "C" __declspec(dllimport) int Multi(int, int);
typedef int (*pDllFunction)(int, int);
pDllFunction pFunction = NULL;

int main()
{
	int a=0, b=0;
	HMODULE hMod = NULL;
	hMod = LoadLibraryA("dllProject.dll");
	if (hMod == NULL)
	{
		printf("Dll load failed\n");
		return 0;
	}
	printf("data1:");
	scanf("%d", &a);
	printf("data2:");
	scanf("%d", &b);
	printf("lib sum:%d\n",Sum(a, b));
	pFunction = (pDllFunction)GetProcAddress(hMod, "Multi");
	printf("data1:");
	scanf("%d", &a);
	printf("data2:");
	scanf("%d", &b);
	printf("mult = %d \n", pFunction(a, b));
	//������ ȣ���Ҷ� ���
	//printf("dll multi:%d\n",Multi(2, 3));
	return 0;
}