#include <string>
using namespace std;
//************************************
// Method:    SplitIpAndName
// FullName:  SplitIpAndName
// Access:    public 
// Returns:   bool
// Description: 192.168.1.1;china; 类型字符串进行拆分
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
// Description: 获取配置文件路径
//************************************
CString GetMoudleConfigFilePath();

//************************************
// Method:    GetCurrentTimeL
// FullName:  GetCurrentTimeL
// Access:    public 
// Returns:   int
// Description: 获取当前时间,HHMMSSMMM形式显示
//************************************
int GetCurrentTimeL();
