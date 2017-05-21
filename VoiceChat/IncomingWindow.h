#pragma once

class CServerWindow;
struct CallingInfo
{
	char szClientIpAddress[64]; //IP��ַ
	char szClientName[128];     //��������
	int  nBeginTime;            //ͨ����ʼʱ��:hhmmssmmm
	int  nEndTime;              //ͨ������ʱ��:hhmmssmmm
								//TODO:��ô����δ����
	wchar_t wcCallTime[64];     //ͨ��ʱ��

	CallingInfo()
	{
		memset(this, 0, sizeof(CallingInfo));
		wsprintfW(this->wcCallTime, L"ͨ��ʱ��:00:00:00");
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

