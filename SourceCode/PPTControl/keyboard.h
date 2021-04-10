#pragma once
#include "windows.h"
#include "CarryMove.h"
#include "mouse.h"
#include <iostream>
#include <string>


class keyboard:public mouse
{
public:
	keyboard(std::string dir);
	void BoardCarry(int sign);//执行操作的选择函数
	~keyboard();
	CarryMove* cmd;
private:
	int x, y;
	
	void tail();//无意义，空函数
	LPCWSTR stringtoLPCWSTR(std::string org);//路径转换为execute可用数据
};

