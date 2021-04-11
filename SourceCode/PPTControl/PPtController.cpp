#include "PPtController.h"

PPtController::PPtController(std::string dir)
{
	actionnum = new std::vector<size_t>(6);

	ppt = new pptControl(dir, 0);	//创建控制对象

	lastactiontime = std::chrono::system_clock::now();

	// addAction函数移入线程，绑定this指针，确保可以操作类内数据
	actionThread = std::thread(std::bind(&PPtController::addAction, this));

	// detach
	actionThread.detach();
}

PPtController::~PPtController()
{
	// 关闭线程
	quitAddAction = true;

	// 释放资源
	if (ppt != NULL)
	{
		delete ppt;
	}
	ppt = NULL;

	if (actionnum != NULL)
	{
		delete actionnum;
	}
	actionnum = NULL;
}

void PPtController::setAction(int action)
{
	// 外部传入action的接口
	this->action = action;
	this->setactionstatus = true;
}

void PPtController::setActionNumZero()
{
	// 初始化为0
	for (auto& i : *actionnum)
	{
		i = 0;
	}
}

void PPtController::executeAction()
{
	// 上锁，避免竞争
	std::lock_guard<std::mutex> lock(m_mutex);

	executeactionststus = true;

	// 找出容器中最大的数目对应的数组下标，对应到五个动作的编号1~5
	auto maxPosition = std::max_element(actionnum->begin(), actionnum->end());
	// 查看当前时间距离上次操作的时间
	auto diff = (std::chrono::system_clock::now() - lastactiontime).count() / 10000;

	if (*maxPosition != 0 && diff > timelimit)
	{
		auto maxPositionIndex = maxPosition - actionnum->begin();
		ppt->update(static_cast<int> (maxPositionIndex));

		lastactiontime = std::chrono::system_clock::now();

		// 初始化容器为0
		setActionNumZero();
	}

	executeactionststus = false;
}

void PPtController::addAction()
{
	while (true)
	{
		// 将传入的action放入容器对应的下表的空间
		if (setactionstatus == true && executeactionststus == false)
		{
			switch (action + 1)
			{
			case DIANJI:
				++(*actionnum)[DIANJI];
				break;

			case PINGYI:
				++(*actionnum)[PINGYI];
				break;

			case SUOFANG:
				++(*actionnum)[SUOFANG];
				break;

			case ZHUAQU:
				++(*actionnum)[ZHUAQU];
				break;

			case XUANZHUAN:
				++(*actionnum)[XUANZHUAN];
				break;

			default:
				break;
			}
			executeAction();
			setactionstatus = false;
		}
		// 退出循环，结束进程
		if (quitAddAction)
		{
			break;
		}
	}
}
