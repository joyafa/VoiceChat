// VoiceChat.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "VoiceChat.h"

CServerWindow g_hWindow;
HINSTANCE g_hInstance = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//ֻ����һ������
	HANDLE hMutex = CreateMutex(NULL, TRUE, "VoiceChat");
	DWORD dwRet = GetLastError();
	if (hMutex)
	{
		if (ERROR_ALREADY_EXISTS == dwRet)
		{
			XTRACE("�����Ѿ�����������,�����˳�!\n");
			CloseHandle(hMutex);
			
			return 0;
		}
	}
	else
	{
		XTRACE("��������������,�����˳�!\n");
		CloseHandle(hMutex);

		return 0;
	}

	g_hInstance = hInstance;
	//��ʼ��
	XInitXCGUI(); 

	//����������
	g_hWindow.Create();
	/*g_hWindow.InsertItemData("wind", "192.168.10.75");
	g_hWindow.InsertItemData("china", "192.168.10.11");
	g_hWindow.InsertItemData("good", "192.168.10.22");
	g_hWindow.InsertItemData("myself", "192.168.10.33");
	g_hWindow.InsertItemData("joyafa", "192.168.10.44");*/

	//XEle_SetToolTip();

	XRunXCGUI(); //����

	g_hWindow.Release();

	XExitXCGUI(); //�ͷ���Դ
	
	CloseHandle(hMutex);

	return 0;
}
