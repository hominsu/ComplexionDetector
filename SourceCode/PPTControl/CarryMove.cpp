#include "CarryMove.h"


CarryMove::CarryMove()
{
	//std::cout << "等待指令" << std::endl;
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
	case DIANJI://打开ppt
		//std::cout << "1" << std::endl;
		Start();
		break;
	case PINGYI://切换到下一页
		//std::cout << "2" << std::endl;
		changeNext();
		break;
	case SUOFANG://切换到上一页
		//std::cout << "3" << std::endl;
		changeBefore();
		break;
	case ZHUAQU://放大
		//std::cout << "4" << std::endl;
		enlarge();
		break;
	case XUANZHUAN://退出
		//std::cout << "5" << std::endl;
		quit();
		break;
	default:
		break;
	}
}

void CarryMove::setNum(int num)
{
	keybd_event(num, 0, 0, 0);//执行键位按下
	keybd_event(num, 0, KEYEVENTF_KEYUP, 0);//执行键位起来，两个动作连起来构成一次模拟键盘
}

void CarryMove::setNum2(int num1, int num2)
{
	keybd_event(num1, 0, 0, 0);
	keybd_event(num2, 0, 0, 0);
	keybd_event(num2, 0, KEYEVENTF_KEYUP, 0);
	keybd_event(num1, 0, KEYEVENTF_KEYUP, 0);
}
//播放ppt
void CarryMove::Start()
{
	//MouseLClick();
	setNum(116);//F5模拟
}

void CarryMove::changeNext()
{
	setNum(78);//N键模拟
}

void CarryMove::changeBefore()
{
	setNum(80);//P键模拟
}

void CarryMove::enlarge()
{
	MouseRClick();
	Sleep(50);//此处设置一定的等待时间是为了下一步操作正常执行，如果不设置会导致右击操作执行但是此处的U键不能执行，下quit同理
	setNum(85);//U键模拟
	Sleep(3000);//此处设置为看放大的效果设置，使用时不必添加，或者添加少量即可
	MouseLClick();//该操作为退出放大效果，目前执行完该操作才可以执行其他操作(例如鼠标右击)

}

void CarryMove::quit()
{
	MouseRClick();
	Sleep(50);
	setNum(69);//E键模拟
}
