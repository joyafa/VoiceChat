// VoiceChat.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "VoiceChat.h"
#include "IncommingWindow.h"

CServerWindow g_hWindow;
HINSTANCE g_hInstance = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;
	
	XInitXCGUI(); //��ʼ��
	
	g_hWindow.Create();
	
	for (int i=0;i<100;i++)
	{
		g_hWindow.InsertItemData();
	}
	XRunXCGUI(); //����
	g_hWindow.Release();
	XExitXCGUI(); //�ͷ���Դ
	
	return 0;
}

//ע�ắ����ȥ, 
//messageloop,���ö���