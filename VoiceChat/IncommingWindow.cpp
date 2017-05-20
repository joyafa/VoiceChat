#include "StdAfx.h"
#include "IncommingWindow.h"
#include "ServerWindow.h"
#include <atltypes.h>

#define ID_Window_Btn_ACCEPT 101
#define ID_Window_Btn_REJECT 102
#define IDC_CALLER_IP        103
#define IDC_CALLER_NAME      104
#define IDC_CALLER_TIME      105

#define TIME_EVENT           1000

CIncommingWindow::CIncommingWindow(CServerWindow *pParent)
	: m_hWindow(NULL)
	, m_hBtnAccept(NULL)
    , m_hBtnReject(NULL)
	, m_dwCounts(0)
	, m_pParent(pParent)
{
}


CIncommingWindow::~CIncommingWindow()
{
}

bool CIncommingWindow::Create(HXCGUI &hParent)
{
	//加载资源文件
	XC_LoadResource(L"res\\resource.res", L"res");
	//加载布局文件
	HXCGUI hXCGUI = XC_LoadLayout(L"res\\LayoutIncomming.xml", hParent);
	m_hWindow = (HWINDOW)hXCGUI;
	m_hBtnAccept = (HELE)XC_GetObjectByID(m_hWindow, ID_Window_Btn_ACCEPT);
	XEle_RegEventCPP(m_hBtnAccept, XE_BNCLICK, &CIncommingWindow::OnBnClickedAccept);
	m_hBtnReject = (HELE)XC_GetObjectByID(m_hWindow, ID_Window_Btn_REJECT);
	XEle_RegEventCPP(m_hBtnReject, XE_BNCLICK, &CIncommingWindow::OnBnClickedReject);
	//timer
	XWnd_RegEventCPP(m_hWindow, WM_TIMER, &CIncommingWindow::OnWndTimer);

	XWnd_ShowWindow(m_hWindow, SW_HIDE);

	return true;
}


int CIncommingWindow::OnBnClickedAccept(BOOL *pbHandled)
{
	XWnd_ShowWindow((HWINDOW)m_hBtnAccept, SW_HIDE);
	XWnd_ShowWindow((HWINDOW)m_hBtnReject, SW_NORMAL);

	CRect rect;
	XWnd_GetClientRect(m_hWindow, &rect);
	//ScreenToClient(rect);

	CRect rectRejcetButton;
	XEle_GetClientRect(m_hBtnReject, &rectRejcetButton);
	//ScreenToClient(&rectRejcetButton);
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


int CIncommingWindow::OnBnClickedReject(BOOL *pbHandled)
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
	CString strCallingTime;
	UpdataWindow(strCallingTime);

	*pbHandled = TRUE;

	return 0;
}


void CIncommingWindow::UpdataWindow(CString &strCallingTime)
{
	//TODO
	HXCGUI HText = (HXCGUI)XC_GetObjectByID(m_hWindow, 103);
	XShapeText_SetText(HText, L"STERRET");
	XWnd_ShowWindow((HWINDOW)m_hBtnAccept, SW_NORMAL);
	XWnd_ShowWindow((HWINDOW)m_hBtnReject, SW_NORMAL);
	XWnd_KillXCTimer(m_hWindow, TIME_EVENT);
	CRect rect;
	XWnd_GetClientRect(m_hWindow, &rect);
	//ScreenToClient(&rectAcceptButton);

	CRect rectAcceptButton;
	XEle_GetClientRect(m_hBtnAccept, &rectAcceptButton);
	//ScreenToClient(&rectAcceptButton);

	CRect rectRejectButton;
	XEle_GetClientRect(m_hBtnReject, &rectRejectButton);

	int x = rectAcceptButton.right + (rect.Width() - rectAcceptButton.left - rectAcceptButton.Width() - rectRejectButton.Width()) / 2;
	RECT rectNewPos;
	rectNewPos.left = x;
	rectNewPos.top = rectRejectButton.top;
	rectNewPos.right = rectNewPos.left + rectRejectButton.Width();
	rectNewPos.bottom = rectNewPos.top + rectRejectButton.Height();

	XEle_SetRect(m_hBtnReject, &rectNewPos, TRUE);
	XWnd_ShowWindow(m_hWindow, SW_HIDE);
}


//BOOL CIncommingWindow::PreTranslateMessage(MSG* pMsg)
//{
//	if (pMsg->wParam == WM_KEYDOWN)
//	{
//		if (VK_ESCAPE == pMsg->lParam)
//		{
//			return TRUE;
//		}
//	}
//
//	return CDialogEx::PreTranslateMessage(pMsg);
//}


int CIncommingWindow::OnWndTimer(UINT nIDEvent, BOOL *pbHandled)
{
	if (TIME_EVENT == nIDEvent)
	{
		DWORD dwTickCount = GetTickCount();
		DWORD dwEscape = (dwTickCount - m_dwCounts) / 1000;
		wchar_t wcTime[64];
		wsprintfW(wcTime, L"%02d:%02d:%02d", dwEscape / 3600, dwEscape % 3600 / 60, dwEscape % 60);
		HXCGUI hTextBlock = (HXCGUI)XC_GetObjectByID(m_hWindow, IDC_CALLER_TIME);
		XShapeText_SetText(hTextBlock, wcTime);
	}

	return 0;
}

