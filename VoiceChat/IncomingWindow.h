#pragma once

class CServerWindow;
struct CallingInfo
{
	char szClientIpAddress[64]; //IP地址
	char szClientName[128];     //机器名称
	int  nBeginTime;            //通话开始时间:hhmmssmmm
	int  nEndTime;              //通话结束时间:hhmmssmmm
								//TODO:怎么体现未接听
	wchar_t wcCallTime[64];     //通话时长

	CallingInfo()
	{
		memset(this, 0, sizeof(CallingInfo));
		wsprintfW(this->wcCallTime, L"通话时长:00:00:00");
	}
};

class CIncomingWindow
{
public:
	CIncomingWindow(CServerWindow *pParent );
	virtual ~CIncomingWindow();

	bool Create(HXCGUI &hParent);
	HWINDOW m_hWindow;
	void UpdataWindow(CallingInfo &info, bool bIncoming = false);

private:
	int OnBnClickedAccept(BOOL *pbHandled);
	int OnBnClickedReject(BOOL *pbHandled);
	int OnWndTimer(UINT nIDEvent, BOOL *pbHandled);
private:
	HELE m_hBtnAccept;
	HELE m_hBtnReject;
	DWORD m_dwCounts;
	CServerWindow *m_pParent;
	CallingInfo m_info;
};

