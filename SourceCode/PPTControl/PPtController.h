#pragma once
#include <iostream>
#include <algorithm>
#include <functional>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
#include "pptControl.h"


class PPtController
{
public:
	PPtController(std::string dir);
	~PPtController();

public:
	void addAction();
	void setAction(int action);
	void executeAction();
	void setActionNumZero();

private:
	bool quitAddAction = false;	// 控制detach的线程的关闭
	int action = 0;	// 记录传入的动作，然后线程中的addAction读取
	bool setactionstatus = false;	// 记录传入动作的状态，触发线程中的addAction读取
	bool executeactionststus = false;
	std::vector<size_t>* actionnum = NULL;	// 记录对应五个动作的触发次数

	pptControl* ppt = NULL;
	const int timelimit = 3000;	// 限定时间内不能执行动作
	std::chrono::system_clock::time_point lastactiontime;	// 记录上次操作时间
	std::thread actionThread;	// 线程
	std::mutex m_mutex;	// 线程锁

};

