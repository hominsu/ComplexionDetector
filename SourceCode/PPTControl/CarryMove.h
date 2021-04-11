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
	void CarrySelect(int signal);//相当于操作的选择部分，用switch实现,在构造函数中被引用

private:
	void setNum(int num);//用于封装模拟键盘的操作，num就是对应键盘的一个数值，数值在windows官方文档里有
	void setNum2(int num1, int num2);//用于组合键，目前还没用到组合键，暂时保留
	void Start();//开始播放，目前仅支持从首页开始，后续做调整
	void changeNext();//切换到下一页
	void changeBefore();//切换到上一页
	void enlarge();//对当前页面进行放大
	void quit();//退出播放

};

