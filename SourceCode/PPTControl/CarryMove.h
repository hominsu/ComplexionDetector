#pragma once
#include <iostream>
#include  "windows.h "                      
#include  "shellapi.h "
#include  "mouse.h"


#define dian_ji 1
#define ping_yi 2
#define suo_fang 3
#define zhua_qu 4
#define xuan_zhuan 5


class CarryMove:public mouse
{
public:
	CarryMove();
	CarryMove(int signal);
	void CarrySelect(int signal);//�൱�ڲ�����ѡ�񲿷֣���switchʵ��,�ڹ��캯���б�����

	~CarryMove();
private:
	void setNum(int num);//���ڷ�װģ����̵Ĳ�����num���Ƕ�Ӧ���̵�һ����ֵ����ֵ��windows�ٷ��ĵ�����
	void setNum2(int num1, int num2);//������ϼ���Ŀǰ��û�õ���ϼ�����ʱ����
	void Start();//��ʼ���ţ�Ŀǰ��֧�ִ���ҳ��ʼ������������
	void changeNext();//�л�����һҳ
	void changeBefore();//�л�����һҳ
	void enlarge();//�Ե�ǰҳ����зŴ�
	void quit();//�˳�����

};

