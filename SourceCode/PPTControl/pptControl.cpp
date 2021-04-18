#include "pptControl.h"


pptControl::pptControl(std::string dir,int signal)
{
	setData(dir,signal);
	key = new keyboard(getDir());
	this->isStart = 0;
	if (!this->Sign)
	{
		//std::cout << "�ļ��򿪳ɹ����ȴ�ָ��" << std::endl;
	}
}

pptControl::~pptControl()
{
	if (key != NULL)
	{
		delete key;
	}
	key = NULL;
}

void pptControl::setData(const std::string dir,const int signal)
{
	this->Fdir = dir;
	this->Sign = signal;
}

std::string pptControl::getDir()
{
	std::string tmp = this->Fdir;
	return tmp;
}
int pptControl::getSign()
{
	int n = this->Sign;
	return n;
}
bool pptControl::Run()
{
	key->BoardCarry(getSign());
	if (this->Sign == 1)
	{
		this->isStart = 1;
	}
	return true;
}
void pptControl::update(int signal)
{
	this->Sign = signal;
	if (this->isStart || this->Sign == 1)
	{
		Sleep(1000);
		Run();
	}
	else
	{
		//std::cout << "��ǰ�ļ�δ����" << std::endl;
	}
	
}