#include "StdAfx.h"
#include "TrayEx.h"
#include "shellapi.h"
#include "resource.h"

extern HINSTANCE g_hInstance;
#define  WM_SHOWTASK (WM_USER+100)
void CTrayEx::Create(HWINDOW hWindow)
{
	m_hWindow = hWindow;
	XWnd_RegEventCPP(m_hWindow, WM_SHOWTASK, &CTrayEx::OnWndTray);
	XWnd_RegEventCPP(m_hWindow, WM_DESTROY, &CTrayEx::OnWndDestroy);
	XWnd_RegEventCPP(m_hWindow, XWM_MENU_SELECT, &CTrayEx::OnWndMenuSelect);
	XWnd_RegEventCPP(m_hWindow, WM_SHOWWINDOW, &CTrayEx::OnShowWindow);
}

void CTrayEx::CreateTray()
{
	NOTIFYICONDATA tnd;
	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = XWnd_GetHWND(m_hWindow);
	tnd.uID = 100;
	tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	tnd.uCallbackMessage = WM_SHOWTASK; //�û��Զ������Ϣ����������������ϳ���ͼ���϶���ʱͼ�귢�͵���Ϣ
	tnd.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_VOICECHAT));
	
	strcpy(tnd.szTip, "ͨ�������");//ͼ����ʾΪ"ǰ̨"
	Shell_NotifyIcon(NIM_ADD, &tnd);//�����������ͼ��
}


void CTrayEx::DeleteTray()
{
	NOTIFYICONDATA nid;
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = XWnd_GetHWND(m_hWindow);
	nid.uID = 100;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_SHOWTASK;//�Զ������Ϣ���� 

	nid.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_VOICECHAT));
	strcpy(nid.szTip, "ͨ�������");    //��Ϣ��ʾ��Ϊ���ƻ��������ѡ� 
	Shell_NotifyIcon(NIM_DELETE, &nid);    //��������ɾ��ͼ�� 
}

int  CTrayEx::OnWndMenuSelect(int nID, BOOL *pBool)
{
	switch (nID)
	{
	case 202:
		XWnd_ShowWindow(m_hWindow, SW_SHOW);
		break;
	case 203:
		XWnd_ShowWindow(m_hWindow, SW_HIDE);
		break;
	case 204:
		XWnd_CloseWindow(m_hWindow);
		break;
	}
	XTRACE("menu-XWM_MENU_SELECT item:%d\n", nID);
	return 0;
}
int CTrayEx::OnWndTray(WPARAM wParam, LPARAM lParam, BOOL *pbHandled)
{

	if (wParam != 100)
		return 1;
	switch (lParam)
	{
		case WM_RBUTTONUP://�Ҽ�����ʱ������ݲ˵�������ֻ��һ�����رա� 
			{
				SetForegroundWindow(XWnd_GetHWND(m_hWindow));

				HMENUX hMenu = XMenu_Create();
				XMenu_AddItem(hMenu, 202, L"��ʾ������");

				POINT pt;
				GetCursorPos(&pt);
				XMenu_Popup(hMenu, XWnd_GetHWND(m_hWindow), pt.x, pt.y);

			}
			break;
		case WM_LBUTTONDBLCLK://˫������Ĵ��� 
			{
				XWnd_ShowWindow(m_hWindow, SW_SHOW);//�򵥵���ʾ���������¶�
			}
			break;
		default:
			{
				return 0;
			}
			break;
	}

	return 0;
}