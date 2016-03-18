// ProcessInject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ServiceCtrl.h"
#include "Shlwapi.h"

#pragma comment(lib, "Shlwapi.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	/*SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = _T("ProcessInject");
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)CmdStart;
	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;
	StartServiceCtrlDispatcher(ServiceTable);*/ 	 
	if (!PathIsDirectory("C:\\RegAndFile\\Reg"))
	{
		::CreateDirectory("C:\\RegAndFile\\Reg", NULL);
	}
	if (!PathIsDirectory("C:\\RegAndFile\\File"))
	{
		::CreateDirectory("C:\\RegAndFile\\File", NULL);
	}
	::Program *process = new Program;
	//process->InjectDll(3124);
	process->Init();
	//process->UnInjectAll();
	//process->UnInjectDll(696);
	//process->Init();
	//
	//while(1){
	//	Sleep(1000);
	//	process->InjectNewProcess();
	//	//Releaseinfo("xun huan yi ci");
	//}
	return 0;
}

