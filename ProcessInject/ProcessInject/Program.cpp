/************************************************************
** copyright(C) mjw lgy
** author      : mjw,lgy
** Date        : 2015-03-24
** description : ע���ϵͳ���̣�hookע�����ļ�����
************************************************************/
#include "stdafx.h"
#include "Program.h"

void Releaseinfo(char *infostr)
{
	char Time[9];			 // ʱ��
	char Date[11];			 //����
	::GetTimeFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP|TIME_FORCE24HOURFORMAT,NULL,"HH':'mm':'ss",Time,9);
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyy'-'MM'-'dd",Date,11);
	FILE *fp;
	fp = fopen("C:\\RegAndFile\\ProcessInject.log","a+");
	if(fp == NULL)
	{
		MessageBox(NULL, "file open fail\n", "!", 0);
		return ;
	}
	fprintf(fp,"%s,%s,%s\r\n",infostr,Date,Time);
	fclose(fp);
}

Program *Program::_instance = NULL;
LPVOID pRemoteBuf = NULL;
/*********************************************
** ע��ϵͳ����
/********************************************/
BOOL Program::InjectDll(DWORD dwPID){
	char szDllPath[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH,szDllPath);	
	strcat(szDllPath, "\\testFileDatabase.dll");
	//printf("ע��dllΪ%s\n", szDllPath);
	//Releaseinfo(szDllPath);
	////////////////////////////////////////////////////////////////////////
	// ����ϵͳȨ��
	HANDLE hToken;
    LUID uID;
    TOKEN_PRIVILEGES tp;
    OpenProcessToken(GetCurrentProcess(),TOKEN_ALL_ACCESS,&hToken);
    LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&uID);
 
    tp.PrivilegeCount=1;
    tp.Privileges[0].Luid=uID;
    tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
 
    AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL);
 
	if (hToken != NULL)
		CloseHandle(hToken);
	else{
		printf("access failed!\n");
		return FALSE;
	}

	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	HMODULE hMod = NULL;

	//ע��
	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
	LPTHREAD_START_ROUTINE pThreadProc;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))){
		_tprintf("OpenProcess(%d)failed![%d]\n", dwPID, GetLastError());
		return FALSE;
	}
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	if (!WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL)){
		printf("writeprocessmemory failed!\n");
		return FALSE;
	}
	hMod = GetModuleHandle("kernel32.dll");
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
	if (!hThread){
		_tprintf("create thread failed!\n");
		return FALSE;
	}

	WaitForSingleObject(hThread, INFINITE);
	if (pRemoteBuf != NULL)
		VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
	if (hThread != NULL)
		CloseHandle(hThread);
	if (hProcess != NULL)
		CloseHandle(hProcess);
	Releaseinfo("inject over");
	return TRUE;
}
/*********************************************
**ж��dll
*********************************************/
BOOL Program::UnInjectDll(DWORD dwPID){
	char szDllPath[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH,szDllPath);	
	strcat(szDllPath, "\\testFileDatabase.dll");
	//printf("ע��dllΪ%s\n", szDllPath);
	//Releaseinfo(szDllPath);
	////////////////////////////////////////////////////////////////////////
	// ����ϵͳȨ��
	HANDLE hToken;
    LUID uID;
    TOKEN_PRIVILEGES tp;
    OpenProcessToken(GetCurrentProcess(),TOKEN_ALL_ACCESS,&hToken);
    LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&uID);
 
    tp.PrivilegeCount=1;
    tp.Privileges[0].Luid=uID;
    tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
 
    AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(tp),NULL,NULL);
 
	if (hToken != NULL)
		CloseHandle(hToken);
	else{
		printf("access failed!\n");
		return FALSE;
	}

	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	HMODULE hMod = NULL;
	//LPVOID pRemoteBuf = NULL;

	//ע��
	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
	LPTHREAD_START_ROUTINE pThreadProc;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))){
		_tprintf("OpenProcess(%d)failed![%d]\n", dwPID, GetLastError());
		return FALSE;
	}
	HANDLE hthSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hthSnapshot == INVALID_HANDLE_VALUE){
		Releaseinfo("CreateToolhelp32Snapshot error!");
	}
	MODULEENTRY32 me = {sizeof(me)};
	BOOL bMoreMods = Module32First(hthSnapshot, &me);
	for (; bMoreMods; bMoreMods = Module32Next(hthSnapshot, &me)){
		if (strcmp(me.szModule, "testFileDatabase.dll") == 0 || strcmp(me.szExePath, "testFileDatabase.dll") == 0)
			break;
	}
	/*pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	if (!WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL)){
		printf("writeprocessmemory failed!\n");
		return FALSE;
	}*/
	hMod = GetModuleHandle("kernel32.dll");
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "FreeLibrary");
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, me.modBaseAddr, 0, NULL);
	if (!hThread){
		_tprintf("create thread failed!\n");
		return FALSE;
	}

	WaitForSingleObject(hThread, INFINITE);
	if (pRemoteBuf != NULL)
		VirtualFreeEx(hProcess, pRemoteBuf, 0, MEM_RELEASE);
	if (hThread != NULL)
		CloseHandle(hThread);
	if (hProcess != NULL)
		CloseHandle(hProcess);
	Releaseinfo("inject over");
	return TRUE;
}
/*********************************************
** ��̬����
/********************************************/
Program * Program::SharedInstance()
{
	if (_instance == NULL)
	{
		_instance = new Program;
	}
	return _instance;
}
/*********************************************
** ��ʼ����ñ�����������ID
/********************************************/
Program::Program(void)
{
	ProcessName = GetProcessNameById(GetCurrentProcessId());
	ProcessID = GetCurrentProcessId();
	ProcessName = NULL;
	this->ProcessOldList = this->ProcessNewList = this->ProcessCreateList = NULL;
}

/*********************************************
** ��ʼ������������ע�뵽���еĽ�����
/********************************************/
BOOL Program::Init()
{
	ProcessOldList = GetProcessList();
	ProcessNews *tPro = NULL;
	char log[100000];
	for(tPro = ProcessOldList; tPro != NULL; tPro = tPro->Next)
	{
		if (IsSystemProcess(tPro->ProcessID, tPro->ProcessName))
			continue;
		if (InjectDll(tPro->ProcessID))
			sprintf(log, "��ʼ������ID:%d, %s\n �ɹ���",tPro->ProcessID, tPro->ProcessName);
			//printf("��ʼ��inject ����ID:%d, %s\n �ɹ���",tPro->ProcessID, tPro->ProcessName);
		else{
			sprintf(log, "��ʼ��inject ����ID:%d, %s\n ʧ�ܣ�",tPro->ProcessID, tPro->ProcessName);
		}
		Releaseinfo(log);
	}

	return TRUE;
}
/*********************************************
** ��ʼ������������ע�뵽���еĽ�����
/********************************************/
BOOL Program::UnInjectAll()
{
	Releaseinfo("UnInjectAll");
	//ProcessOldList = GetProcessList();
	ProcessNews *tPro = NULL;
	char log[100000];
	for(tPro = ProcessOldList; tPro != NULL; tPro = tPro->Next)
	{
		if (IsSystemProcess(tPro->ProcessID, tPro->ProcessName))
			continue;
		if (UnInjectDll(tPro->ProcessID))
			sprintf(log, "uninject����ID:%d, %s\n �ɹ���",tPro->ProcessID, tPro->ProcessName);
			//printf("��ʼ��inject ����ID:%d, %s\n �ɹ���",tPro->ProcessID, tPro->ProcessName);
		else{
			sprintf(log, "uninject ����ID:%d, %s\n ʧ�ܣ�",tPro->ProcessID, tPro->ProcessName);
		}
		Releaseinfo(log);
	}

	return TRUE;
}
/*********************************************
** ��ؽ��̴�����ע�뵽�µĽ���
/********************************************/
BOOL Program::InjectNewProcess()
{
	ProcessNewList = GetProcessList();
	ProcessNews *tPro = NULL;
	char log[100000];

	for (tPro = ProcessNewList; tPro != NULL; tPro = tPro->Next)
	{
		if (IsOldProcess(tPro->ProcessID))
			continue;
		if (InjectDll(tPro->ProcessID))
			sprintf(log, "xin inject ����ID:%d, %s\n �ɹ���",tPro->ProcessID, tPro->ProcessName);
		else{
			sprintf(log, "xin inject ����ID:%d, %s\n ʧ�ܣ�",tPro->ProcessID, tPro->ProcessName);
		}
		Releaseinfo(log);
	}

	ReleaseProcessList();
	ProcessOldList = ProcessNewList;
	ProcessNewList = NULL;

	return TRUE;
}

/*********************************************
** �ж�һ���ǲ����Ѿ�ע����Ľ���
/********************************************/
BOOL Program::IsOldProcess(LONG ID)
{
	ProcessNews *oPro;

	for (oPro = ProcessOldList; oPro != NULL; oPro = oPro->Next)
	{
		if (oPro->ProcessID == ID)
			return TRUE;
	}
	return FALSE;
}

Program::~Program(void)
{
}

/*********************************************
** ͨ��ID��ý�������
/********************************************/
char * Program::GetProcessNameById(long ProcessId)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,ProcessId);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		////printf("CreateToolHelp32Snap Failed!\n");
		return "";
	}
	MODULEENTRY32 me32 = {0}; 
	me32.dwSize = sizeof(MODULEENTRY32);
	BOOL bMore = ::Module32First(hProcessSnap,&me32);
	CloseHandle(hProcessSnap);
	return me32.szModule;
}

/*********************************************
** ���ϵͳ����
/********************************************/
ProcessNews * Program::GetProcessList(void)
{
	HANDLE hProcessSnapNew = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); 
	if (hProcessSnapNew == INVALID_HANDLE_VALUE)  
	{  
		return NULL;  
	}
	PROCESSENTRY32 pe32;
	ProcessNews *tPro;
	ProcessNews *NewPro = NULL;
	HANDLE handle;
	pe32.dwSize = sizeof(pe32);  
	BOOL bMore = ::Process32First(hProcessSnapNew,&pe32);
	while(bMore)
	{
		if(NewPro == NULL)
		{	
			tPro = NewPro = new ProcessNews;
		}
		else
		{
			tPro->Next = new ProcessNews;
			tPro = tPro->Next;
		}
		tPro->ProcessID = pe32.th32ProcessID;
		strcpy(tPro->ProcessName, GetProcessNameById(pe32.th32ProcessID));
		bMore = ::Process32Next(hProcessSnapNew,&pe32);
		
	}
	tPro->Next = NULL;
	if (hProcessSnapNew != INVALID_HANDLE_VALUE)
		CloseHandle(hProcessSnapNew);
	return NewPro;
}

/*********************************************
** �ͷžɵ�ϵͳ�����б�
/********************************************/
BOOL Program::ReleaseProcessList()
{
	ProcessNews *tPro = ProcessOldList->Next;

	while (tPro){
		delete ProcessOldList;
		ProcessOldList = tPro;
		tPro = tPro->Next;
	}
	delete ProcessOldList;

	return TRUE;
}

/*********************************************
** �ж��ǲ���hook�����Ľ��̣�����
/********************************************/
BOOL Program::IsSystemProcess(DWORD ProcessId, char *ProcessName)
{
	if (ProcessId == 0 || ProcessId == 4 || ProcessId == ProcessID || !strcmp(ProcessName, "csrss.exe"))
		return TRUE;
	return FALSE;
}
