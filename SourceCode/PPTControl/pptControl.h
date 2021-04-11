#pragma once
#include <iostream>
#include <string>
#include "CarryMove.h"
#include "mouse.h"
#include "keyboard.h"


class pptControl
{
public:
	pptControl(std::string dir,int signal);
	~pptControl();
	void setData(const std::string dir,const int signal);//���ö���Ĳ�����ֵ
	void update(int signal);//����ָ��,�ڲ����ӳ�1s�Ĳ���
	std::string getDir();//���·������this->Fdirһ����˼
	int getSign();//����ź�
	bool Run();//ִ��ָ���update������ִ��

	int isStart;//�ж��ļ��Ƿ񱻴򿪣�����Ĵ���ָ�ں�����ִ�д��ļ��Ĳ���������
				//��ʵ�ļ������������򿪣�isStartΪ��ʱ�����粥�ŵȶ����ſ���ִ�У�����
				//����ִ�иñ�����keyboard��������ʱ���ڹ��캯������Ϊ1

private:
	std::string Fdir;//·��
	int Sign;//�ź�
	keyboard* key;//���տ��Ʋ�������ʱ����
};

