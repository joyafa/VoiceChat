#include "stdafx.h"
#include "PublicKit.h"

//192.168.1.1;china;
bool SplitIpAndName(const string &strIpName, string &ip, string &name, char chSeparator )
{
	size_t pos = strIpName.find_first_of(chSeparator);
	if (pos != std::string::npos)
	{
		ip = strIpName.substr(0, pos);
		name = strIpName.substr(pos + 1);
	}

	return true;
}


CString GetMoudleConfigFilePath()
{
	TCHAR _szPath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, _szPath, MAX_PATH);
	(_tcsrchr(_szPath, _T('\\')))[1] = 0;//删除文件名，只获得路径 字串  
	string strPath;
	for (int n = 0; _szPath[n]; n++)
	{
		if (_szPath[n] != _T('\\'))
		{
			strPath += _szPath[n];
		}
		else
		{
			strPath += _T("\\\\");
		}
	}

	strcat(_szPath, "ini");

	return _szPath;
}

int GetCurrentTimeL()
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	return st.wHour * 10000000 + st.wMinute * 100000 + st.wSecond * 1000 + st.wMilliseconds;
}