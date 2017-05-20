// VoiceChat.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "VoiceChat.h"
#include "IncommingWindow.h"

CServerWindow g_hWindow;
HINSTANCE g_hInstance = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;
	
	XInitXCGUI(); //初始化
	
	g_hWindow.Create();
	
	for (int i=0;i<100;i++)
	{
		g_hWindow.InsertItemData();
	}
	XRunXCGUI(); //运行
	g_hWindow.Release();
	XExitXCGUI(); //释放资源
	
	return 0;
}

//注册函数进去, 
//messageloop,放置堵塞