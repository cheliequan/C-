#include "stdafx.h"
//#include <winsvc.h>
#include <stdio.h>
#include "ServiceCtrl.h"

//HANDLE					hMutex;
//LPPROCESSDATA			lpProcessDataHead;
//LPPROCESSDATA			lpProcessDataEnd;
SERVICE_STATUS			ServiceStatus;
SERVICE_STATUS_HANDLE	ServiceStatusHandle;

::Program *process = new Program;
HANDLE hThread;
DWORD dwThreadID;
// 服务初始化
int WINAPI InitService() 
{ 
	//CBase::Instance()->WriteLog("init service"); 
    return 0; 
}

DWORD WINAPI MonitorThread(void *pArg)
{
	while(1){
		Sleep(1000);
		process->InjectNewProcess();
		//Releaseinfo("xun huan yi ci");
	}
}

void WINAPI CmdStart(DWORD dwArgc, LPTSTR* lpArgv)	// 执行操作创建服务线程
{
	Releaseinfo("cmdstart qi dong");
	ServiceStatus.dwServiceType			= SERVICE_WIN32;
	ServiceStatus.dwCurrentState		= SERVICE_START_PENDING;
	ServiceStatus.dwControlsAccepted	= SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwWin32ExitCode			= 0;
	ServiceStatus.dwCheckPoint				= 0;
	ServiceStatus.dwWaitHint				= 0;
	ServiceStatusHandle = RegisterServiceCtrlHandler(_T("MonitorSCPHost"), CmdControl);
	if(ServiceStatusHandle == NULL)
	{
		Releaseinfo("ServiceStatusHandle == NULL");
		return;
	}

	int error = InitService(); 

	if (error < 0) 
	{
	  // Initialization failed
	  Releaseinfo("error < 0");
	  ServiceStatus.dwCurrentState =  SERVICE_STOPPED; 
	  ServiceStatus.dwWin32ExitCode = -1; 
	  SetServiceStatus(ServiceStatusHandle, &ServiceStatus); 
	  
	  return; 
	} 

	ServiceStatus.dwCurrentState	= SERVICE_RUNNING;
	ServiceStatus.dwCheckPoint		= 0;
	ServiceStatus.dwWaitHint		= 0;
	
	if(SetServiceStatus(ServiceStatusHandle, &ServiceStatus) == 0)
	{
		Releaseinfo("SetServiceStatus(ServiceStatusHandle, &ServiceStatus) == 0");
		return;
	}
	process->Init();
	
	hThread=CreateThread(NULL,0,MonitorThread,0,NULL,&dwThreadID);//
	if (0 == dwThreadID){
		Releaseinfo("线程创建失败");
		return ;
	}
	//CloseHandle(hThread); 
	return;
}

void WINAPI CmdControl(DWORD dwCode)	// 接收各种控制命令
{
	BOOL code;
	switch(dwCode)
	{
	case SERVICE_CONTROL_STOP: 
		Releaseinfo("SERVICE_CONTROL_STOP");
		code = TerminateThread(hThread, dwThreadID);
		if (!code){
			Releaseinfo("TerminateThread error!");
		}
		process->UnInjectAll();
		ServiceStatus.dwWin32ExitCode = 0; 
		ServiceStatus.dwCurrentState = SERVICE_STOPPED; 
		SetServiceStatus (ServiceStatusHandle, &ServiceStatus);
		return;

	case SERVICE_CONTROL_SHUTDOWN:
		Releaseinfo("SERVICE_CONTROL_SHUTDOWN");
		code = TerminateThread(hThread, dwThreadID);
		if (!code){
			Releaseinfo("TerminateThread error!");
		}
		process->UnInjectAll();
		ServiceStatus.dwWin32ExitCode = 0; 
		ServiceStatus.dwCurrentState = SERVICE_STOPPED; 
		SetServiceStatus (ServiceStatusHandle, &ServiceStatus);
		return;

	default:
		break;
	}
    SetServiceStatus (ServiceStatusHandle, &ServiceStatus);
	return;
}