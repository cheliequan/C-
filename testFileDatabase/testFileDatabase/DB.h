#include "stdafx.h"
#include <afxdb.h>
#include "windows.h"
#include <vector>
#include "TlHelp32.h"


typedef char * REG_OPERATION_NAME;
		
//连接数据库并新建RegChangeRecorde表
BOOL MysqlConnect();

//Release Mysql handle
void ReleaseMysqlHandle();

//向数据库存储数据
BOOL MysqlAddData(UCHAR * Sql);

void GetRegRootName(HKEY hKey, char *hKeyName);

//获取当前进程名

//形成SQL执行语句
BOOL BuildSqlSentence(LPCTSTR RegFullName,REG_OPERATION_NAME OperationName);

//形成SQL执行语句
BOOL BuildSqlSentencef(const char *ProcessName, const char *FileName, char *type, char *OperationDate, char *OperationTime);

BOOL WriteFileRecord(char *buff);
BOOL WriteRegRecord(char *buff);
char *GetCurProcessName();