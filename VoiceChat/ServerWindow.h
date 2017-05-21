#pragma once

#include "hardware\UsbDevice.h"
#include "MCIPlayMusic\MCIPlayMusic.h"
#include "TrayEx.h"
#include "ServiceInterface.h"
#include "IncomingWindow.h"
#include <map>

using namespace std;

enum CallStatus
{
	INITIAL = 0, //��ʼ״̬     
	DIALING = 1, //����ing
	ACCEPTING, //����ing: ���з�,������������,δ����,���ԹҶ�(Reject),���Խ���
	ONLINE, //ͨ����,���ԹҶ�(Hangup),���ߵȶԷ��Ҷ�(�յ��Է�ʲô��Ϣ)
	BUSYING         //���г�ʱ,æ��
};

class CServerWindow;

struct _tagCallFrom
{
	CString strIpName;
	CServerWindow *pWindow;
	bool bAcceptCall;
};




class CServerWindow
{
public:
	CServerWindow();
	void Release();

	//************************************
	// Method:    GetConfigInfo
	// FullName:  CServerWindow::GetConfigInfo
	// Access:    public 
	// Returns:   void
	// Description: ��ȡ�����ļ���Ϣ
	//************************************
	void GetConfigInfo();
	void Create();


	int OnWndTimer(UINT nIDEvent, BOOL *pbHandled);
	int OnTreeSelect(int m_nItem, BOOL *pbHandled);
	
	int OnTreeExpand(int id, BOOL bExpand, BOOL *pbHandled);
	
	int  OnTemplateCreate(tree_item_i *pItem, BOOL *pbHandled);
	
	int  OnTemplateDestroy(tree_item_i *pItem, BOOL *pbHandled);
	
	int  OnTemplateAdjustCoordinate(tree_item_i *pItem, BOOL *pbHandled);
	
	//************************************
	// Method:    OnCloseWindow
	// FullName:  CServerWindow::OnCloseWindow
	// Access:    public 
	// Returns:   int
	// Description: �رմ���
	// Parameter: BOOL * pbHandled
	//************************************
	int OnCloseWindow(BOOL * pbHandled);

	//usb(��Ӧ����)
	int OnHandlePhone(WPARAM wParam, LPARAM lParam, BOOL *pbHandled);

	bool AcceptCallFrom(const char* pIpAndName);
	//************************************
	// Method:    OnStopMusic
	// FullName:  CServerWindow::OnStopMusic
	// Access:    public 
	// Returns:   int
	// Description: ֹͣ��������
	// Parameter: WPARAM wParam
	// Parameter: LPARAM lParam
	// Parameter: BOOL * pbHandled
	//************************************
	int OnStopMusic(WPARAM wParam, LPARAM lParam, BOOL *pbHandled);

	int OnMCINotify(WPARAM wParam, LPARAM lParam, BOOL *pbHandled);

	//************************************
	// Method:    PlaySound
	// FullName:  CServerWindow::PlaySound
	// Access:    public 
	// Returns:   void
	// Description: ����·���������ļ�
	// Parameter: const CString & strSonndPath
	//************************************
	void PlaySound(const CString &strSonndPath);

	//************************************
	// Method:    InsertItemData
	// FullName:  CServerWindow::InsertItemData
	// Access:    public 
	// Returns:   int
	// Description: 
	// Parameter: const char * pComputerName
	// Parameter: const char * pIp
	//************************************
	int InsertItemData(const char *pComputerName, const char *pIp);


	//************************************
	// Method:    InsertCallingInfo
	// FullName:  CServerWindow::InsertCallingInfo
	// Access:    public 
	// Returns:   bool
	// Description: ��¼ͨ����Ϣ,�������������ʾ��ʾ��Ϣʱ��ʹ��
	// Parameter: int nIndex
	// Parameter: const CallingInfo * info
	//************************************
	bool InsertCallingInfo(int nIndex, const CallingInfo &info)
	{
		//TODO:��������;
		//map: key:�±�, value:CallingInfo
		return m_mapCallInfo.insert(pair<int, CallingInfo>(nIndex, info)).second;
	}

	//************************************
	// Method:    GetCallingInfo
	// FullName:  CServerWindow::GetCallingInfo
	// Access:    public 
	// Returns:   CallingInfo &
	// Description: ͨ��list������ʾ�����,��ȡ��Ϣ
	// Parameter: int nIndex
	//************************************
	CallingInfo &GetCallingInfo(int nIndex)
	{
		//TODO:��������;
		//map: key:�±�, value:CallingInfo
		map<int, CallingInfo>::iterator it = m_mapCallInfo.find(nIndex);
		if (it == m_mapCallInfo.end())
		{
			throw "Key not found!!!!";
		}

		//��������,���Ը�������
		return it->second;
	}


	//************************************
	// Method:    AddTray
	// FullName:  CServerWindow::AddTray
	// Access:    public 
	// Returns:   void
	// Qualifier: ����ϵͳ����
	//************************************
	void AddTray();
	
public:
	//CIncommingDialog *m_pIncommingDlg;
	//�����¼�: 0:�����¼�;1:�Ҷ��¼�
	HANDLE m_hAcceptCallEvents[2];
	//TODO:�򵥴���,1 �ͻ��˶���ͨ�����غ��з�,��������صĲ���״̬;
	//              2 ǰ̨�����,���з�,ֻ���������绰����,����ǰ̨���пͻ��˵����--> ��Ϊ�н���,��ȡ���水ť��ʽ,�����¼���ʽ
	//��¼��ǰ״̬: ���� ���� ����
	CallStatus m_callStatus;
	CServiceInterface m_talk;
	HWINDOW m_hWindow;
	int m_nItem;
	//�������ѶԻ���
	CIncomingWindow *m_pIncomingDlg;

private:
	HELE    m_hTree;
	HIMAGE  m_hVip;
	HIMAGE  m_hQZone;
	HIMAGE  m_hAvatar;
	HIMAGE  m_hAvatarLarge;
	HTEMP  m_pTemplate_group;
	HTEMP  m_pTemplate_sel;
	HXCGUI m_hAdapterTree;
	CTrayEx m_TrayEx;	
	
	//Ӳ��PID,VID
	DWORD m_dwPID;
	DWORD m_dwVID;
	CString m_strServiceIP;
	//��������
	CString m_strPathDialingBell;
	//��������
	CString m_strPathIncommingBell;
	//æ��
	CString m_strPathBusyBell;

	CMCIPlayMusic m_mciMusic;
	CUsbDevice *m_pUsbDevice;

	//��¼ͨ����Ϣ
	map<int, CallingInfo>m_mapCallInfo;
};


