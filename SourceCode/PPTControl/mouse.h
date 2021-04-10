#pragma once
#include <iostream>
#include "windows.h"

class mouse
{
public:
	mouse();
	void getPos();//获取位置，暂时用不到，但是不要删
	void MouseLClick();//模拟鼠标左击，这里是适用于点击分辨率为1920*1080的屏幕中央，所以换到其他清晰度可能会出问题
	void MouseRClick();//模拟鼠标右击
	int x, y;//坐标值
private:
	

};

