#include "stdafx.h"
#include "windows.h"
#include "tlhelp32.h"

void Releaseinfo(char *infostr);

struct ProcessNews{
	LONG ProcessID;
	char ProcessName[260];
	ProcessNews *Next;
};

class Program
{
public:
	Program(void);
	~Program(void);
	BOOL GetNext(void);
	static Program * Program::SharedInstance();
	BOOL GetFirst(void);
private:
	HANDLE hProcessSnap;
	TCHAR *ProcessName;  //��������
	DWORD ProcessID;       //������ID
	static Program *_instance;
	FILETIME CreateTime,ExitTime,lpKernelTime,lpUserTime;
	ProcessNews *ProcessNewList,*ProcessOldList,*ProcessCreateList;
public:
	BOOL Init(void);					//��ʼ��
	BOOL InjectDll(DWORD dwPID);
	BOOL UnInjectDll(DWORD dwPID);
	BOOL UnInjectAll();
	BOOL ReleaseProcessList();
	BOOL InjectAllProcessDll();
	char *GetProcessNameById(long ProcessId);
	BOOL IsSystemProcess(DWORD ProcessId, char *ProcessName);
	BOOL InjectNewProcess();
	BOOL IsOldProcess(LONG ID);
	ProcessNews *GetProcessList(void);
};