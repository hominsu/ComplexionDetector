#include "CarryMove.h"
CarryMove::CarryMove()
{
	std::cout << "�ȴ�ָ��" << std::endl;
}
CarryMove::CarryMove(int signal)
{
	this->CarrySelect(signal);
}
void CarryMove::CarrySelect(int signal)
{
	switch (signal) {
	case dian_ji://��ppt
		//cout << "1" << endl;
		Start();
		break;
	case ping_yi://�л�����һҳ
		//cout << "2" << endl;
		changeNext();
		break;
	case suo_fang://�л�����һҳ
		//cout << "3" << endl;
		changeBefore();
		break;
	case zhua_qu://�Ŵ�
		//cout << "4" << endl;
		enlarge();
		break;
	case xuan_zhuan://�˳�
		//cout << "5" << endl;
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
CarryMove::~CarryMove()
{

}