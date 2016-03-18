// ListenAPI.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#ifndef _GLOBAL
#include "global.h"
#define _GLOBAL
#endif
#ifndef _STDIO
#include "stdio.h"
#define _STDIO
#endif
#ifndef _DB
#include "DB.h"
#define _DB
#endif

typedef LONG NTSTATUS;
 
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
#endif
  
#ifndef STATUS_BUFFER_TOO_SMALL
#define STATUS_BUFFER_TOO_SMALL ((NTSTATUS)0xC0000023L)
#endif

//---------------------------------------全局变量声明---------------------------//
//API及其加载内存地址
struct API{
	char ApiName[256];				//API NAME
	PROC ApiAddress;				// API Address
	PROC ReplaceApiAddress;		// Replace API Address
	bool Replaced;	
	bool used;
	//API *Next;
};

//根据打开的注册表句柄获得操作的整个句柄
std::wstring GetKeyPathFromKKEY(HKEY key)
{
	std::wstring keyPath;
	if (key != NULL){
		HMODULE dll = LoadLibrary("ntdll.dll");
		if (dll != NULL) {
			typedef DWORD (__stdcall *ZwQueryKeyType)(
			HANDLE  KeyHandle,
			int KeyInformationClass,
			PVOID  KeyInformation,
			ULONG  Length,
			PULONG  ResultLength);
			ZwQueryKeyType func = reinterpret_cast<ZwQueryKeyType>(::GetProcAddress(dll, "ZwQueryKey"));
			if (func != NULL) {
				DWORD size = 0;
				DWORD result = 0;
				result = func(key, 3, 0, 0, &size);
				if (result == STATUS_BUFFER_TOO_SMALL){
					size = size + 2;
					wchar_t* buffer = new (std::nothrow) wchar_t[size];
					if (buffer != NULL){
						result = func(key, 3, buffer, size, &size);
						if (result == STATUS_SUCCESS){
							buffer[size / sizeof(wchar_t)] = L'\0';
							keyPath = std::wstring(buffer + 2);
						}
						delete[] buffer;
					}
				}
			}
		FreeLibrary(dll);
		}
	}
	return keyPath;
}

//-------全局变量声明----
API ApiNode[22];	//API 信息数组 存放所有要修改的API函数名 原地址 替换地址 以及PE文件IAT表的	
//----------------------函数部分------------------------------------//

typedef LONG (WINAPI* PFNRegCreateKeyExA)(
  HKEY hKey,                                  // handle to open key
  LPCTSTR lpSubKey,                           // subkey name
  DWORD Reserved,                             // reserved
  LPTSTR lpClass,                             // class string
  DWORD dwOptions,                            // special options
  REGSAM samDesired,                          // desired security access
  LPSECURITY_ATTRIBUTES lpSecurityAttributes, // inheritance
  PHKEY phkResult,                            // key handle 
  LPDWORD lpdwDisposition                     // disposition value buffer
);
typedef LONG (WINAPI *PFNRegCreateKeyExW)(
  HKEY hKey,                                  // handle to open key
  LPCWSTR lpSubKey,                           // subkey name
  DWORD Reserved,                             // reserved
  LPWSTR lpClass,                             // class string
  DWORD dwOptions,                            // special options
  REGSAM samDesired,                          // desired security access
  LPSECURITY_ATTRIBUTES lpSecurityAttributes, // inheritance
  PHKEY phkResult,                            // key handle 
  LPDWORD lpdwDisposition                     // disposition value buffer
);
typedef LONG (WINAPI* PFNRegCreateKeyA)(
  HKEY hKey,        // handle to an open key
  LPCTSTR lpSubKey, // subkey name
  PHKEY phkResult   // buffer for key handle
  );
typedef LONG (WINAPI* PFNRegCreateKeyW)(
  HKEY hKey,        // handle to an open key
  LPCWSTR lpSubKey, // subkey name
  PHKEY phkResult   // buffer for key handle
  );

//RegDeleteKey替换函数
typedef LSTATUS (WINAPI* PFNRegDeleteKeyA)(
  HKEY hKey,         // handle to open key
  LPCTSTR lpSubKey   // subkey name
  );

typedef LONG (WINAPI* PFNRegDeleteKeyW)(
  HKEY hKey,         // handle to open key
  LPCWSTR lpSubKey   // subkey name
  );

//RegSetValue 替换函数
typedef LONG (WINAPI* PFNRegSetValueA)(HKEY hKey,LPCTSTR lpSubKey,DWORD dwType,LPCSTR lpData,DWORD cbData);

typedef LONG (WINAPI* PFNRegSetValueW)(HKEY hKey,LPCWSTR lpSubKey,DWORD dwType,LPCWSTR lpData,DWORD cbData);

//RegSetValueEx 替换函数
typedef LONG (WINAPI* PFNRegSetValueExA)(
  HKEY hKey,           // handle to key
  LPCTSTR lpValueName, // value name
  DWORD Reserved,      // reserved
  DWORD dwType,        // value type
  CONST BYTE *lpData,  // value data
  DWORD cbData         // size of value data
);

typedef LONG (WINAPI* PFNRegSetValueExW)(
  HKEY hKey,           // handle to key
  LPCWSTR lpValueName, // value name
  DWORD Reserved,      // reserved
  DWORD dwType,        // value type
  CONST BYTE *lpData,  // value data
  DWORD cbData         // size of value data
);

//RegSetKeySecurity 替换函数
typedef LONG (WINAPI* PFNRegSetKeySecurity)(
  HKEY hKey,                                // handle to key
  SECURITY_INFORMATION SecurityInformation, // request
  PSECURITY_DESCRIPTOR pSecurityDescriptor  // SD
  );

//RegLoadKey 替换函数
typedef LONG (WINAPI* PFNRegLoadKeyA)(
  HKEY hKey,        // handle to open key
  LPCTSTR lpSubKey, // subkey name
  LPCTSTR lpFile    // registry file name
);

typedef LONG (WINAPI* PFNRegLoadKeyW)(
  HKEY hKey,        // handle to open key
  LPCWSTR lpSubKey, // subkey name
  LPCWSTR lpFile    // registry file name
);

//RegSaveKey 替换函数
typedef LONG (WINAPI* PFNRegSaveKeyA)(
  HKEY hKey,                                  // handle to key
  LPCTSTR lpFile,                             // data file
  LPSECURITY_ATTRIBUTES lpSecurityAttributes  // SD
);

typedef LONG (WINAPI* PFNRegSaveKeyW)(
  HKEY hKey,                                  // handle to key
  LPCWSTR lpFile,                             // data file
  LPSECURITY_ATTRIBUTES lpSecurityAttributes  // SD
);

//RegSaveKeyEx 替换函数
typedef LONG (WINAPI* PFNRegSaveKeyExA)(
  HKEY hKey,  
  LPCTSTR lpFile,  
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,  
  DWORD Flags  
  );

typedef LONG (WINAPI* PFNRegSaveKeyExW)(
  HKEY hKey,  
  LPCWSTR lpFile,  
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,  
  DWORD Flags  
  );

// RegNotifyChangeKeyValue 替换函数
typedef LONG (WINAPI* PFNRegNotifyChangeKeyValue)(HKEY hKey,BOOL bWatchSubtree,DWORD dwNotifyFilter,HANDLE hEvent,BOOL fAsynchronous );

//RegReplaceKey 替换函数
typedef LONG (WINAPI* PFNRegReplaceKeyA)(
  HKEY hKey,         // handle to open key
  LPCTSTR lpSubKey,  // subkey name
  LPCTSTR lpNewFile, // data file
  LPCTSTR lpOldFile  // backup file
  );

typedef LONG (WINAPI* PFNRegReplaceKeyW)(
  HKEY hKey,         // handle to open key
  LPCWSTR lpSubKey,  // subkey name
  LPCWSTR lpNewFile, // data file
  LPCWSTR lpOldFile  // backup file
  );

// RegUnLoadKey 替换函数
typedef LONG (WINAPI* PFNRegUnLoadKeyA)(
  HKEY hKey,         // handle to open key
  LPCTSTR lpSubKey   // subkey name
);

typedef LONG (WINAPI* PFNRegUnLoadKeyW)(
  HKEY hKey,         // handle to open key
  LPCWSTR lpSubKey   // subkey name
);

/**************************************************************
** 替换函数
***************************************************************/

//替换RegCreateKeyEx函数的函数
LONG WINAPI MyRegCreateKeyExA(
  HKEY hKey,                                  // handle to open key
  LPCTSTR lpSubKey,                           // subkey name
  DWORD Reserved,                             // reserved
  LPTSTR lpClass,                             // class string
  DWORD dwOptions,                            // special options
  REGSAM samDesired,                          // desired security access
  LPSECURITY_ATTRIBUTES lpSecurityAttributes, // inheritance
  PHKEY phkResult,                            // key handle 
  LPDWORD lpdwDisposition                     // disposition value buffer
){
	//Add you operator here
	// 记录操作
	// 根键名称获取
	char hKeyName[MAX_PATH];
	::GetRegRootName(hKey, hKeyName);
	strcat(hKeyName, lpSubKey);
	::BuildSqlSentence(hKeyName, "RegCreateKey");
	if (isBlack){
	//恢复原函数入口
		if (MessageBox(NULL, hKeyName,"RegCreateKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegCreateKeyExA pfnCreateKeyExA = (PFNRegCreateKeyExA)GetProcAddress(LoadLibrary("advapi32.dll"), "RegCreateKeyExA");
			LONG handlef = pfnCreateKeyExA (hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
	
			return handlef;
		}
	}
	return 0;
}

LONG WINAPI MyRegCreateKeyExW(
  HKEY hKey,                                  // handle to open key
  LPCWSTR lpSubKey,                           // subkey name
  DWORD Reserved,                             // reserved
  LPWSTR lpClass,                             // class string
  DWORD dwOptions,                            // special options
  REGSAM samDesired,                          // desired security access
  LPSECURITY_ATTRIBUTES lpSecurityAttributes, // inheritance
  PHKEY phkResult,                            // key handle 
  LPDWORD lpdwDisposition                     // disposition value buffer
){
	//Add you operator here
	// 记录操作
	// 根键名称获取
	char hKeyName[256];
	::GetRegRootName(hKey, hKeyName);
	string buff = UnicodeToANSI(lpSubKey);
	strcat(hKeyName, "\\");
	strcat(hKeyName, buff.c_str());
	::BuildSqlSentence(hKeyName,"RegCreateKey");
	buff.clear();
	//恢复原函数入口
	if (isBlack){
		if (MessageBox(NULL, hKeyName,"RegCreateKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegCreateKeyExW pfnRegCreateKeyExW = (PFNRegCreateKeyExW)GetProcAddress(LoadLibrary("advapi32.dll"), "RegCreateKeyExW");
			LONG handlef = pfnRegCreateKeyExW(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
	
			return handlef;
		}
	}
	return 0;
}

//替换RegCreateKey函数的函数
LONG WINAPI MyRegCreateKeyA(
  HKEY hKey,        // handle to an open key
  LPCTSTR lpSubKey, // subkey name
  PHKEY phkResult   // buffer for key handle
  ){
	
	// 根键名称获取
	char hKeyName[MAX_PATH];
	::GetRegRootName(hKey, hKeyName);
	strcat(hKeyName, "\\");
	strcat(hKeyName, lpSubKey);
	// 记录操作
	::BuildSqlSentence(hKeyName,"RegCreateKey");
	
	if (isBlack){
		if (MessageBox(NULL, hKeyName,"RegCreateKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegCreateKeyA pfnRegCreateKeyA = (PFNRegCreateKeyA)GetProcAddress(LoadLibrary("advapi32.dll"), "RegCreateKeyA");
			LONG handlef = pfnRegCreateKeyA (hKey, lpSubKey,phkResult);
	
			return handlef;
		}
	}
	return 0;
}
LONG WINAPI MyRegCreateKeyW(
  HKEY hKey,        // handle to an open key
  LPCWSTR lpSubKey, // subkey name
  PHKEY phkResult   // buffer for key handle
  ){
	// 根键名称获取
	string slpSubKey = UnicodeToANSI(lpSubKey);
	char hKeyName[MAX_PATH];
	::GetRegRootName(hKey, hKeyName);
	strcat(hKeyName, "\\");
	strcat(hKeyName, slpSubKey.c_str());
	// 记录操作
	::BuildSqlSentence(hKeyName,"RegCreateKey");
	slpSubKey.clear();

	if (isBlack){
		if (MessageBox(NULL, hKeyName,"RegCreateKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegCreateKeyW pfnRegCreateKeyW = (PFNRegCreateKeyW)GetProcAddress(LoadLibrary("advapi32.dll"), "RegCreateKeyW");
			LONG handlef = pfnRegCreateKeyW (hKey, lpSubKey,phkResult);
	
			return handlef;
		}
	}
	return 0;
}

//RegDeleteKey替换函数
LSTATUS WINAPI MyRegDeleteKeyA(
  HKEY hKey,         // handle to open key
  LPCTSTR lpSubKey   // subkey name
  ){
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	hKeyName.append("\\");
	hKeyName.append(lpSubKey);
	// 记录操作
	::BuildSqlSentence(hKeyName.c_str(),"RegDeleteKey");

	if (isBlack){
		if (MessageBox(NULL, hKeyName.c_str(),"RegDeleteKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegDeleteKeyA pfnRegDeleteKeyA = (PFNRegDeleteKeyA)GetProcAddress(LoadLibrary("advapi32.dll"), "RegDeleteKeyA");
			LONG handlef = pfnRegDeleteKeyA(hKey, lpSubKey);
	
			return handlef;
		}
	}
	return 0;
}

LONG WINAPI MyRegDeleteKeyW(
  HKEY hKey,         // handle to open key
  LPCWSTR lpSubKey   // subkey name
  ){
	
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	hKeyName.append("\\");
	string lpSubKeys = UnicodeToANSI(lpSubKey);
	hKeyName.append(lpSubKeys);
	 // 记录操作
	::BuildSqlSentence(hKeyName.c_str(),"RegDeleteKey");

	if (isBlack){
		if (MessageBox(NULL, hKeyName.c_str(),"RegDeleteKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegDeleteKeyW pfnDeleteKeyW = (PFNRegDeleteKeyW)GetProcAddress(LoadLibrary("advapi32.dll"), "RegDeleteKeyW");
			LONG handlef = pfnDeleteKeyW(hKey, lpSubKey);
	
			return handlef;
		}
	}
	return 0;
}

//RegSetValue 替换函数
LONG WINAPI MyRegSetValueA(HKEY hKey,LPCTSTR lpSubKey,DWORD dwType,LPCSTR lpData,DWORD cbData)
{
	//printf("替换RegSetValue 成功!\n");
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	hKeyName.append("\\");
	hKeyName.append(lpSubKey);
	// 记录操作
	::BuildSqlSentence(hKeyName.c_str(),"RegSetValue");
	
	if (isBlack){
		if (MessageBox(NULL, hKeyName.c_str(),"RegSetValue", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegSetValueA pfnRegSetValueA = (PFNRegSetValueA)GetProcAddress(LoadLibrary("advapi32.dll"), "RegSetValueA");
			LONG handlef = pfnRegSetValueA( hKey, lpSubKey, dwType, lpData, cbData);
			return handlef;
		}
	}
	return 0;
}

LONG WINAPI MyRegSetValueW(HKEY hKey,LPCWSTR lpSubKey,DWORD dwType,LPCWSTR lpData,DWORD cbData)
{
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	char hKeyFullName[MAX_PATH];
	strcpy(hKeyFullName, hKeyName.c_str());
	string slpSubKey  = UnicodeToANSI(lpSubKey);
	strcat(hKeyFullName, "\\");
	strcat(hKeyFullName, slpSubKey.c_str());
	// 记录操作
	::BuildSqlSentence(hKeyFullName,"RegSetValue");
	slpSubKey.clear();
	
	if (isBlack){
		if (MessageBox(NULL, hKeyFullName,"RegSetValue", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegSetValueW pfnRegSetValueW = (PFNRegSetValueW)GetProcAddress(LoadLibrary("advapi32.dll"), "RegSetValueW");
			LONG handlef = pfnRegSetValueW( hKey, lpSubKey, dwType, lpData, cbData);

			return handlef;
		}
	}
	return 0;
}

//RegSetValueEx 替换函数
LONG WINAPI MyRegSetValueExA(
  HKEY hKey,           // handle to key
  LPCTSTR lpValueName, // value name
  DWORD Reserved,      // reserved
  DWORD dwType,        // value type
  CONST BYTE *lpData,  // value data
  DWORD cbData         // size of value data
)
{
	//printf("替换RegSetValueEx 成功!\n");
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	char hKeyFullName[MAX_PATH];
	strcpy(hKeyFullName, hKeyName.c_str());
	strcat(hKeyFullName, "VALUE:");
	strcat(hKeyFullName, lpValueName);
	// 记录操作
	::BuildSqlSentence(hKeyFullName,"RegSetValueEx");
	
	if (isBlack){
		if (MessageBox(NULL, hKeyFullName,"RegSetValue", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegSetValueExA pfnRegSetValueExA = (PFNRegSetValueExA)GetProcAddress(LoadLibrary("advapi32.dll"), "RegSetValueExA");
			LONG handlef = pfnRegSetValueExA( hKey, lpValueName, Reserved, dwType, lpData, cbData);
	
			return handlef;
		}
	}
	return 0;
}

LONG WINAPI MyRegSetValueExW(
  HKEY hKey,           // handle to key
  LPCWSTR lpValueName, // value name
  DWORD Reserved,      // reserved
  DWORD dwType,        // value type
  CONST BYTE *lpData,  // value data
  DWORD cbData         // size of value data
)
{
	//printf("替换RegSetValueEx 成功!\n");
	//Add you operator here
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	char hKeyFullName[MAX_PATH];
	strcpy(hKeyFullName, hKeyName.c_str());
	string lpValueNames = UnicodeToANSI(lpValueName);
	strcat(hKeyFullName, "VALUE:");
	strcat(hKeyFullName, lpValueNames.c_str());
	// 记录操作
	::BuildSqlSentence(hKeyFullName,"RegSetValue");
	
	if (isBlack){
		if (MessageBox(NULL, hKeyFullName,"RegSetValue", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegSetValueExW pfnRegSetValueExW = (PFNRegSetValueExW)GetProcAddress(LoadLibrary("advapi32.dll"), "RegSetValueExA");
			LONG handlef = pfnRegSetValueExW( hKey, lpValueName, Reserved, dwType, lpData, cbData);

			return handlef;
		}
	}
	return 0;
}

//RegSetKeySecurity 替换函数
LONG WINAPI MyRegSetKeySecurity(
  HKEY hKey,                                // handle to key
  SECURITY_INFORMATION SecurityInformation, // request
  PSECURITY_DESCRIPTOR pSecurityDescriptor  // SD
  )
{
	//printf("替换RegSetKeySecurity 成功!\n");
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	// 记录操作
	::BuildSqlSentence(hKeyName.c_str(),"RegSetKeySecurity");
	
	if (isBlack){
		if (MessageBox(NULL, hKeyName.c_str(),"RegSetKeySecurity", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
		PFNRegSetKeySecurity pfnRegSetKeySecurity = (PFNRegSetKeySecurity)GetProcAddress(LoadLibrary("advapi32.dll"), "RegSetKeySecurity");
		LONG handlef = pfnRegSetKeySecurity( hKey, SecurityInformation, pSecurityDescriptor);

		return handlef;
		}
	}
	return 0;
}

//RegLoadKey 替换函数
LONG WINAPI MyRegLoadKeyA(
  HKEY hKey,        // handle to open key
  LPCTSTR lpSubKey, // subkey name
  LPCTSTR lpFile    // registry file name
)
{
	//printf("替换RegLoadKey 成功!\n");
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	hKeyName.append("\\");
	hKeyName.append(lpSubKey);
	// 记录操作
	::BuildSqlSentence(hKeyName.c_str(),"RegLoadKey");

	if (isBlack){
		if (MessageBox(NULL, hKeyName.c_str(),"RegLoadKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
		PFNRegLoadKeyA pfnRegLoadKeyA = (PFNRegLoadKeyA)GetProcAddress(LoadLibrary("advapi32.dll"), "RegLoadKeyA");
		LONG handlef = pfnRegLoadKeyA( hKey, lpSubKey, lpFile);

		return handlef;
		}
	}
	return 0;
}

LONG WINAPI MyRegLoadKeyW(
  HKEY hKey,        // handle to open key
  LPCWSTR lpSubKey, // subkey name
  LPCWSTR lpFile    // registry file name
)
{
	//printf("替换RegLoadKey 成功!\n");
	//Add you operator here
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	char hKeyFullName[MAX_PATH];
	strcpy(hKeyFullName, hKeyName.c_str());
	// 记录操作
	string slpSubKey = UnicodeToANSI(lpSubKey);
	strcat(hKeyFullName, "\\");
	strcat(hKeyFullName, slpSubKey.c_str());
	::BuildSqlSentence(hKeyFullName,"RegLoadKey");
	slpSubKey.clear();
	
	if (isBlack){
		if (MessageBox(NULL, hKeyName.c_str(),"RegLoadKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegLoadKeyW pfnRegLoadKeyW = (PFNRegLoadKeyW)GetProcAddress(LoadLibrary("advapi32.dll"), "RegLoadKeyW");
			LONG handlef = pfnRegLoadKeyW( hKey, lpSubKey, lpFile);

			return handlef;
		}
	}
	return 0;
}

//RegSaveKey 替换函数
LONG WINAPI MyRegSaveKeyA(
  HKEY hKey,                                  // handle to key
  LPCTSTR lpFile,                             // data file
  LPSECURITY_ATTRIBUTES lpSecurityAttributes  // SD
){
	//printf("替换RegSaveKey 成功!\n");
	// 记录操作
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	::BuildSqlSentence(hKeyName.c_str(),"RegSaveKey");
	
	if (isBlack){
		if (MessageBox(NULL, hKeyName.c_str(),"RegSaveKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegSaveKeyA pfnRegSaveKeyA= (PFNRegSaveKeyA)GetProcAddress(LoadLibrary("advapi32.dll"), "RegSaveKeyA");
			LONG handlef = pfnRegSaveKeyA( hKey, lpFile, lpSecurityAttributes);

			return handlef;
		}
	}
	return 0;
}

LONG WINAPI MyRegSaveKeyW(
  HKEY hKey,                                  // handle to key
  LPCWSTR lpFile,                             // data file
  LPSECURITY_ATTRIBUTES lpSecurityAttributes  // SD
){
	//printf("替换RegSaveKey 成功!\n");
	//Add you operator here
	// 记录操作
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	::BuildSqlSentence(hKeyName.c_str(),"RegSaveKey");
	
	if (isBlack){
		if (MessageBox(NULL, hKeyName.c_str(),"RegSaveKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegSaveKeyW pfnRegSaveKeyW = (PFNRegSaveKeyW)GetProcAddress(LoadLibrary("advapi32.dll"), "RegSaveKeyW");
			LONG handlef = pfnRegSaveKeyW( hKey, lpFile, lpSecurityAttributes);

			return handlef;
		}
	}
	return 0;
}

//RegSaveKeyEx 替换函数
LONG WINAPI MyRegSaveKeyExA(
  HKEY hKey,  
  LPCTSTR lpFile,  
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,  
  DWORD Flags  
  ){
	//printf("替换RegSaveKeyEx 成功!\n");
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	// 记录操作
	::BuildSqlSentence(hKeyName.c_str(),"RegSaveKey");
	
	if (isBlack){
		if (MessageBox(NULL, hKeyName.c_str(),"RegSaveKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegSaveKeyExA pfnRegSaveKeyExA = (PFNRegSaveKeyExA)GetProcAddress(LoadLibrary("advapi32.dll"), "RegSaveKeyExA");
			LONG handlef = pfnRegSaveKeyExA( hKey, lpFile, lpSecurityAttributes, Flags);

			return handlef;
		}
	}
	return 0;
}

LONG WINAPI MyRegSaveKeyExW(
  HKEY hKey,  
  LPCWSTR lpFile,  
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,  
  DWORD Flags  
  ){
	//printf("替换RegSaveKeyEx 成功!\n");
	//Add you operator here
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	// 记录操作
	::BuildSqlSentence(hKeyName.c_str(),"RegSaveKey");
	
	if (isBlack){
		if (MessageBox(NULL, hKeyName.c_str(),"RegSaveKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegSaveKeyExW pfnRegSaveKeyExW = (PFNRegSaveKeyExW)GetProcAddress(LoadLibrary("advapi32.dll"), "RegSaveKeyExW");
			LONG handlef = pfnRegSaveKeyExW( hKey, lpFile, lpSecurityAttributes, Flags);
	
			return handlef;
		}
	}
	return 0;
}

// RegNotifyChangeKeyValue 替换函数
LONG WINAPI MyRegNotifyChangeKeyValue(HKEY hKey,BOOL bWatchSubtree,DWORD dwNotifyFilter,HANDLE hEvent,BOOL fAsynchronous )
{
	//printf("替换RegNotifyChangeKeyValue 成功!\n");
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	::BuildSqlSentence(hKeyName.c_str(),"RegNotifyChangeKeyValue");
	
	if (isBlack){
		if (MessageBox(NULL, hKeyName.c_str(),"RegNotifyChangeKeyValue", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegNotifyChangeKeyValue pfnRegNotifyChangeKeyValue = (PFNRegNotifyChangeKeyValue)GetProcAddress(LoadLibrary("advapi32.dll"), "RegNotifyChangeKeyValue");
			LONG handlef = pfnRegNotifyChangeKeyValue(hKey,bWatchSubtree,dwNotifyFilter,hEvent,fAsynchronous);
			return handlef;
		}
	}
	return 0;
}

//RegReplaceKey 替换函数
LONG WINAPI MyRegReplaceKeyA(
  HKEY hKey,         // handle to open key
  LPCTSTR lpSubKey,  // subkey name
  LPCTSTR lpNewFile, // data file
  LPCTSTR lpOldFile  // backup file
  ){
	//printf("替换RegReplaceKey 成功!\n");
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	char hKeyFullName[MAX_PATH];
	strcpy(hKeyFullName, hKeyName.c_str());
	strcat(hKeyFullName, "\\");
	strcat(hKeyFullName, lpSubKey);
	::BuildSqlSentence(hKeyFullName,"RegReplaceKey");
	
	if (isBlack){
		if (MessageBox(NULL, hKeyFullName,"RegReplaceKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegReplaceKeyA pfnRegReplaceKeyA = (PFNRegReplaceKeyA)GetProcAddress(LoadLibrary("advapi32.dll"), "RegReplaceKeyA");
			LONG handlef = pfnRegReplaceKeyA(hKey,lpSubKey, lpNewFile, lpOldFile);
			return handlef;
		}
	}
	return 0;
}

LONG WINAPI MyRegReplaceKeyW(
  HKEY hKey,         // handle to open key
  LPCWSTR lpSubKey,  // subkey name
  LPCWSTR lpNewFile, // data file
  LPCWSTR lpOldFile  // backup file
  ){
	//printf("替换RegReplaceKey 成功!\n");
	//Add you operator here
	 wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	char hKeyFullName[MAX_PATH];
	strcpy(hKeyFullName, hKeyName.c_str());
	// 记录操作
	string slpSubKey = UnicodeToANSI(lpSubKey);
	strcat(hKeyFullName, "\\");
	strcat(hKeyFullName, slpSubKey.c_str());
	::BuildSqlSentence(hKeyFullName,"RegReplaceKey");
	slpSubKey.clear();
	
	if (isBlack){
		if (MessageBox(NULL, hKeyFullName,"RegReplaceKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegReplaceKeyW pfnRegReplaceKeyW = (PFNRegReplaceKeyW)GetProcAddress(LoadLibrary("advapi32.dll"), "RegReplaceKeyW");
			LONG handlef = pfnRegReplaceKeyW(hKey,lpSubKey, lpNewFile, lpOldFile);

			return handlef;
		}
	}
	return 0;
}

// RegUnLoadKey 替换函数
LONG WINAPI MyRegUnLoadKeyA(
  HKEY hKey,         // handle to open key
  LPCTSTR lpSubKey   // subkey name
)
{
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	char hKeyFullName[MAX_PATH];
	strcpy(hKeyFullName, hKeyName.c_str());
	strcat(hKeyFullName, "\\");
	strcat(hKeyFullName, lpSubKey);
	::BuildSqlSentence(hKeyFullName,"RegUnLoadKey");
	
	if (isBlack){
		if (MessageBox(NULL, hKeyFullName,"RegUnloadKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegUnLoadKeyA pfnRegUnLoadKeyA = (PFNRegUnLoadKeyA)GetProcAddress(LoadLibrary("advapi32.dll"), "RegUnLoadKeyA");
			LONG handlef = pfnRegUnLoadKeyA(hKey,lpSubKey);

			return handlef;
		}
	}
	return 0;
}

LONG WINAPI MyRegUnLoadKeyW(
  HKEY hKey,         // handle to open key
  LPCWSTR lpSubKey   // subkey name
)
{
	wstring hKeyNamew = GetKeyPathFromKKEY(hKey);
	string hKeyName = UnicodeToANSI(hKeyNamew);
	char hKeyFullName[MAX_PATH];
	strcpy(hKeyFullName, hKeyName.c_str());
	string slpSubKey = UnicodeToANSI(lpSubKey);
	strcat(hKeyFullName, "\\");
	strcat(hKeyFullName, slpSubKey.c_str());
	::BuildSqlSentence(hKeyFullName,"RegUnLoadKey");
	slpSubKey.clear();
	
	if (isBlack){
		if (MessageBox(NULL, hKeyFullName,"RegUnloadKey", MB_ICONEXCLAMATION|MB_OKCANCEL ) == IDOK){
			PFNRegUnLoadKeyW pfnRegUnLoadKeyW = (PFNRegUnLoadKeyW)GetProcAddress(LoadLibrary("advapi32.dll"), "RegUnLoadKeyW");
			LONG handlef = pfnRegUnLoadKeyW(hKey,lpSubKey);

			return handlef;
		}
	}
	return 0;
}

void ReadApiName(/*API *Api*/)
{
	int i = 0;
	FILE *fp = fopen("C:\\ApiNameText.txt","r");
	HMODULE APIHmodule = ::LoadLibrary("advapi32.dll");
	while(!feof(fp))
	{
		::fgets((ApiNode[i].ApiName),256,fp);
		ApiNode[i].used = true;
		i++;
	}
	//API 地址 初始化
	ApiNode[0].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegCreateKeyExA");
	ApiNode[1].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegCreateKeyExW");
	ApiNode[2].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegCreateKeyA");
	ApiNode[3].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegCreateKeyW");
	ApiNode[4].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegDeleteKeyA");
	ApiNode[5].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegDeleteKeyW");
	ApiNode[6].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegSetValueA");
	ApiNode[7].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegSetValueW");
	ApiNode[8].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegSetValueExA");
	ApiNode[9].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegSetValueExW");
	ApiNode[10].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegSetKeySecurity");
	ApiNode[11].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegLoadKeyA");
	ApiNode[12].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegLoadKeyW");
	ApiNode[13].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegSaveKeyA");
	ApiNode[14].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegSaveKeyW");
	ApiNode[15].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegSaveKeyExA");
	ApiNode[16].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegSaveKeyExW");
	ApiNode[17].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegNotifyChangeKeyValue");
	ApiNode[18].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegReplaceKeyA");
	ApiNode[19].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegReplaceKeyW");
	ApiNode[20].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegUnLoadKeyA");
	ApiNode[21].ApiAddress = (PROC)::GetProcAddress(APIHmodule,"RegUnLoadKeyW");

	//API 替换函数初始化
	ApiNode[0].ReplaceApiAddress = (PROC)MyRegCreateKeyExA;
	ApiNode[1].ReplaceApiAddress = (PROC)MyRegCreateKeyExW;
	ApiNode[2].ReplaceApiAddress = (PROC)MyRegCreateKeyA;
	ApiNode[3].ReplaceApiAddress = (PROC)MyRegCreateKeyW;
	ApiNode[4].ReplaceApiAddress = (PROC)MyRegDeleteKeyA;
	ApiNode[5].ReplaceApiAddress = (PROC)MyRegDeleteKeyW;
	ApiNode[6].ReplaceApiAddress = (PROC)MyRegSetValueA;
	ApiNode[7].ReplaceApiAddress = (PROC)MyRegSetValueW;
	ApiNode[8].ReplaceApiAddress = (PROC)MyRegSetValueExA;
	ApiNode[9].ReplaceApiAddress = (PROC)MyRegSetValueExW;
	ApiNode[10].ReplaceApiAddress = (PROC)MyRegSetKeySecurity;
	ApiNode[11].ReplaceApiAddress = (PROC)MyRegLoadKeyA;
	ApiNode[12].ReplaceApiAddress = (PROC)MyRegLoadKeyW;
	ApiNode[13].ReplaceApiAddress = (PROC)MyRegSaveKeyA;
	ApiNode[14].ReplaceApiAddress = (PROC)MyRegSaveKeyW;
	ApiNode[15].ReplaceApiAddress = (PROC)MyRegSaveKeyExA;
	ApiNode[16].ReplaceApiAddress = (PROC)MyRegSaveKeyExW;
	ApiNode[17].ReplaceApiAddress = (PROC)MyRegNotifyChangeKeyValue;
	ApiNode[18].ReplaceApiAddress = (PROC)MyRegReplaceKeyA;
	ApiNode[19].ReplaceApiAddress = (PROC)MyRegReplaceKeyW;
	ApiNode[20].ReplaceApiAddress = (PROC)MyRegUnLoadKeyA;
	ApiNode[21].ReplaceApiAddress = (PROC)MyRegUnLoadKeyW;
 }

void RecoverIAT()
{
	for(int i = 0;i <= 21;i++)
	{
		if(ApiNode[i].Replaced&&ApiNode[i].used)
		{
			//HMODULE APIHmodule = ::LoadLibrary("advapi32.dll");
			::SetHook(::GetModuleHandle(NULL), "advapi32.dll",(PROC)ApiNode[i].ReplaceApiAddress,(PROC)ApiNode[i].ApiAddress);
		}
	}
	//::ReleaseMysqlHandle();
	return ;
}


//恢复原来的PE文件的导入表
void ReplaceAPIMain()
{
	::ReadApiName();
	for(int i = 0;i <= 21;i++)
	{
		if(ApiNode[i].used == true)
			ApiNode[i].Replaced = ::SetHook(::GetModuleHandle(NULL), "advapi32.dll",(PROC)ApiNode[i].ApiAddress,(PROC)ApiNode[i].ReplaceApiAddress);
		else
			ApiNode[i].Replaced = false;
	}
	return ;
}
