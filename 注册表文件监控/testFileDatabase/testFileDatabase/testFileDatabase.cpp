#include "stdafx.h"
//#include "windows.h"
//#include "Dbghelp.h"
//
//#pragma comment(lib, "Dbghelp.lib")
#include "hookFile.h"
#include "ListenAPI.h"

bool IsBlackProcess()
{
	Releaseinfo("IsBlackProcess");
	FILE *fp = fopen("c:\\RegAndFile\\black.txt", "r");
	if (NULL == fp){
		Releaseinfo("open black.txt failed!");
		return false;
	}
	char processName[MAX_PATH];
	strcpy(processName,::GetCurProcessName());
	char processNameb[MAX_PATH];
	while (fgets(processNameb, MAX_PATH, fp)){
		processNameb[strlen(processNameb)-2] = '\0';
		Releaseinfo(processNameb);
		Releaseinfo(processName);
		if (!strcmp(processName, processNameb)){
			Releaseinfo(processName);
			return true;
		}
	}
	return false;
}
//注册表监控线程
void RegListenerThreadProc()
{
	::ReplaceAPIMain();
	return ;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, void* lpReserved)
{	
	HANDLE g_hModule;
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		//MessageBox(NULL, "DLL_PROCESS_ATTACH", "info", 0);
		isBlack = IsBlackProcess();
		RegListenerThreadProc();
		hookFileThreadProc();
		g_hModule = (HINSTANCE)hModule;
		break;
	case DLL_PROCESS_DETACH:
		UnHookFileThreadProc();
		RecoverIAT();
		g_hModule = NULL;
		break;
	}
	return true;
}
//HANDLE WINAPI MyCreateFileW(
//	LPCTSTR lpFileName,
//	DWORD dwDesiredAccess,
//	DWORD dwShareMode,
//	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
//	DWORD dwCreationDisposition,
//	DWORD dwFlagsAndAttributes,
//	HANDLE hTemplateFile
//)
//{
//	MessageBox(NULL, "MyCreateFileW", "hook", 0);
//	//char Time[9];			 // 时间
//	//char Date[11];			 //日期
//	//::GetTimeFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP|TIME_FORCE24HOURFORMAT,NULL,"HH':'mm':'ss",Time,9);
//	//::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyy'-'MM'-'dd",Date,11);
//
//	////BuildSqlSentencef(GetCurProcessName(), lpFileName, "创建", Date, Time);
//	//
//	//PFNCreateFile pfnCreateFile = (PFNCreateFile)GetProcAddress(LoadLibrary("Kernel32.dll"), "CreateFileW");
//	//HANDLE handlef = pfnCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes,hTemplateFile);
//
//	return NULL;
//}

//bool SetHook(::GetModuleHandle(NULL),HMODULE hMod, char * moduleName, PROC originFun, PROC newFun)
//{
//	ULONG ulSize;
//    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
//        hMod,
//        TRUE,
//        IMAGE_DIRECTORY_ENTRY_IMPORT,
//        &ulSize );
//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	while(pImportDesc->FirstThunk)
//	{
//		char *pszDllName = (char *) ( (BYTE *)hMod + pImportDesc->Name);
//		if(_stricmp(pszDllName, moduleName) == 0 )
//		{
//			break;
//		}
//		pImportDesc++;
//	}
//	if(pImportDesc->FirstThunk)
//	{
//		IMAGE_THUNK_DATA *pThunk = (IMAGE_THUNK_DATA *)( (BYTE *)hMod + pImportDesc->FirstThunk);
//		while(pThunk->u1.Function)
//		{
//			PROC *lpAddr = (PROC *)&pThunk->u1.Function;
//			if(*lpAddr == originFun)
//			{
//				DWORD dwOldProtect;
//				MEMORY_BASIC_INFORMATION mbi;
//				VirtualQuery(lpAddr, &mbi, sizeof(mbi));
//				::VirtualProtect(lpAddr, sizeof(newFun), PAGE_WRITECOPY, &dwOldProtect);
//				::WriteProcessMemory(GetCurrentProcess(), lpAddr, &newFun, sizeof(newFun), NULL);
//				//*lpAddr = (DWORD)lpNewProc;
//				::VirtualProtect(lpAddr, sizeof(newFun), dwOldProtect, &dwOldProtect);
//				return TRUE;
//			}
//			pThunk++;
//		}
//	}
//	//MessageBox(NULL, L"错误", L"Hook", 0);
//	return false;
//}

//int main()
//{
//	while (1){
//		getchar();
//
//		DWORD szBuffer[4];
//		char szText[] = "123";
//		char lpBuffer[10];
//		DWORD NumberOfBytesRead;
//		hookFileThreadProc();
//		HANDLE handlef = CreateFileW(L"name.txt", FILE_MAP_ALL_ACCESS, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);//FILE_ATTRIBUTE_NORMAL
//		DWORD Length=strlen(szText);//计算自己要写入的长度
//		WriteFile(handlef,szText,Length,szBuffer,NULL);//写入刚才创建的空文件中
//		ReadFile(handlef, lpBuffer, 1, &NumberOfBytesRead, NULL);
//		/*TCHAR szFilePath[MAX_PATH + 1];
//		if (GetFileNameFromHandle(handlef, szFilePath))
//			printf("成功\n");*/
//		CloseHandle(handlef);
//		DeleteFile("name.txt");
//	}
//}

//#include "windows.h"
//#include "Dbghelp.h"
//
//#pragma comment(lib, "Dbghelp.lib")
//
//bool SetHook(::GetModuleHandle(NULL),HMODULE hMod, char * moduleName, PROC newFun);
//typedef int (WINAPI *PFNMESSAGEBOX)(HWND, LPCSTR, LPCSTR, UINT uType);
//PROC g_orgProc = (PROC)GetProcAddress(LoadLibrary("Kernel32.dll"), "CreateFileA");
////
//BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, void* lpReserved)
//{	
//	HANDLE g_hModule;
//	switch(dwReason)
//	{
//	case DLL_PROCESS_ATTACH:
//		SetHook(::GetModuleHandle(NULL),::GetModuleHandle(NULL), "Kernel32.dll", (PROC)MyCreateFileW);
//		g_hModule = (HINSTANCE)hModule;
//		break;
//	case DLL_PROCESS_DETACH:
//		g_hModule = NULL;
//		break;
//	}
//	return true;
//}
////int WINAPI MyMessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
////{
////	return ((PFNMESSAGEBOX)g_orgProc)(hWnd, "新函数", "09HookDemo", 0);
////}
//HANDLE WINAPI MyCreateFile(
//	LPCTSTR lpFileName,
//	DWORD dwDesiredAccess,
//	DWORD dwShareMode,
//	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
//	DWORD dwCreationDisposition,
//	DWORD dwFlagsAndAttributes,
//	HANDLE hTemplateFile
//)
//{
//	MessageBox(NULL, "创建新文件", "Hook", 0);
//	return NULL;
//}
//
//bool SetHook(::GetModuleHandle(NULL),HMODULE hMod, char * moduleName, PROC newFun)
//{
//	////////////////////////////////////////////////////////////////////
//	//获得PIMAGE_IMPORT_DESCRIPTOR指针的两种方法
//	//IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)hMod;
//	//IMAGE_OPTIONAL_HEADER *pOptHeader = (IMAGE_OPTIONAL_HEADER *)( (BYTE *)hMod + pDosHeader->e_lfanew + 24 );
//	//IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR *)( (BYTE *)hMod + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
//	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	///////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ULONG ulSize;
//    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
//        hMod,
//        TRUE,
//        IMAGE_DIRECTORY_ENTRY_IMPORT,
//        &ulSize );
//	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	while(pImportDesc->FirstThunk)
//	{
//		char *pszDllName = (char *) ( (BYTE *)hMod + pImportDesc->Name);
//		if(_stricmp(pszDllName, moduleName) == 0 )
//		{
//			break;
//		}
//		pImportDesc++;
//	}
//	if(pImportDesc->FirstThunk)
//	{
//		IMAGE_THUNK_DATA *pThunk = (IMAGE_THUNK_DATA *)( (BYTE *)hMod + pImportDesc->FirstThunk);
//		while(pThunk->u1.Function)
//		{
//			PROC *lpAddr = (PROC *)&pThunk->u1.Function;
//			if(*lpAddr == g_orgProc)
//			{
//				DWORD dwOldProtect;
//				MEMORY_BASIC_INFORMATION mbi;
//				VirtualQuery(lpAddr, &mbi, sizeof(mbi));
//				::VirtualProtect(lpAddr, sizeof(newFun), PAGE_WRITECOPY, &dwOldProtect);
//				::WriteProcessMemory(GetCurrentProcess(), lpAddr, &newFun, sizeof(newFun), NULL);
//				//*lpAddr = (DWORD)lpNewProc;
//				::VirtualProtect(lpAddr, sizeof(newFun), dwOldProtect, &dwOldProtect);
//				return TRUE;
//			}
//			pThunk++;
//		}
//	}
//	//MessageBox(NULL, L"错误", L"Hook", 0);
//	return false;
//}