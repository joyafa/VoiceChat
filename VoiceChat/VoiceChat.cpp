// VoiceChat.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "VoiceChat.h"

CServerWindow g_hWindow;
HINSTANCE g_hInstance = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//只允许一个进程
	HANDLE hMutex = CreateMutex(NULL, TRUE, "VoiceChat");
	DWORD dwRet = GetLastError();
	if (hMutex)
	{
		if (ERROR_ALREADY_EXISTS == dwRet)
		{
			XTRACE("程序已经在运行中了,程序退出!\n");
			CloseHandle(hMutex);
			
			return 0;
		}
	}
	else
	{
		XTRACE("创建互斥量错误,程序退出!\n");
		CloseHandle(hMutex);

		return 0;
	}

	g_hInstance = hInstance;
	//初始化
	XInitXCGUI(); 

	//创建主界面
	g_hWindow.Create();
	/*g_hWindow.InsertItemData("wind", "192.168.10.75");
	g_hWindow.InsertItemData("china", "192.168.10.11");
	g_hWindow.InsertItemData("good", "192.168.10.22");
	g_hWindow.InsertItemData("myself", "192.168.10.33");
	g_hWindow.InsertItemData("joyafa", "192.168.10.44");*/

	//XEle_SetToolTip();

	XRunXCGUI(); //运行

	g_hWindow.Release();

	XExitXCGUI(); //释放资源
	
	CloseHandle(hMutex);

	return 0;
}
