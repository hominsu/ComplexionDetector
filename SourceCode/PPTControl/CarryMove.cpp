#include "CarryMove.h"


CarryMove::CarryMove()
{
	//std::cout << "�ȴ�ָ��" << std::endl;
}

CarryMove::~CarryMove()
{

}

CarryMove::CarryMove(int signal)
{
	this->CarrySelect(signal);
}

void CarryMove::CarrySelect(int signal)
{
	switch (signal) {
	case DIANJI://��ppt
		//std::cout << "1" << std::endl;
		Start();
		break;
	case PINGYI://�л�����һҳ
		//std::cout << "2" << std::endl;
		changeNext();
		break;
	case SUOFANG://�л�����һҳ
		//std::cout << "3" << std::endl;
		changeBefore();
		break;
	case ZHUAQU://�Ŵ�
		//std::cout << "4" << std::endl;
		enlarge();
		break;
	case XUANZHUAN://�˳�
		//std::cout << "5" << std::endl;
		quit();
		break;
	default:
		break;
	}
}

void CarryMove::setNum(int num)
{
	keybd_event(num, 0, 0, 0);//ִ�м�λ����
	keybd_event(num, 0, KEYEVENTF_KEYUP, 0);//ִ�м�λ������������������������һ��ģ�����
}

void CarryMove::setNum2(int num1, int num2)
{
	keybd_event(num1, 0, 0, 0);
	keybd_event(num2, 0, 0, 0);
	keybd_event(num2, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(num1, 0, KEYEVENTF_KEYUP, 0);
}
//����ppt
void CarryMove::Start()
{
	//MouseLClick();
	setNum(116);//F5ģ��
}

void CarryMove::changeNext()
{
	setNum(78);//N��ģ��
}

void CarryMove::changeBefore()
{
	setNum(80);//P��ģ��
}

void CarryMove::enlarge()
{
	MouseRClick();
	Sleep(50);//�˴�����һ���ĵȴ�ʱ����Ϊ����һ����������ִ�У���������ûᵼ���һ�����ִ�е��Ǵ˴���U������ִ�У���quitͬ��
	setNum(85);//U��ģ��
	Sleep(3000);//�˴�����Ϊ���Ŵ��Ч�����ã�ʹ��ʱ������ӣ����������������
	MouseLClick();//�ò���Ϊ�˳��Ŵ�Ч����Ŀǰִ����ò����ſ���ִ����������(��������һ�)

}

void CarryMove::quit()
{
	MouseRClick();
	Sleep(50);
	setNum(69);//E��ģ��
}
