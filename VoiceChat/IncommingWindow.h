#pragma once

class CServerWindow;

class CIncommingWindow
{
public:
	CIncommingWindow(CServerWindow *pParent);
	virtual ~CIncommingWindow();

	bool Create(HXCGUI &hParent);
	HWINDOW m_hWindow;

private:
	int OnBnClickedAccept(BOOL *pbHandled);
	int OnBnClickedReject(BOOL *pbHandled);
	void UpdataWindow(CString &strCallingTime);
	int OnWndTimer(UINT nIDEvent, BOOL *pbHandled);
private:
	HELE m_hBtnAccept;
	HELE m_hBtnReject;
	DWORD m_dwCounts;
	CServerWindow *m_pParent;
};

