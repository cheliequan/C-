#include "stdafx.h"
#include <afxdb.h>
#include "windows.h"
#include <vector>
#include "TlHelp32.h"


typedef char * REG_OPERATION_NAME;
		
//�������ݿⲢ�½�RegChangeRecorde��
BOOL MysqlConnect();

//Release Mysql handle
void ReleaseMysqlHandle();

//�����ݿ�洢����
BOOL MysqlAddData(UCHAR * Sql);

void GetRegRootName(HKEY hKey, char *hKeyName);

//��ȡ��ǰ������

//�γ�SQLִ�����
BOOL BuildSqlSentence(LPCTSTR RegFullName,REG_OPERATION_NAME OperationName);

//�γ�SQLִ�����
BOOL BuildSqlSentencef(const char *ProcessName, const char *FileName, char *type, char *OperationDate, char *OperationTime);

BOOL WriteFileRecord(char *buff);
BOOL WriteRegRecord(char *buff);
char *GetCurProcessName();