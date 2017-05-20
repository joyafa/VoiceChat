#pragma once

#include "hardware\UsbDevice.h"
#include "MCIPlayMusic\MCIPlayMusic.h"
#include "TrayEx.h"
#include "ServiceInterface.h"
#include "IncommingWindow.h"

enum CallStatus
{
	INITIAL = 0, //初始状态     
	DIALING = 1, //呼叫ing
	ACCEPTING, //接听ing: 被叫方,来电铃声提醒,未接听,可以挂断(Reject),可以接听
	ONLINE, //通话中,可以挂断(Hangup),或者等对方挂断(收到对方什么消息)
	BUSYING         //呼叫超时,忙音
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
	char szClientIpAddress[64]; //IP地址
	char szClientName[512];     //机器名称
	char szBeginTime;           //通话开始时间 hh:mm:ss'ms
	int  nLastTime;             //通话持续时长
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
	//usb(响应处理)
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
	//被叫事件: 0:接听事件;1:挂断事件
	HANDLE m_hAcceptCallEvents[2];
	//TODO:简单处理,1 客户端都是通过开关呼叫方,做呼叫相关的操作状态;
	//              2 前台服务端,被叫方,只能做接听电话操作,考虑前台呼叫客户端的情况--> 因为有界面,采取界面按钮方式,而非事件方式
	//记录当前状态: 主叫 还是 接听
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
	//ip name time  list显示使用

	//硬件PID,VID
	DWORD m_dwPID;
	DWORD m_dwVID;
	CString m_strServiceIP;
	//拨号铃声
	CString m_strPathDialingBell;
	//来电铃声
	CString m_strPathIncommingBell;
	//忙音
	CString m_strPathBusyBell;

	CMCIPlayMusic m_mciMusic;
	CUsbDevice *m_pUsbDevice;
	CIncommingWindow *m_pIncommingDlg;

};


