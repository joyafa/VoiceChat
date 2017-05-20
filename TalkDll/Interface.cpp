// Interface.cpp: implementation of the CInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Interface.h"
#include "head.h"
#include "MyWaveIn.h"	// Added by ClassView
#include "WaveOut.h"	// Added by ClassView
#include "ListenSocket.h"	// Added by ClassView
#include "SendClient.h"
#include "UdpSocket.h"
#include "RecSocket.h"
#include "Log.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

LOG_INIT;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWaveOut *g_pOut;
CCriticalSection g_soLock;

CInterface::CInterface():
m_bIni (FALSE)
{	
	m_bWork = FALSE;
	m_pUdp = new CUdpSocket();
	m_pRec = new CRecSocket();
	
	g_pOut = new CWaveOut();
	m_pIn = new CMyWaveIn(m_pUdp);

	m_sopListen = new CListenSocket(this,m_pIn,m_pUdp);
	
	LOG->SetLogLevel(10);
	//	m_sopSend = new CSendClient(m_pIn,this);
	m_sopSend=0;
	m_CallBackFun=0;
}

CInterface::~CInterface()
{
	if (m_bIni)
	{
		m_sopListen->Close ();
	}
	if (m_bWork)
	{
		End();
	}
	m_pIn->StopRec ();
	m_pUdp->CloseSocket ();
	
	m_pRec->CloseSocket ();
	g_pOut->StopPlay ();
	
	//	delete m_sopSend;
	delete m_sopListen;
	delete m_pIn;
	delete g_pOut;
	delete m_pUdp;
	delete m_pRec;
}

CString CInterface::GetHostIpName()
{
	char chName[128] = {0};
	if (SOCKET_ERROR  == gethostname(chName, sizeof(chName)))
	{
		LOG_ERR << "Get host name failed!";
		return "";
	}
	struct hostent *phost;
	phost = gethostbyname (chName);
	if (phost == NULL)
	{
		LOG_ERR << "gethostbyname error!";
		return "";
	}

	CString loip;
	int i = 0;
	while (phost->h_addr_list[i])
	{
		loip = inet_ntoa (*(struct in_addr *)phost->h_addr_list[i++]);
		break;
	}
	//name\nip ������\nIP
	CString strIpInfo = CString("������:") + chName + "\n" + "IP��ַ:" + loip + "\n";
	LOG_MSG << strIpInfo;

	return strIpInfo;
}

bool CInterface::GetLocalIpAddress(string &strName, string &strIpAddress)
{
	char hostName[128] = {0};
	if (SOCKET_ERROR == gethostname(hostName, sizeof(hostName)))
	{
		LOG_ERR << "Get host name failed!";
		return false;
	}
	strName = hostName;
	struct hostent *phost;
	phost = gethostbyname(hostName);
	if (NULL == phost)
	{
		LOG_ERR << "gethostbyname error!";
		return false;
	}

	int i = 0;
	while (phost->h_addr_list[i])
	{
		strIpAddress = inet_ntoa(*(struct in_addr *)phost->h_addr_list[i++]);
		break;
	}

	return true;
}

BOOL CInterface::Start(const char *ip)
{
	BOOL bRet  = FALSE;
	try
	{
		char name[128];
		int iLen = 128;
		int i = 0;
		CString loip;

		do
		{
			if (m_bWork)
			{
				LOG_MSG << "The talk has working...";
				break;
			}
			//m_sIp = ip;

			if (SOCKET_ERROR  == gethostname(name,iLen))
			{
				LOG_ERR << "Get host name failed!";
				break;
			}
			struct hostent *phost;
			phost = gethostbyname (name);
			if (phost == NULL)
			{
				LOG_ERR << "gethostbyname failed!";
				break;
			}

			i = 0;
			while (phost->h_addr_list[i])
			{
				loip = inet_ntoa (*(struct in_addr *)phost->h_addr_list[i++]);
				if (loip == ip)
					break;
			}

			if(NULL == m_sopSend)
			{
				m_sopSend = new CSendClient(m_pIn,this);
				bRet      = m_sopSend->Create ();
				if (!bRet)
				{
					break;;
				}
			}
			else
			{
				break;
			}

			//�ͻ���TCP��ʽ���ӷ�����
			BOOL nRet = m_sopSend->Connect (ip,TALK_COM_PORT);
			if (GetLastError() != WSAEWOULDBLOCK)
			{
				m_sopSend->Close ();
				break;
			}
			m_pUdp->SetIp (ip);

			m_bWork = TRUE;
			break;
		}while(false);
	}
	catch(...)
	{
		TRACE("Start failed!!!");
	}

	return bRet;
}


BOOL CInterface::End()
{
	try
	{
		//TODO: ������ܶ����еĶ�Ҫ����, ����Ҫ�ж�����ͨ����
		//if (!m_bWork)
		//{
			//����������ص�client,�ܾ�����ʱ�����
		if (m_sopListen)
		{
			m_sopListen->CloseClient();
		}

		//	TRACE("The talk hasn't worked.\n");
		//	return FALSE;
		//}

		if (m_pIn)
		{
			m_pIn->EnableSend (FALSE);
		}

		if(m_sopSend)
		{
			m_sopSend->Close ();
			m_sopSend->m_bConnect = FALSE;
		}

		m_bWork = FALSE;
		
		if(m_sopSend)
		{
			delete m_sopSend;
			m_sopSend = NULL;
		}

		return TRUE;
	}
	catch(...)
	{

	}

	return FALSE;
}

BOOL CInterface::Ini(_CallBackFun __CallBackFun)
{
	try
	{
		m_CallBackFun=__CallBackFun;
		if (m_bIni)
		{
			LOG_ERR << "You have in the talk model.";
			return FALSE;
		}
		//����tcp����
		if (!m_sopListen->Create (TALK_COM_PORT))
		{
			CString str;
			str.Format("m_sopListen create Error:%d \n", WSAGetLastError());
			LOG_ERR << str.GetString();
			goto Exit;
		}
		int timeout = 1000;
		setsockopt(m_sopListen->m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

		//����Socket��ѡ��, ���10048�������Ĳ���,�ر�scoket��˿ڱ��������ͷ�
		BOOL bOptVal = TRUE;
		int bOptLen = sizeof(BOOL);
		m_sopListen->SetSockOpt(SO_REUSEADDR, (void *)&bOptVal, bOptLen, SOL_SOCKET);

		//����
		m_sopListen->Listen ();
		
		//UDP �㲥��ʽ
		if (!m_pRec->Ini ())
		{
			goto Exit1;
		}

		//UDP����
		if (!m_pUdp->Ini ())
		{
			goto Exit2;
		}
		//��ʼ����
		if (!g_pOut->StartPlay ())
		{
			goto Exit3;
		};

		//��ʼ¼��
		if (!m_pIn->StartRec ())
		{
			goto Exit4;
		};
		
		m_bIni = TRUE;

		goto Exit;
Exit4:
		g_pOut->StopPlay ();
Exit3:
		m_pUdp->CloseSocket ();
Exit2:
		m_pRec->CloseSocket ();
Exit1:
		m_sopListen->Close ();
Exit:
		return m_bIni;
	}
	catch(...)
	{
	}
	return FALSE;
}

BOOL CInterface::IncomingCall(CString ip)
{
	if(m_CallBackFun!=0)
	{
		m_CallBackFun(MSG_CallIn,"����...");
	}
	////�Ƿ����
	//if (IDYES ==  MessageBox(NULL,"talk?","talk",MB_YESNO))
	//{
	//	return TRUE;
	//}
	//else
	//{
	//	return FALSE;
	//}
	
	return TRUE;
}

void CInterface::TalkStart(string ip)
{
	if(m_CallBackFun!=0)
	{
		m_CallBackFun(MSG_CallOk,"�Ի�");
	}
}

void CInterface::TalkBeClose()
{
	if(m_CallBackFun!=0)
	{
		m_CallBackFun(MSG_CallClose,"�ر�");
	}
}

void CInterface::BeClose()
{
	try
	{
		g_soLock.Lock ();
		//if (m_bWork)
		{
			m_pIn->EnableSend (FALSE);
			if(NULL != m_sopSend)
			{
				m_sopSend->Close ();
				m_sopSend->m_bConnect = FALSE;
				m_sopSend = NULL;
			}

			m_bWork = FALSE;
			
			TalkBeClose();
		}
		g_soLock.Unlock ();
	}
	catch(...)
	{
	}
}

//���ӷ������ɹ�,������Ϣ,��ʼ����ͨ��
void CInterface::ConnectResult(int nErrorCode)
{
	try
	{
		//���ӷ�����ʧ��
		if (nErrorCode != 0)
		{
			m_bWork = FALSE;
			TalkOnConnect (FALSE);

			return ;
		}

		TalkOnConnect (TRUE);
		
		//���Ϳͻ�����Ϣ
		//TODO: ���Ͽ�ʼͨ��ʱ�� �Լ� �Ҷ�ʱ�� ͨ��ʱ��
		//TODO:list������ʾ��ͼ����  ���Ե� �Ǹ� ͼ�� 
		char buffer[240];
		memset(buffer,0,240);
		struct TalkFrame *frame;
		frame = (struct TalkFrame *)buffer;
		sprintf(frame->cFlag,"NETTALK");
		sprintf(frame->chClientInfo, GetHostIpName());
		frame->iLen = 0;
		frame->iCom = TC_NORMAL_TALK;
		//���Ϳͻ�����Ϣ 
		if(NULL != m_sopSend)
		{
			m_sopSend->Send (buffer,sizeof(struct TalkFrame));
		}
	}
	catch(...)
	{
	}
}

void CInterface::TalkOnConnect(BOOL bRet)
{
	//TODO: bRet falseʱ�� ����ʧ��,�Ҷϴ���

	if(m_CallBackFun!=0)
	{
		if (bRet)
		{
			m_CallBackFun(MSG_CallOut, "���ں���ǰ̨...");
		}
		else 
		{

			m_CallBackFun(MSG_CallClose, "�����ж�");
		}
	}
}