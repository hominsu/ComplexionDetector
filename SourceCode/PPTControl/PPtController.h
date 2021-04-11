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
	bool quitAddAction = false;	// ����detach���̵߳Ĺر�
	int action = 0;	// ��¼����Ķ�����Ȼ���߳��е�addAction��ȡ
	bool setactionstatus = false;	// ��¼���붯����״̬�������߳��е�addAction��ȡ
	bool executeactionststus = false;
	std::vector<size_t>* actionnum = NULL;	// ��¼��Ӧ��������Ĵ�������

	pptControl* ppt = NULL;
	const int timelimit = 3000;	// �޶�ʱ���ڲ���ִ�ж���
	std::chrono::system_clock::time_point lastactiontime;	// ��¼�ϴβ���ʱ��
	std::thread actionThread;	// �߳�
	std::mutex m_mutex;	// �߳���

};

