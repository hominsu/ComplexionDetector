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
	void setData(const std::string dir,const int signal);//给该对象的参数赋值
	void update(int signal);//更新指令,内部有延迟1s的操作
	std::string getDir();//获得路径，和this->Fdir一个意思
	int getSign();//获得信号
	bool Run();//执行指令，在update函数中执行

	int isStart;//判定文件是否被打开，这里的打开是指在函数中执行打开文件的操作，不是
				//真实文件被其他操作打开，isStart为真时，诸如播放等动作才可以执行，否则
				//不能执行该变量在keyboard对象生成时就在构造函数被置为1

private:
	std::string Fdir;//路径
	int Sign;//信号
	keyboard* key;//最终控制操作的临时对象
};

