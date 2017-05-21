#include "StdAfx.h"
#include "IncomingWindow.h"
#include "ServerWindow.h"
#include <atltypes.h>

#define ID_Window_Btn_ACCEPT 101
#define ID_Window_Btn_REJECT 102
#define IDC_CALLER_IP        103
#define IDC_CALLER_NAME      104
#define IDC_CALLER_TIME      105

#define TIME_EVENT           1000

CIncomingWindow::CIncomingWindow(CServerWindow *pParent)
	: m_hWindow(NULL)
	, m_hBtnAccept(NULL)
    , m_hBtnReject(NULL)
	, m_dwCounts(0)
	, m_pParent(pParent)
{
}


CIncomingWindow::~CIncomingWindow()
{
}

bool CIncomingWindow::Create(HXCGUI &hParent)
{
	//加载资源文件,不能再次XC_LoadResource资源,否则报错 XImage_GetWidth() 输入句柄可能无效
	//加载布局文件
	HXCGUI hXCGUI = XC_LoadLayout(L"res\\LayoutIncomming.xml");
	m_hWindow     = (HWINDOW)hXCGUI;
	m_hBtnAccept  = (HELE)XC_GetObjectByID(m_hWindow, ID_Window_Btn_ACCEPT);
	XEle_RegEventCPP(m_hBtnAccept, XE_BNCLICK, &CIncomingWindow::OnBnClickedAccept);
	m_hBtnReject  = (HELE)XC_GetObjectByID(m_hWindow, ID_Window_Btn_REJECT);
	XEle_RegEventCPP(m_hBtnReject, XE_BNCLICK, &CIncomingWindow::OnBnClickedReject);
	//timer
	XWnd_RegEventCPP(m_hWindow, WM_TIMER, &CIncomingWindow::OnWndTimer);
	XWnd_ShowWindow( m_hWindow, SW_HIDE);

	return true;
}


int CIncomingWindow::OnBnClickedAccept(BOOL *pbHandled)
{
	XEle_ShowEle(m_hBtnAccept, FALSE);
	XEle_ShowEle(m_hBtnReject, TRUE);
	XWnd_RedrawWnd(m_hWindow, TRUE);
	CRect rect;
	XWnd_GetClientRect(m_hWindow, &rect);
	CRect rectRejcetButton;
	XEle_GetRect(m_hBtnReject, &rectRejcetButton);
	int x = (rect.Width() - rectRejcetButton.Width()) / 2;
	RECT rectNewPos;
	rectNewPos.left  = x;
	rectNewPos.top   = rectRejcetButton.top;
	rectNewPos.right = rectNewPos.left + rectRejcetButton.Width();
	rectNewPos.bottom= rectNewPos.top + rectRejcetButton.Height();

	XEle_SetRect(m_hBtnReject, &rectNewPos, TRUE);

	XWnd_SetTimer(m_hWindow, TIME_EVENT, 1000);

	m_dwCounts = GetTickCount();

	if (NULL != m_pParent)
	{
		SetEvent(m_pParent->m_hAcceptCallEvents[0]);
	}

	*pbHandled = TRUE;

	return 0;
}


int CIncomingWindow::OnBnClickedReject(BOOL *pbHandled)
{
	XWnd_KillTimer(m_hWindow, TIME_EVENT);
	if (NULL != m_pParent)
	{
		if (m_pParent->m_callStatus == ACCEPTING)
		{
			SetEvent(m_pParent->m_hAcceptCallEvents[1]);
		}
		m_pParent->m_talk.End();
	}
	//不接听,直接挂断
	UpdataWindow(m_info);

	*pbHandled = TRUE;

	return 0;
}


void CIncomingWindow::UpdataWindow( CallingInfo &info, bool bIncoming)
{
	if (bIncoming)
	{
		m_info = info;
	}
	//Update IP address
	HXCGUI hText = (HXCGUI)XC_GetObjectByID(m_hWindow, IDC_CALLER_IP);
	wchar_t wcBuffer[64];
	wsprintfW(wcBuffer, L"IP:%s", info.szClientIpAddress);
	XShapeText_SetText(hText, wcBuffer);
	//update client name
	hText = (HXCGUI)XC_GetObjectByID(m_hWindow, IDC_CALLER_NAME);
	wsprintfW(wcBuffer, L"机器名:%s", info.szClientName);
	XShapeText_SetText(hText, wcBuffer);
	//update call time
	////来电时不显示
	//if (!bIncoming)
	//{
	//	wsprintfW(info.wcCallTime, L"通话时长:00:00:00");
	//	hText = (HXCGUI)XC_GetObjectByID(m_hWindow, IDC_CALLER_TIME);
	//	XShapeText_SetText(hText, wcBuffer);
	//}
	//获取界面上显示的通话时长作为通话时长信息
	HXCGUI hTextBlock = (HXCGUI)XC_GetObjectByID(m_hWindow, IDC_CALLER_TIME);
	XShapeText_GetText(hTextBlock, info.wcCallTime, wcslen(info.wcCallTime));

	//show button
	XEle_ShowEle(m_hBtnAccept, TRUE);
	XEle_ShowEle(m_hBtnReject, TRUE);
	XWnd_KillXCTimer(m_hWindow, TIME_EVENT);

	CRect rect;
	XWnd_GetClientRect(m_hWindow, &rect);

	CRect rectAcceptButton;
	XEle_GetRect(m_hBtnAccept, &rectAcceptButton);
	CRect rectRejectButton;
	XEle_GetRect(m_hBtnReject, &rectRejectButton);
	//计算中间位置
	int x = rectAcceptButton.right + (rect.Width() - rectAcceptButton.left - rectAcceptButton.Width() - rectRejectButton.Width()) / 2;
	RECT rectNewPos;
	rectNewPos.left = x;
	rectNewPos.top = rectRejectButton.top;
	rectNewPos.right = rectNewPos.left + rectRejectButton.Width();
	rectNewPos.bottom = rectNewPos.top + rectRejectButton.Height();
	XEle_SetRect(m_hBtnReject, &rectNewPos, TRUE);
	XWnd_ShowWindow(m_hWindow, bIncoming ? SW_NORMAL : SW_HIDE);
}


int CIncomingWindow::OnWndTimer(UINT nIDEvent, BOOL *pbHandled)
{
	if (TIME_EVENT == nIDEvent)
	{
		DWORD dwTickCount = GetTickCount();
		DWORD dwEscape = (dwTickCount - m_dwCounts) / 1000;
		wchar_t wcTime[64];
		wsprintfW(wcTime, L"通话时长:%02d:%02d:%02d", dwEscape / 3600, dwEscape % 3600 / 60, dwEscape % 60);
		HXCGUI hTextBlock = (HXCGUI)XC_GetObjectByID(m_hWindow, IDC_CALLER_TIME);
		XShapeText_SetText(hTextBlock, wcTime);
		XShape_Redraw(hTextBlock);
	}

	return 0;
}

