#include "pptControl.h"
#include <thread>
pptControl::pptControl(std::string dir,int signal)
{
	
	setData(dir,signal);
	keyboard* test = new keyboard(getDir());
	this->isStart = 0;
	if (!this->Sign)
	{
		std::cout << "文件打开成功，等待指令" << std::endl;
	}
	
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
		
		test->BoardCarry(getSign());
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
		std::cout << "当前文件未开启" << std::endl;
	}
	
}