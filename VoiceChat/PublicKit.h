#include <string>
using namespace std;
//************************************
// Method:    SplitIpAndName
// FullName:  SplitIpAndName
// Access:    public 
// Returns:   bool
// Description: 192.168.1.1;china; �����ַ������в��
// Parameter: const string & strIpName
// Parameter: string & ip
// Parameter: string & name
// Parameter: char chSeparator
//************************************
bool SplitIpAndName(const string &strIpName, string &ip, string &name, char chSeparator = '\n');

//************************************
// Method:    GetMoudleConfigFilePath
// FullName:  CServerWindow::GetMoudleConfigFilePath
// Access:    public 
// Returns:   CString
// Description: ��ȡ�����ļ�·��
//************************************
CString GetMoudleConfigFilePath();

//************************************
// Method:    GetCurrentTimeL
// FullName:  GetCurrentTimeL
// Access:    public 
// Returns:   int
// Description: ��ȡ��ǰʱ��,HHMMSSMMM��ʽ��ʾ
//************************************
int GetCurrentTimeL();
