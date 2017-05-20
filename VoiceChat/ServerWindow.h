#pragma once

#include "hardware\UsbDevice.h"
#include "MCIPlayMusic\MCIPlayMusic.h"
#include "TrayEx.h"
#include "ServiceInterface.h"
#include "IncommingWindow.h"

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

struct CallingInfo
{
	char szClientIpAddress[64]; //IP��ַ
	char szClientName[512];     //��������
	char szBeginTime;           //ͨ����ʼʱ�� hh:mm:ss'ms
	int  nLastTime;             //ͨ������ʱ��
};


class CServerWindow
{
public:
	CServerWindow();
	void Release();
	CString GetMoudleConfigFilePath();
	void GetConfigInfo();
	void Create();


	int OnTreeSelect(int m_nItem, BOOL *pbHandled);
	
	int OnTreeExpand(int id, BOOL bExpand, BOOL *pbHandled);
	
	int  OnTemplateCreate(tree_item_i *pItem, BOOL *pbHandled);
	
	int  OnTemplateDestroy(tree_item_i *pItem, BOOL *pbHandled);
	
	int  OnTemplateAdjustCoordinate(tree_item_i *pItem, BOOL *pbHandled);

	
	int OnCloseWindow(BOOL * pbHandled);
	//usb(��Ӧ����)
	int OnHandlePhone(WPARAM wParam, LPARAM lParam, BOOL *pbHandled);
	bool AcceptCallFrom(const char* pIpAndName);
	int OnStopMusic(WPARAM wParam, LPARAM lParam, BOOL *pbHandled);
	int OnMCINotify(WPARAM wParam, LPARAM lParam, BOOL *pbHandled);

	void PlaySound(const CString &strSonndPath);
	void InsertItemData();

	void AddTray();
	
	HWINDOW &GetHWindow()
	{
		return m_hWindow;
	}
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
	CString m_strCallingInfo;

private:
	int m_nItem;
	HELE    m_hTree;
	HIMAGE  m_hVip;
	HIMAGE  m_hQZone;
	HIMAGE  m_hAvatar;
	HIMAGE  m_hAvatarLarge;
	HTEMP  m_pTemplate_group;
	HTEMP  m_pTemplate_sel;
	HXCGUI m_hAdapterTree;
	CTrayEx m_TrayEx;	

	
	CString	m_name;
	//ip name time  list��ʾʹ��

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
	CIncommingWindow *m_pIncommingDlg;

};


