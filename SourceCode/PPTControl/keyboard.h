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
	void BoardCarry(int sign);//ִ�в�����ѡ����
	~keyboard();
	CarryMove* cmd;
private:
	int x, y;
	
	void tail();//�����壬�պ���
	LPCWSTR stringtoLPCWSTR(std::string org);//·��ת��Ϊexecute��������
};

