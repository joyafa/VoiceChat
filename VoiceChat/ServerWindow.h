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
	// Description: 获取配置文件信息
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
	// Description: 关闭窗口
	// Parameter: BOOL * pbHandled
	//************************************
	int OnCloseWindow(BOOL * pbHandled);

	//usb(响应处理)
	int OnHandlePhone(WPARAM wParam, LPARAM lParam, BOOL *pbHandled);

	bool AcceptCallFrom(const char* pIpAndName);
	//************************************
	// Method:    OnStopMusic
	// FullName:  CServerWindow::OnStopMusic
	// Access:    public 
	// Returns:   int
	// Description: 停止播放声音
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
	// Description: 播放路径的声音文件
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
	// Description: 记录通话信息,方便后续界面显示提示信息时候使用
	// Parameter: int nIndex
	// Parameter: const CallingInfo * info
	//************************************
	bool InsertCallingInfo(int nIndex, const CallingInfo &info)
	{
		//TODO:加锁保护;
		//map: key:下标, value:CallingInfo
		return m_mapCallInfo.insert(pair<int, CallingInfo>(nIndex, info)).second;
	}

	//************************************
	// Method:    GetCallingInfo
	// FullName:  CServerWindow::GetCallingInfo
	// Access:    public 
	// Returns:   CallingInfo &
	// Description: 通过list界面显示的序号,获取信息
	// Parameter: int nIndex
	//************************************
	CallingInfo &GetCallingInfo(int nIndex)
	{
		//TODO:加锁保护;
		//map: key:下标, value:CallingInfo
		map<int, CallingInfo>::iterator it = m_mapCallInfo.find(nIndex);
		if (it == m_mapCallInfo.end())
		{
			throw "Key not found!!!!";
		}

		//返回引用,可以更新数据
		return it->second;
	}


	//************************************
	// Method:    AddTray
	// FullName:  CServerWindow::AddTray
	// Access:    public 
	// Returns:   void
	// Qualifier: 增加系统托盘
	//************************************
	void AddTray();
	
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
	int m_nItem;
	//来电提醒对话框
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

	//记录通话信息
	map<int, CallingInfo>m_mapCallInfo;
};


