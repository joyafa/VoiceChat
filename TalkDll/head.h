#ifndef _WAVE_H_
#define _WAVE_H_

#pragma pack(push, 1)
struct Frame
{
	unsigned __int32	iIndex;
	char cb[12];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TalkFrame
{
	char cFlag[16];
	unsigned __int32 iCom;
	unsigned __int32 iLen;
	char chClientInfo[512];//ip 和 name
};
#pragma pack(pop)

#define TALK_REC_PORT 19346

//通话端口
#define TALK_COM_PORT 19345
//接收到新来的通话连接
#define TC_NORMAL_TALK	0x1000
#define TC_AGREE_TALK	0x1001
#define TC_DISAGREE_TALK	0x1002
#endif