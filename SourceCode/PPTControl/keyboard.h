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
	~keyboard();
	void BoardCarry(int sign);//ִ�в�����ѡ����

private:
	int x, y;
	CarryMove* cmd;
	void tail();//�����壬�պ���
	LPCWSTR stringtoLPCWSTR(std::string org);//·��ת��Ϊexecute��������
};

