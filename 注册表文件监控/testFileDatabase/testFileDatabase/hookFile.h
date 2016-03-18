/*****************************************************
** copyright (C) 2015-2016
** File Name   : HookFile.h
** author      : mjw
** version     : 1.0
** discription : hook the change of file
*****************************************************/

#include "stdafx.h"
#include "stdio.h"
#ifndef _GLOBAL
#include "global.h"
#define _GLOBAL
#endif
#include "DB.h"
//#include <TlHelp32.h>
//#include <dbghelp.h>
//#include "winnt.h"
#include <psapi.h>
#include <strsafe.h>

#define BUFSIZE 512 

#pragma comment(lib,"Psapi.lib")


//根据打开的句柄获得文件名
BOOL GetFileNameFromHandle(HANDLE hFile, TCHAR *pszFilename) 
{
  BOOL bSuccess = FALSE;
  //TCHAR pszFilename[MAX_PATH+1];
  HANDLE hFileMap;

  // Get the file size.
  DWORD dwFileSizeHi = 0;
  DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi); 

  if( dwFileSizeLo == 0 && dwFileSizeHi == 0 )
  {
    // MessageBox(NULL, L"Cannot map a file with a length of zero.\n", L"hook demo", 0);
     return FALSE;
  }

  // Create a file mapping object.
  hFileMap = CreateFileMapping(hFile, 
                    NULL, 
                    PAGE_READONLY,
                    0, 
                    1,
                    NULL);

  if (hFileMap) 
  {
    // Create a file mapping to get the file name.
    void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

    if (pMem) 
    {
      if (GetMappedFileName (GetCurrentProcess(), 
                             pMem, 
                             pszFilename,
                             MAX_PATH)) 
      {
        TCHAR szTemp[BUFSIZE];
        szTemp[0] = '\0';

        if (GetLogicalDriveStrings(BUFSIZE-1, szTemp)) 
        {
          TCHAR szName[MAX_PATH];
          TCHAR szDrive[3] = TEXT(" :");
          BOOL bFound = FALSE;
          TCHAR* p = szTemp;

          do 
          {
            // Copy the drive letter to the template string
			  //MessageBox(NULL, L"jinruxunhuan", L"getfilename", 0);
            *szDrive = *p;

            // Look up each device name
            if (QueryDosDevice(szDrive, szName, MAX_PATH))
            {
              size_t uNameLen = strlen(szName);

              if (uNameLen < MAX_PATH) 
              {
                bFound = strnicmp(pszFilename, szName, uNameLen) == 0
                         && *(pszFilename + uNameLen) == L'\\';

                if (bFound) 
                {
                  // Reconstruct pszFilename using szTempFile
                  // Replace device path with DOS path
                  TCHAR szTempFile[MAX_PATH];
                  StringCchPrintf(szTempFile,
                            MAX_PATH,
                            TEXT("%s%s"),
                            szDrive,
                            pszFilename+uNameLen);
                  StringCchCopyN(pszFilename, MAX_PATH+1, szTempFile, strlen(szTempFile));
                }
              }
            }

            // Go to the next NULL character.
            while (*p++);
          } while (!bFound && *p); // end of string
		  bSuccess = TRUE;
        }
      }
      UnmapViewOfFile(pMem);
    } 
    CloseHandle(hFileMap);
  }
  return(bSuccess);
}

//改变IAT里面的地址
bool SetHook(HMODULE hMod, char * moduleName, PROC originFun, PROC newFun);
typedef HANDLE (WINAPI* PFNCreateFile)(
	LPCTSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
);
typedef HANDLE (WINAPI* PFNCreateFileW)(
	LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
);

/******************************************
** 所有需要hook的api函数指针，用来再恢复函数
** 功能
******************************************/
typedef int (WINAPI* PFNOpenFile)(
	LPCSTR lpFileName,
	LPOFSTRUCT lpReOpenBuff,
	UINT uStyle
);

typedef BOOL (WINAPI* PFNDeleteFile)(
	LPCTSTR lpFileName
);
typedef BOOL (WINAPI* PFNDeleteFileW)(
	LPCWSTR lpFileName
);

typedef BOOL (WINAPI* PFNWriteFile)(
	HANDLE hFile,
	LPCVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped
);

typedef BOOL (WINAPI* PFNWriteFileEx)(
	HANDLE hFile,
	LPCVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPOVERLAPPED lpOverlapped,
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);

typedef BOOL (WINAPI* PFNReadFile)(
	HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
);

typedef BOOL (WINAPI* PFNReadFileEx)(
	HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPOVERLAPPED lpOverlapped,
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);

/******************************************
** hook后转向的函数定义
******************************************/
HANDLE WINAPI MyCreateFileA(
	LPCTSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
)
{
	char Time[9];			 // 时间
	char Date[11];			 //日期
	::GetTimeFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP|TIME_FORCE24HOURFORMAT,NULL,"HH':'mm':'ss",Time,9);
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyy'-'MM'-'dd",Date,11);

	BuildSqlSentencef(GetCurProcessName(), lpFileName, "创建", Date, Time);
	
	PFNCreateFile pfnCreateFile = (PFNCreateFile)GetProcAddress(LoadLibrary("Kernel32.dll"), "CreateFileA");
	HANDLE handlef = pfnCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes,hTemplateFile);

	return 0;
}

HANDLE WINAPI MyCreateFileW(
	LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile
)
{
	char Time[9];			 // 时间
	char Date[11];			 //日期
	::GetTimeFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP|TIME_FORCE24HOURFORMAT,NULL,"HH':'mm':'ss",Time,9);
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyy'-'MM'-'dd",Date,11);
	string f = UnicodeToANSI(lpFileName);
	BuildSqlSentencef(GetCurProcessName(), f.c_str(), "创建", Date, Time);
	
	PFNCreateFileW pfnCreateFileW = (PFNCreateFileW)GetProcAddress(LoadLibrary("Kernel32.dll"), "CreateFileW");
	HANDLE handlef = pfnCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes,hTemplateFile);

	return NULL;
}

HFILE WINAPI MyOpenFile(
	LPCSTR lpFileName,
	LPOFSTRUCT lpReOpenBuff,
	UINT uStyle
)
{
	char Time[9];			 // 时间
	char Date[11];			 //日期
	::GetTimeFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP|TIME_FORCE24HOURFORMAT,NULL,"HH':'mm':'ss",Time,9);
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyy'-'MM'-'dd",Date,11);
	BuildSqlSentencef(GetCurProcessName(), lpFileName, "打开", Date, Time);

	PFNOpenFile pfnOpenFile = (PFNOpenFile)GetProcAddress(LoadLibrary("Kernel32.dll"), "OpenFile");
	pfnOpenFile(lpFileName, lpReOpenBuff, uStyle);

	return 0;
}

BOOL WINAPI MyDeleteFileA(
	LPCTSTR lpFileName
)
{
	char Time[9];			 // 时间
	char Date[11];			 //日期
	::GetTimeFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP|TIME_FORCE24HOURFORMAT,NULL,"HH':'mm':'ss",Time,9);
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyy'-'MM'-'dd",Date,11);
	BuildSqlSentencef(GetCurProcessName(), lpFileName, "删除", Date, Time);

	PFNDeleteFile pfnDeleteFile = (PFNDeleteFile)GetProcAddress(LoadLibrary("Kernel32.dll"), "DeleteFileA");
	pfnDeleteFile(lpFileName);

	return true;
}

BOOL WINAPI MyDeleteFileW(
	LPCWSTR lpFileName
)
{
	char Time[9];			 // 时间
	char Date[11];			 //日期
	::GetTimeFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP|TIME_FORCE24HOURFORMAT,NULL,"HH':'mm':'ss",Time,9);
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyy'-'MM'-'dd",Date,11);
	string f = UnicodeToANSI(lpFileName);
	BuildSqlSentencef(GetCurProcessName(), f.c_str(), "删除", Date, Time);

	PFNDeleteFileW pfnDeleteFileW = (PFNDeleteFileW)GetProcAddress(LoadLibrary("Kernel32.dll"), "DeleteFileW");
	pfnDeleteFileW(lpFileName);

	return true;
}

BOOL WINAPI MyWriteFile(
	HANDLE hFile,
	LPCVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped
)
{
	char szFilePath[MAX_PATH + 1];
	char Time[9];			 // 时间
	char Date[11];			 //日期
	PFNWriteFile pfnWriteFile = (PFNWriteFile)GetProcAddress(LoadLibrary("Kernel32.dll"), "WriteFile");
	BOOL result = pfnWriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
	if (!GetFileNameFromHandle(hFile, szFilePath))
		strcpy(szFilePath, "无法获取文件名");
	 
	::GetTimeFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP|TIME_FORCE24HOURFORMAT,NULL,"HH':'mm':'ss",Time,9);
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyy'-'MM'-'dd",Date,11);
	BuildSqlSentencef(GetCurProcessName(), szFilePath, "写入", Date, Time);
	return result;
}

BOOL WINAPI MyWriteFileEx(
	HANDLE hFile,
	LPCVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPOVERLAPPED lpOverlapped,
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{
	PFNWriteFileEx pfnWriteFileEx = (PFNWriteFileEx)GetProcAddress(LoadLibrary("Kernel32.dll"), "WriteFileEx");
	BOOL result = pfnWriteFileEx(hFile, lpBuffer, nNumberOfBytesToWrite, lpOverlapped, lpCompletionRoutine);
	TCHAR szFilePath[MAX_PATH];
	if (!GetFileNameFromHandle(hFile, szFilePath))
		strcpy(szFilePath, "无法获取文件名");

	char Time[9];			 // 时间
	char Date[11];			 //日期
	::GetTimeFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP|TIME_FORCE24HOURFORMAT,NULL,"HH':'mm':'ss",Time,9);
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyy'-'MM'-'dd",Date,11);
	BuildSqlSentencef(GetCurProcessName(), "", "写入", Date, Time);
	return result;
}

BOOL WINAPI MyReadFile(
	HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped
)
{
	TCHAR szFilePath[MAX_PATH];
	if (!GetFileNameFromHandle(hFile, szFilePath))
		strcpy(szFilePath, "无法获取文件名");

	char Time[9];			 // 时间
	char Date[11];			 //日期
	::GetTimeFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP|TIME_FORCE24HOURFORMAT,NULL,"HH':'mm':'ss",Time,9);
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyy'-'MM'-'dd",Date,11);

	BuildSqlSentencef(GetCurProcessName(), szFilePath, "读入", Date, Time);

	PFNReadFile pfnReadFile = (PFNReadFile)GetProcAddress(LoadLibrary("Kernel32.dll"), "ReadFile");
	BOOL result = pfnReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);

	return result;
}

BOOL WINAPI MyReadFileEx(
	HANDLE hFile,
	LPVOID lpBuffer,
	DWORD nNumberOfBytesToRead,
	LPOVERLAPPED lpOverlapped,
	LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
)
{
	TCHAR szFilePath[MAX_PATH];
	if (!GetFileNameFromHandle(hFile, szFilePath))
		strcpy(szFilePath, "无法获取文件名");

	char Time[9];			 // 时间
	char Date[11];			 //日期
	::GetTimeFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP|TIME_FORCE24HOURFORMAT,NULL,"HH':'mm':'ss",Time,9);
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyy'-'MM'-'dd",Date,11);

	BuildSqlSentencef(GetCurProcessName(), szFilePath, "读入", Date, Time);

	PFNReadFileEx pfnReadFileEx = (PFNReadFileEx)GetProcAddress(LoadLibrary("Kernel32.dll"), "ReadFileEx");
	BOOL result = pfnReadFileEx(hFile, lpBuffer, nNumberOfBytesToRead, lpOverlapped, lpCompletionRoutine);

	return result;
}

PROC originCreateFileW;
PROC originCreateFileA;
PROC originOpenFile;
PROC originDeleteFileW;
PROC originDeleteFileA;
PROC originWriteFile;
PROC originWriteFileEx;
PROC originReadFile;
PROC originReadFileEx;

void hookFileThreadProc()
{	//Releaseinfo("hookFileThreadProc");
	originCreateFileW = (PROC)GetProcAddress(LoadLibrary("Kernel32.dll"), "CreateFileW");
	originCreateFileA  = (PROC)GetProcAddress(LoadLibrary("Kernel32.dll"), "CreateFileA");
	originOpenFile = (PROC)GetProcAddress(LoadLibrary("Kernel32.dll"), "OpenFile");
	originDeleteFileW = (PROC)GetProcAddress(LoadLibrary("Kernel32.dll"), "DeleteFileW");
	originDeleteFileA = (PROC)GetProcAddress(LoadLibrary("Kernel32.dll"), "DeleteFileA");
	originWriteFile = (PROC)GetProcAddress(LoadLibrary("Kernel32.dll"), "WriteFile");
	originWriteFileEx = (PROC)GetProcAddress(LoadLibrary("Kernel32.dll"), "WriteFileEx");
	originReadFile = (PROC)GetProcAddress(LoadLibrary("Kernel32.dll"), "ReadFile");
	originReadFileEx = (PROC)GetProcAddress(LoadLibrary("Kernel32.dll"), "ReadFileEx");
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", originCreateFileW, (PROC)MyCreateFileW);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", originCreateFileA, (PROC)MyCreateFileA);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", originOpenFile, (PROC)MyOpenFile);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", originDeleteFileW, (PROC)MyDeleteFileW);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", originDeleteFileA, (PROC)MyDeleteFileA);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", originWriteFile, (PROC)MyWriteFile);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", originWriteFileEx, (PROC)MyWriteFileEx);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", originReadFile, (PROC)MyReadFile);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", originReadFileEx, (PROC)MyReadFileEx);
	return ;
}

void UnHookFileThreadProc()
{
	//Releaseinfo("UnHookFileThreadProc");
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", (PROC)MyCreateFileW, originCreateFileW);;
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", (PROC)MyCreateFileA, originCreateFileA);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", (PROC)MyOpenFile, originOpenFile);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", (PROC)MyDeleteFileW, originDeleteFileW);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", (PROC)MyDeleteFileA, originDeleteFileA);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", (PROC)MyWriteFile, originWriteFile);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", (PROC)MyWriteFileEx, originWriteFileEx);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", (PROC)MyReadFile, originReadFile);
	SetHook(::GetModuleHandle(NULL), "Kernel32.dll", (PROC)MyReadFileEx, originReadFileEx);
}
/***********************************************************************************************************************************************************
** 获得PIMAGE_IMPORT_DESCRIPTOR指针的两种方法
** IMAGE_DOS_HEADER *pDosHeader = (IMAGE_DOS_HEADER *)hMod;
** IMAGE_OPTIONAL_HEADER *pOptHeader = (IMAGE_OPTIONAL_HEADER *)( (BYTE *)hMod + pDosHeader->e_lfanew + 24 );
** IMAGE_IMPORT_DESCRIPTOR *pImportDesc = (IMAGE_IMPORT_DESCRIPTOR *)( (BYTE *)hMod + pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
*************************************************************************************************************************************************************/
//bool SetHook(HMODULE hMod, char * moduleName, PROC originFun, PROC newFun)
//{
//	ULONG ulSize;
//    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
//        hMod,
//        TRUE,
//        IMAGE_DIRECTORY_ENTRY_IMPORT,
//        &ulSize );
//
//	while(pImportDesc->FirstThunk)
//	{
//		char *pszDllName = (char *) ( (BYTE *)hMod + pImportDesc->Name);
//		if(_stricmp(pszDllName, moduleName) == 0 )
//		{
//			break;
//		}
//		pImportDesc++;
//	}
//
//	if(pImportDesc->FirstThunk)
//	{
//		IMAGE_THUNK_DATA *pThunk = (IMAGE_THUNK_DATA *)( (BYTE *)hMod + pImportDesc->FirstThunk);
//		while(pThunk->u1.Function)
//		{
//			DWORD *lpAddr = (DWORD *)&pThunk->u1.Function;
//			if(*lpAddr == (DWORD)originFun)
//			{
//				DWORD *lpNewProc = (DWORD *)newFun;
//				DWORD dwOldProtect;
//				MEMORY_BASIC_INFORMATION mbi;
//				VirtualQuery(lpAddr, &mbi, sizeof(mbi));
//				::VirtualProtect(lpAddr, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
//				::WriteProcessMemory(GetCurrentProcess(), lpAddr, &lpNewProc, sizeof(lpNewProc), NULL);
//				::VirtualProtect(lpAddr, sizeof(DWORD), dwOldProtect, 0);
//				return TRUE;
//			}
//			pThunk++;
//		}
//	}
//	return false;
//}

//bool SetHook(HMODULE hMod, char * moduleName, PROC originFun, PROC newFun)
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