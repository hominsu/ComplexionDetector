#include "keyboard.h"


keyboard::keyboard(std::string dir)
{
	this->cmd = new CarryMove();
	LPCWSTR out = stringtoLPCWSTR(dir);
	int ret;
	ret = (int)ShellExecute(NULL, L"open",out, NULL, NULL, SW_NORMAL);//打开指定文件，将调用默认处理的程序打开
}

keyboard::~keyboard()
{
	if (cmd != NULL)
	{
		delete cmd;
	}
	cmd = NULL;
}

void keyboard::BoardCarry(int sign)
{
	cmd->CarrySelect(sign);
}

void keyboard::tail()
{

}

LPCWSTR keyboard::stringtoLPCWSTR(std::string orig)//将string类型转换为LPCWSTR，以供execute使用
{
		size_t origsize = orig.length() + 1;
		const size_t newsize = 100;
		size_t convertedChars = 0;
		wchar_t* wcstring = new wchar_t[sizeof(wchar_t) * (orig.length() - 1)];
		mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
		return wcstring;
}