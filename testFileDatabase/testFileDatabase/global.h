#include "stdafx.h"
#include <string>
#include "afxwin.h"
#include <Dbghelp.h>

#pragma comment(lib,"Dbghelp.lib")

using namespace std;

bool isBlack;

void Releaseinfo(char *infostr)
{
	char Time[9];			 // 时间
	char Date[11];			 //日期
	::GetTimeFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP|TIME_FORCE24HOURFORMAT,NULL,"HH':'mm':'ss",Time,9);
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyy'-'MM'-'dd",Date,11);
	FILE *fp;
	fp = fopen("C:\\RegAndFile\\Process.log","a+");
	if(fp == NULL)
	{
		return ;
	}
	fprintf(fp,"%s,%s,%s\r\n",infostr,Date,Time);
	fclose(fp);
}

wstring ANSIToUnicode( const string& str )
{
	int  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar( CP_ACP,
			   0,
			   str.c_str(),
			   -1,
			   NULL,
			   0 );  
	wchar_t *  pUnicode;  
	pUnicode = new  wchar_t[unicodeLen+1];  
	memset(pUnicode,0,(unicodeLen+1)*sizeof(wchar_t));  
	::MultiByteToWideChar( CP_ACP,
			0,
			str.c_str(),
			-1,
			(LPWSTR)pUnicode,
			unicodeLen );  
	wstring  rt;  
	rt = ( wchar_t* )pUnicode;
	delete  pUnicode; 

	return  rt;  
}

string UnicodeToANSI( const wstring& str )
{
	char*     pElementText;
	int    iTextLen;
	iTextLen = WideCharToMultiByte( CP_ACP,
			 0,
			 str.c_str(),
			 -1,
			 NULL,
			0,
			 NULL,
			 NULL );
	pElementText = new char[iTextLen + 1];
	memset( ( void* )pElementText, 0, sizeof( char ) * ( iTextLen + 1 ) );
	::WideCharToMultiByte( CP_ACP,
			 0,
			 str.c_str(),
			 -1,
			 pElementText,
			 iTextLen,
			 NULL,
			 NULL );
	string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}

bool SetHook(HMODULE hMod, char * moduleName, PROC originFun, PROC newFun)
{
	ULONG ulSize;
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(
        hMod,
        TRUE,
        IMAGE_DIRECTORY_ENTRY_IMPORT,
        &ulSize );
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	while(pImportDesc->FirstThunk)
	{
		char *pszDllName = (char *) ( (BYTE *)hMod + pImportDesc->Name);
		if(_stricmp(pszDllName, moduleName) == 0 )
		{
			break;
		}
		pImportDesc++;
	}
	if(pImportDesc->FirstThunk)
	{
		IMAGE_THUNK_DATA *pThunk = (IMAGE_THUNK_DATA *)( (BYTE *)hMod + pImportDesc->FirstThunk);
		while(pThunk->u1.Function)
		{
			PROC *lpAddr = (PROC *)&pThunk->u1.Function;
			if(*lpAddr == originFun)
			{
				DWORD dwOldProtect;
				MEMORY_BASIC_INFORMATION mbi;
				VirtualQuery(lpAddr, &mbi, sizeof(mbi));
				::VirtualProtect(lpAddr, sizeof(newFun), PAGE_WRITECOPY, &dwOldProtect);
				::WriteProcessMemory(GetCurrentProcess(), lpAddr, &newFun, sizeof(newFun), NULL);
				//*lpAddr = (DWORD)lpNewProc;
				::VirtualProtect(lpAddr, sizeof(newFun), dwOldProtect, &dwOldProtect);
				return TRUE;
			}
			pThunk++;
		}
	}
	//Releaseinfo("hook error!");
	//MessageBox(NULL, L"错误", L"Hook", 0);
	return false;
}

//获取当前进程名