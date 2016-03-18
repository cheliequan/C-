#include "stdafx.h"
#include "DB.h"

static HENV henv;			//���ݿ⻷�����
static HDBC hDbc;	

static char sql[1000000]; // if not static when function return ,corrupt!

static char *Rsql = "%s, %s, %s, %s, %s\r\n";
static char *Fsql = "%s, %s, %s, %s, %s\r\n";


//�������ݿⲢ�½�RegChangeRecorde��
BOOL MysqlConnect()
{
	//NULL��ODBC_ADD_DSN��"MySQL ODBC 3.51 Driver"��"DSN=odbctest\0 UID=root\0 PWD=\0 SERVER=localhost\0 DATABASE=odbc\0\0
	RETCODE retcode;
	retcode = ::SQLAllocEnv(&henv);
	if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		retcode = ::SQLAllocConnect(henv,&hDbc);
		if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
		{
			retcode = ::SQLConnect(hDbc,(SQLCHAR *)"MysqlODBCA",SQL_NTS,(SQLCHAR *)"root",SQL_NTS,(SQLCHAR *)"root",SQL_NTS);
			if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				////printf("Mysql connect success!\n");
				//������¼��
				HSTMT hstmt;
				retcode = ::SQLAllocStmt(hDbc,&hstmt);
				if(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				{
					retcode = ::SQLExecDirect(hstmt,(SQLCHAR *)"create table if not exists RegChangeRecorde(ProcessName varchar(32),RegRootName varchar(32),Date varchar(11),time varchar(9),RegItem varchar(32),Operation varchar(32))",SQL_NTS);
					if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
					{
						return FALSE;
					}
					retcode = ::SQLExecDirect(hstmt,(SQLCHAR *)"create table if not exists FileChangeRecorde(ProcessName varchar(32),FileName varchar(32),Date varchar(11),time varchar(9),type varchar(32))",SQL_NTS);
					if(retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
					{
						return FALSE;
					}
				}
				
			}
		}
	}
	return TRUE;
}

//Release Mysql handle
void ReleaseMysqlHandle()
{
	::SQLFreeConnect(hDbc);
	::SQLFreeEnv(henv);
}

//�����ݿ�洢����
BOOL MysqlAddData(UCHAR * Sql)
{
	HSTMT hstmt;
	RETCODE retcode;

	//���������
	retcode = ::SQLAllocStmt(hDbc,&hstmt);
	if(retcode != SQL_SUCCESS)          //����ʧ�� ����FALSE
	{
		//MessageBoxA(NULL,"Data insert fail 1!","tip",MB_OK);
		return FALSE;
	}
	//ִ��Sql���
	retcode = ::SQLExecDirect(hstmt,Sql,SQL_NTS);
	if(retcode != SQL_SUCCESS)		//ִ��ʧ�� �ͷ������ ����FALSE
	{
		::SQLFreeHandle(SQL_HANDLE_STMT,hstmt);
		//MessageBoxA(NULL,"Data insert fail 2!","tip",MB_OK);
		return FALSE;
	}
	//�ύ����
	retcode = SQLEndTran(SQL_HANDLE_DBC , hDbc, SQL_COMMIT);
	::SQLFreeHandle(SQL_HANDLE_STMT,hstmt);
	if(retcode == SQL_SUCCESS)	
	{
		//MessageBoxA(NULL,"Data insert success!","tip",MB_OK);
		return TRUE;
	}
	else
	{
		SQLEndTran(SQL_HANDLE_DBC , hDbc, SQL_ROLLBACK);
		//MessageBoxA(NULL,"Data insert fail 3!","tip",MB_OK);
		return FALSE;
	}
}

BOOL WriteFileRecord(char *buff)
{
	char szDllPath[MAX_PATH+1];
	/*GetCurrentDirectory(MAX_PATH,szDllPath);*/
	char Date[11];
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyyMMdd",Date,11);
	strcpy(szDllPath, "c:\\RegAndFile\\File\\");
	strcat(szDllPath, Date);
	strcat(szDllPath, "f.txt");
	FILE *fp = fopen(szDllPath, "a+");
	if (!fp)
		return FALSE;
	fprintf(fp, buff);
	fclose(fp);
}

BOOL WriteRegRecord(char *buff)
{
	char szDllPath[MAX_PATH+1];
	/*GetCurrentDirectory(MAX_PATH,szDllPath);*/
	char Date[11];
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyyMMdd",Date,11);
	strcpy(szDllPath, "c:\\RegAndFile\\Reg\\");
	strcat(szDllPath, Date);
	strcat(szDllPath, "r.txt");
	FILE *fp = fopen(szDllPath, "a+");
	if (!fp)
		return FALSE;
	fprintf(fp, buff);
	fclose(fp);
}

//�ж�RegRoot�������ȡ��������
void GetRegRootName(HKEY hKey, char *hKeyName)
{	
	if(hKey == HKEY_CLASSES_ROOT)
	{
		strcpy(hKeyName,"HKEY_CLASSES_ROOT");
	}
	else if(hKey == HKEY_CURRENT_USER)
	{
		strcpy(hKeyName,"HKEY_CURRENT_USER");
	}
	else if(hKey == HKEY_LOCAL_MACHINE)
	{
		strcpy(hKeyName,"HKEY_LOCAL_MACHINE");
	}
	else if(hKey == HKEY_USERS)
	{
		strcpy(hKeyName,"HKEY_USERS");
	}
	else if(hKey == HKEY_CURRENT_CONFIG)
	{
		strcpy(hKeyName,"HKEY_CURRENT_CONFIG");
	}
	else
	{
		DWORD len = 256;
		::RegQueryInfoKey(hKey,hKeyName,&len,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
	}
	return ;
}

char *GetCurProcessName()
{
	//HANDLE CurHandle = 
	long ProcessId = ::GetCurrentProcessId();
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

//�γ�SQLִ�����
BOOL BuildSqlSentence(LPCTSTR RegFullName,REG_OPERATION_NAME OperationName)
{
	//char *SubKeyName;		 //�Ӽ�����
	char Time[9];			 // ʱ��
	char Date[11];			 //����
	char ProcessName[256];		 //������
	//char *Sql = new char[1000];
	char *Sql;                  //Sql���
	
	//��ȡ��ǰʱ��
	//SYSTEMTIME SystemTime;
	::GetTimeFormatA(LOCALE_USER_DEFAULT,LOCALE_USE_CP_ACP|TIME_FORCE24HOURFORMAT,NULL,"HH':'mm':'ss",Time,9);
	::GetDateFormatA(LOCALE_USER_DEFAULT,NULL,NULL,"yyyy'-'MM'-'dd",Date,11);
	
	//��ȡ������
	strcpy(ProcessName,::GetCurProcessName());

	//��װSql���
	sprintf(sql, Rsql, ProcessName, Date, Time, RegFullName, OperationName);
	WriteRegRecord(sql);
	/*vRsql.push_back(sql);
	if (vRsql.size() > 30)
		if (!WriteRegRecord(vRsql))
			return FALSE;*/
	return TRUE;
}

//�γ�SQLִ�����
BOOL BuildSqlSentencef(const char *ProcessName, const char *FileName, char *type, char *OperationDate, char *OperationTime)
{                 //Sql���
	// �������ƻ�ȡ
	//��װSql��� 
	sprintf(sql, Fsql, ProcessName, FileName, OperationDate, OperationTime, type);
	WriteFileRecord(sql);
	return TRUE;
}