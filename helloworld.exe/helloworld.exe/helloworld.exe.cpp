// helloworld.exe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"   
#include <windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
	int i = 0;
	for(;i<10;i++)
	{
		printf("hello world !\n");
		Sleep(200);
	}

	return 0;
}

