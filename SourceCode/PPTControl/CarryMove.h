#pragma once
#include <iostream>
#include  "windows.h "                      
#include  "shellapi.h "
#include  "mouse.h"


#define DIANJI 1
#define PINGYI 2
#define SUOFANG 3
#define ZHUAQU 4
#define XUANZHUAN 5


class CarryMove:public mouse
{
public:
	CarryMove();
	~CarryMove();
	CarryMove(int signal);
	void CarrySelect(int signal);//�൱�ڲ�����ѡ�񲿷֣���switchʵ��,�ڹ��캯���б�����

private:
	void setNum(int num);//���ڷ�װģ����̵Ĳ�����num���Ƕ�Ӧ���̵�һ����ֵ����ֵ��windows�ٷ��ĵ�����
	void setNum2(int num1, int num2);//������ϼ���Ŀǰ��û�õ���ϼ�����ʱ����
	void Start();//��ʼ���ţ�Ŀǰ��֧�ִ���ҳ��ʼ������������
	void changeNext();//�л�����һҳ
	void changeBefore();//�л�����һҳ
	void enlarge();//�Ե�ǰҳ����зŴ�
	void quit();//�˳�����

};

