#include "stdafx.h"
#include "ServerWindow.h"
#include <mmsystem.h>
#include "PublicKit.h"

#define  XC_LNAME    L"name"
#define  XC_LNAME1   L"name"
#define  XC_LNAME2   L"name2"
#define  XC_LNAME3   L"name3"
#define  XC_LNAME4   L"name4"
#define  XC_LNAME5   L"name5"
#define  XC_LNAME6   L"name6"

#define ID_Window_Btn_CloseWindow 100
#define WM_STOPMUSIC WM_USER + 2

CServerWindow::CServerWindow()
	: m_nItem(0)
	, m_pTemplate_group(NULL)
	, m_hWindow(NULL)
	, m_hTree(NULL)
	, m_hVip(NULL)
	, m_hQZone(NULL)
	, m_hAvatar(NULL)
	, m_hAvatarLarge(NULL)
	, m_pTemplate_sel(NULL)
	, m_hAdapterTree(NULL)
	, m_callStatus(INITIAL)
	, m_pIncomingDlg(NULL)
	, m_pUsbDevice(NULL)
{
}
void CServerWindow::Release()
{
	if (m_pUsbDevice)delete m_pUsbDevice;
	if (m_pTemplate_group) XTemp_Destroy(m_pTemplate_group);
}


void CServerWindow::GetConfigInfo()
{
	CString strConfigFilePath = GetMoudleConfigFilePath();

	//硬件PID,VID
	m_dwPID = GetPrivateProfileInt("呼叫", "PID", 0x001B, strConfigFilePath);
	m_dwVID = GetPrivateProfileInt("呼叫", "VID", 0x258A, strConfigFilePath);
	char chBuffer[128] = { 0 };
	GetPrivateProfileString("呼叫", "ServerIP", "", chBuffer, sizeof(chBuffer), strConfigFilePath);
	m_strServiceIP = chBuffer;

	//拨号铃声
	memset(chBuffer, 0, sizeof(chBuffer));
	GetPrivateProfileString("呼叫", "拨号铃声", "", chBuffer, sizeof(chBuffer), strConfigFilePath);
	m_strPathDialingBell = chBuffer;
	//来电铃声
	memset(chBuffer, 0, sizeof(chBuffer));
	GetPrivateProfileString("呼叫", "来电铃声", "", chBuffer, sizeof(chBuffer), strConfigFilePath);
	m_strPathIncommingBell = chBuffer;
	//忙音
	memset(chBuffer, 0, sizeof(chBuffer));
	GetPrivateProfileString("呼叫", "忙音铃声", "", chBuffer, sizeof(chBuffer), strConfigFilePath);
	m_strPathBusyBell = chBuffer;
}

void CServerWindow::Create()
{
	GetConfigInfo();
	//加载资源文件
	XC_LoadResource(L"res\\resource.res", L"res");
	//加载布局文件
	HXCGUI hXCGUI = XC_LoadLayout(L"res\\layout.xml");
	m_hWindow = (HWINDOW)hXCGUI;
	m_hTree = (HELE)XC_GetObjectByID(m_hWindow, 101);
	XTree_EnableConnectLine(m_hTree, FALSE, FALSE);
	XSView_ShowSBarH(m_hTree, FALSE);
	XTree_SetIndentation(m_hTree, 0);
	XTree_SetItemHeightDefault(m_hTree, 28, 54);
	XTree_SetItemTemplateXML(m_hTree, L"res\\Tree_Item_friend.xml");
	XTree_SetItemTemplateXMLSel(m_hTree, L"res\\Tree_Item_friend_sel.xml");
	m_hAvatar = XImage_LoadFile(L"res\\image\\header\\avatar.png");
	m_hAvatarLarge = XImage_LoadFile(L"res\\image\\header\\avatar_large.png");
	m_hAdapterTree = XAdTree_Create();
	XTree_BindAdapter(m_hTree, m_hAdapterTree);
	XAdTree_AddColumn(m_hAdapterTree, XC_LNAME); //昵称 
	XAdTree_AddColumn(m_hAdapterTree, XC_LNAME2); //IP 
	XAdTree_AddColumn(m_hAdapterTree, XC_LNAME3); //小头像
	XAdTree_AddColumn(m_hAdapterTree, XC_LNAME4); //大头像

	XEle_RegEventCPP(m_hTree, XE_TREE_SELECT, &CServerWindow::OnTreeSelect);
	XEle_RegEventCPP(m_hTree, XE_TREE_EXPAND, &CServerWindow::OnTreeExpand);

	XEle_RegEventCPP(m_hTree, XE_TREE_TEMP_CREATE, &CServerWindow::OnTemplateCreate);
	XEle_RegEventCPP(m_hTree, XE_TREE_TEMP_DESTROY, &CServerWindow::OnTemplateDestroy);
	XEle_RegEventCPP(m_hTree, XE_TREE_TEMP_ADJUST_COORDINATE, &CServerWindow::OnTemplateAdjustCoordinate);

	HELE hBtnClose = (HELE)XC_GetObjectByID(m_hWindow, ID_Window_Btn_CloseWindow);
	XEle_RegEventCPP(hBtnClose, XE_BNCLICK, &CServerWindow::OnCloseWindow);
	//注册消息响应函数
	XWnd_RegEventCPP(m_hWindow, WM_PHONE,     &CServerWindow::OnHandlePhone);
	XWnd_RegEventCPP(m_hWindow, MM_MCINOTIFY, &CServerWindow::OnMCINotify);
	XWnd_RegEventCPP(m_hWindow, WM_STOPMUSIC, &CServerWindow::OnStopMusic);

    m_TrayEx.Create(m_hWindow);

	XWnd_AdjustLayout(m_hWindow);
	XWnd_ShowWindow(m_hWindow, SW_SHOW);

	//创建来电对话框,暂时不显示
	m_pIncomingDlg = new CIncomingWindow(this);
	m_pIncomingDlg->Create(hXCGUI);

	//被叫事件
	m_hAcceptCallEvents[0] = CreateEventA(NULL, TRUE, FALSE, NULL);//接听事件
	m_hAcceptCallEvents[1] = CreateEventA(NULL, TRUE, FALSE, NULL);//挂断事件

	int ServiceCallBack(int type, const char *pInfo);
	m_talk.Ini(ServiceCallBack);
	//TODO:界面上显示一下本机的IP 和 名字
	/*string strName, strIpAddress;
	bool bRet = m_talk.GetLocalIpAddress(strName, strIpAddress);
	if (bRet)
	{
		SetDlgItemText(IDC_STATIC_IPADDRESS, strIpAddress.c_str());
		SetDlgItemText(IDC_STATIC_IP, strIpAddress.c_str());
		SetDlgItemText(IDC_STATIC_NAME, strName.c_str());
	}
*/
	//初始化硬件	
	m_pUsbDevice = new CUsbDevice(0x258a, 0x001b);
	if (NULL == m_pUsbDevice)
	{
		return;
	}

	m_pUsbDevice->SetOwner(XWnd_GetHWND(m_hWindow));
	m_pUsbDevice->ConnectDevice();
	m_pUsbDevice->StartMonitor();
	XWnd_RegEventCPP(m_hWindow, WM_TIMER, &CServerWindow::OnWndTimer);

	XWnd_SetTimer(m_hWindow, 990, 5000);
}

int CServerWindow::OnWndTimer(UINT nIDEvent, BOOL *pbHandled)
{
	return 0;
	if (990 == nIDEvent)
	{
		XWnd_ShowWindow(m_pIncomingDlg->m_hWindow, SW_NORMAL);//显示窗口

		CRect rect;
		XWnd_GetClientRect(m_hWindow, &rect);
		CRect rectIncomming;
		XWnd_GetClientRect(m_pIncomingDlg->m_hWindow, &rectIncomming);
		rectIncomming.left  = rect.right;
		rectIncomming.right = rectIncomming.left + rectIncomming.Width();
		rectIncomming.top   = rect.top + rect.Height() / 2 - rectIncomming.Height() / 2;
		rectIncomming.bottom = rectIncomming.top + rectIncomming.Height();
		XWnd_RedrawWndRect(m_pIncomingDlg->m_hWindow, rectIncomming, TRUE);
		*pbHandled = TRUE;
	}

	return 0;
}


int CServerWindow::OnTreeSelect(int nItem, BOOL *pbHandled)
{
	*pbHandled = TRUE;
	return 0;
}
int CServerWindow::OnTreeExpand(int id, BOOL bExpand, BOOL *pbHandled)
{
	*pbHandled = TRUE;
	return 0;
}
int  CServerWindow::OnTemplateCreate(tree_item_i *pItem, BOOL *pbHandled)
{
	if (XC_ID_ERROR != XTree_GetFirstChildItem(m_hTree, pItem->nID))
	{
		if (m_pTemplate_group)
			pItem->hTemp = m_pTemplate_group;
	}
	*pbHandled = TRUE;
	return 0;
}
int  CServerWindow::OnTemplateDestroy(tree_item_i *pItem, BOOL *pbHandled)
{
	*pbHandled = TRUE;
	return 0;
}
int  CServerWindow::OnTemplateAdjustCoordinate(tree_item_i *pItem, BOOL *pbHandled)
{
	*pbHandled = TRUE;
	return 0;
}

int CServerWindow::OnCloseWindow(BOOL *pbHandled)
{
	ShowWindow(XWnd_GetHWND(m_hWindow), SW_HIDE);
	*pbHandled = TRUE;
	return 0;
}


int CServerWindow::OnMCINotify(WPARAM wParam, LPARAM lParam, BOOL *pbHandled)
{
	if (wParam == MCI_NOTIFY_SUCCESSFUL)
	{
		//呼叫
		if (DIALING == m_callStatus)
		{
			PlaySound(m_strPathDialingBell);
		}
		//接听
		else if (ACCEPTING == m_callStatus)
		{
			PlaySound(m_strPathIncommingBell);
		}
		//忙音
		else
		{
			PlaySound(m_strPathBusyBell);
		}
	}
	return 0;
}

void CServerWindow::PlaySound(const CString &strSonndPath)
{
	//先停掉原有的声音
	m_mciMusic.stop();

	DWORD dwResult = m_mciMusic.play(XWnd_GetHWND(m_hWindow), strSonndPath);
	if (dwResult != 0)
	{
		//beatLog_Error(("CServiceTalkDlg", __FUNCDNAME__, "Play sound failed: %s", m_mciMusic.getErrorMsg(dwResult)));
	}
}


int CServerWindow::OnStopMusic(WPARAM wParam, LPARAM lParam, BOOL *pbHandled)
{
	m_mciMusic.stop();

	return 1;
}


int CServerWindow::OnHandlePhone(WPARAM wParam, LPARAM lParam, BOOL *pbHandled)
{
	HardwareEventType event = (HardwareEventType)wParam;

	switch (event)
	{
		//接听
	case LEFT_KEY:
		//呼叫模式, 因为收到来电,状态会变成ACCEPTING
		if (INITIAL == m_callStatus)
		{
			//OnBnClickedBtnCall();

		}
		else if (ACCEPTING == m_callStatus)
		{
			SetEvent(m_hAcceptCallEvents[0]);
		}
		break;
		//挂断
	case RIGHT_KEY:
		//主叫
		if (ONLINE == m_callStatus)
		{
			//挂断 hangup
			m_talk.End();
			m_callStatus = INITIAL;
			CallingInfo &info = GetCallingInfo(m_nItem - 1);
			m_pIncomingDlg->UpdataWindow(info);
		}
		else if (ACCEPTING == m_callStatus)//来电响铃...
		{
			//reject
			SetEvent(m_hAcceptCallEvents[1]);
			//m_pCallCommingDialog->ShowWindow(SW_HIDE);
			m_talk.End();
			m_callStatus = INITIAL;
		}

		break;
	}

	return 1;
}

//TODO:接听过程中,不会有该线程的存在,也就不会有等待事件的情况,只要通过当前的状态,检查当前出于什么状态;
// 1. 通话中,则发送挂断指令;hangup
// 2. 响铃中,这reject;

UINT __stdcall AcceptCallFunc(void *pvoid)
{
	_tagCallFrom *pFrom = (_tagCallFrom *)pvoid;
	CServerWindow *pDlag = (CServerWindow *)pFrom->pWindow;
	if (NULL == pDlag) return 0;

	//电话60s不接听，认为超时，关闭铃声
	//TODO：是否需要通过配置文件设置
	DWORD dwRet = WaitForMultipleObjects(2, pDlag->m_hAcceptCallEvents, FALSE, 60 * 1000);
	if (dwRet == WAIT_OBJECT_0)//接听事件
	{
		//直接停止不行,需要发送消息
		PostMessage(XWnd_GetHWND(pDlag->m_hWindow), WM_STOPMUSIC, NULL, NULL);
		//重置为无信号状态
		//调用 SetEvent设置有信号之后,需要调用reset设置为无信号
		ResetEvent(pDlag->m_hAcceptCallEvents[0]);

		pDlag->m_callStatus = ONLINE;
		pFrom->bAcceptCall  = true;
	}
	else  //挂断事件,超时停止 或 异常 都停止
	{
		PostMessage(XWnd_GetHWND(pDlag->m_hWindow), WM_STOPMUSIC, NULL, NULL);
		//1. 超时或者是拒绝进接听
		pFrom->bAcceptCall = false;
		//设置为无信号
		ResetEvent(pDlag->m_hAcceptCallEvents[1]);

		pDlag->m_callStatus = INITIAL;
	}

	return 1;
}


int   MessageLoop(
	HANDLE*   lphObjects,  //   handles   that   need   to   be   waited   on   
	int       cObjects     //   number   of   handles   to   wait   on   
)
{
	//   The   message   loop   lasts   until   we   get   a   WM_QUIT   message, 
	//   upon   which   we   shall   return   from   the   function. 
	while (TRUE)
	{
		//   block-local   variable   
		DWORD   result;
		MSG   msg;
		//   Read   all   of   the   messages   in   this   next   loop,   
		//   removing   each   message   as   we   read   it. 
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//   If   it 's   a   quit   message,   we 're   out   of   here. 
			if (msg.message == WM_QUIT)
				return   -1;
			//   Otherwise,   dispatch   the   message. 
			DispatchMessage(&msg);
		}   //   End   of   PeekMessage   while   loop. 
			//   Wait   for   any   message   sent   or   posted   to   this   queue   
			//   or   for   one   of   the   passed   handles   be   set   to   signaled. 
		result = MsgWaitForMultipleObjects(cObjects, lphObjects,
			FALSE, INFINITE, QS_ALLINPUT);
		//   The   result   tells   us   the   type   of   event   we   have. 
		if (result == (WAIT_OBJECT_0 + cObjects))
		{
			//   New   messages   have   arrived.   
			//   Continue   to   the   top   of   the   always   while   loop   to   
			//   dispatch   them   and   resume   waiting. 
			continue;
		}
		else
		{
			//   One   of   the   handles   became   signaled.   
			return result - WAIT_OBJECT_0;
		}   //   End   of   else   clause. 
	}

	return -1;
}

extern CServerWindow g_hWindow;
int ServiceCallBack(int type, const char *pInfo)
{
	switch (type)
	{
	case MSG_ADDCLIENTUSER:
		//名字;IP
	{
		CallingInfo info;
		info.nBeginTime = GetCurrentTimeL();
		string strIp, strName;
		if (pInfo)
		{
			SplitIpAndName(pInfo, strIp, strName);
			sprintf(info.szClientIpAddress, strIp.c_str());
			sprintf(info.szClientName, strName.c_str());
		}
		//list显示新来电信息
		int nIndex = g_hWindow.InsertItemData(strName.c_str(), strIp.c_str());
		//向量中缓存来电信息,方便在tooltip以及通话结束时记录通话时长
		//多线程
		if (-1 != nIndex)
		{
			//map: key:下标, value:CallingInfo
			g_hWindow.InsertCallingInfo(nIndex, info);
		}
	}
	break;
	case MSG_CallIn:
		//来电,同步等待完成,传入的是IP name信息
		return g_hWindow.AcceptCallFrom(pInfo);
	case MSG_CallOk:
	{
		//TODO:
		/*CString str = "通话中...";
		str = str + g_hWindow.m_name;
		g_hWindow.SetWindowText(str);*/
		XWnd_KillXCTimer(g_hWindow.m_hWindow, 990);
	}
	break;
	case MSG_CallClose:
		//客户端主动挂掉电话消息,还未接听
		if (g_hWindow.m_callStatus == ACCEPTING)
		{
			SetEvent(g_hWindow.m_hAcceptCallEvents[1]);
		}
		try
		{
			CallingInfo &info = g_hWindow.GetCallingInfo(g_hWindow.m_nItem - 1);
			info.nEndTime = GetCurrentTimeL();
			g_hWindow.m_pIncomingDlg->UpdataWindow(info);
		}
		catch (string &e)
		{
			//TODO:
			//LOG_INFO
		}
		//TODO: 会多显示一次断开时间
		//CString strCallingTime;

		//已接听,挂断
		//if (g_hWindow.m_callStatus == ONLINE)
		//{
			
		//}
		//TODO:指定位置显示一下 执行状态的变化
		//g_hWindow.SetWindowText("呼叫:服务端");
		break;
	}

	return 0;
}



bool CServerWindow::AcceptCallFrom(const char* pIpAndName)
{
	//接听状态
	m_callStatus = ACCEPTING;

	bool bAcceptCall(false);

	CallingInfo info;
	string strIp, strName;
	if (pIpAndName)
	{
		SplitIpAndName(pIpAndName, strIp, strName);
		sprintf(info.szClientIpAddress, strIp.c_str());
		sprintf(info.szClientName, strName.c_str());
	}

	if (m_pIncomingDlg)
	{
		//显示来电界面:有来电,只显示IP,名字,接听挂断按钮
		m_pIncomingDlg->UpdataWindow(info, true);
	}

	//创建播放声音线程
	_tagCallFrom *pFrom = new _tagCallFrom;
	pFrom->strIpName    = pIpAndName;
	pFrom->pWindow      = this;
	pFrom->bAcceptCall  = false;

	//状态1: 新来电，可以 接听 or 挂断
	//状态2: 通话中,可以挂断;
	HANDLE hPlayMusic = (HANDLE)_beginthreadex(NULL, 0, AcceptCallFunc, pFrom, 0, 0);
	PlaySound(m_strPathIncommingBell);
	//简单处理,因为是永远不超时的等待,所有必须有消息或退出消息才会返回
	MessageLoop(&hPlayMusic, 1);
	CloseHandle(hPlayMusic);
	//提示框 showWindow(SW_HIDE);
	bAcceptCall = pFrom->bAcceptCall;
	delete pFrom;

	return bAcceptCall;
}


int CServerWindow::InsertItemData(const char *pComputerName, const char *pIp)
{
	if (NULL == pComputerName || NULL == pIp)
	{
		return -1;
	}

	wchar_t  buf[256] = { 0 };
	wsprintfW(buf, L"机器名:%s", pComputerName);
	m_nItem = XAdTree_InsertItemText(m_hAdapterTree, buf, 0, XC_ID_LAST);
	wsprintfW(buf, L"IP:%s", pIp);
	XAdTree_SetItemTextEx(m_hAdapterTree, m_nItem, XC_LNAME2, L"IP");
	XAdTree_SetItemImageEx(m_hAdapterTree, m_nItem, XC_LNAME3, m_hAvatar);
	XAdTree_SetItemImageEx(m_hAdapterTree, m_nItem, XC_LNAME4, m_hAvatarLarge);
	//HELE hItem = (HELE)XTree_GetTemplateObject((HELE)m_hTree, m_nItem, 2);

	//XEle_SetToolTip(hItem, L"通话时长:10:00:23\nIP:127.0.0.1\nJoyafa");
	//TODO:通过m_nItem进行信息的索引,存到map中,然后根据数组下标查找对应,因为一个时刻只会有一个账号在线,所以可以这么简单处理

	return m_nItem++;
}

