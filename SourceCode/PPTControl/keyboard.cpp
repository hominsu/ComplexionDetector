#include "keyboard.h"


keyboard::keyboard(std::string dir)
{
	this->cmd = new CarryMove();
	LPCWSTR out = stringtoLPCWSTR(dir);
	int ret;
	ret = (int)ShellExecute(NULL, L"open",out, NULL, NULL, SW_NORMAL);//��ָ���ļ���������Ĭ�ϴ���ĳ����
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

LPCWSTR keyboard::stringtoLPCWSTR(std::string orig)//��string����ת��ΪLPCWSTR���Թ�executeʹ��
{
		size_t origsize = orig.length() + 1;
		const size_t newsize = 100;
		size_t convertedChars = 0;
		wchar_t* wcstring = new wchar_t[sizeof(wchar_t) * (orig.length() - 1)];
		mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
		return wcstring;
}