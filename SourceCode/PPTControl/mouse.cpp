#include "mouse.h"


mouse::mouse()
{
	int width = GetSystemMetrics(SM_CXSCREEN);
	int height = GetSystemMetrics(SM_CYSCREEN);
	this->x = width;
	this->y = height;
}

void mouse::getPos()
{
	while (1)
	{
		POINT pt = { 0,0 };
		GetCursorPos(&pt);
		this->x = pt.x;
		this->y = pt.y;
		printf("x=%d,y=%d\n", pt.x, pt.y);
		Sleep(3000);
	}
}

void mouse::MouseLClick()
{
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 960, 540, 0, 0);
}

void mouse::MouseRClick()
{
	mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
}