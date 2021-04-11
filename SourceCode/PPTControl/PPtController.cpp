#include "PPtController.h"

PPtController::PPtController(std::string dir)
{
	actionnum = new std::vector<size_t>(6);

	ppt = new pptControl(dir, 0);	//�������ƶ���

	lastactiontime = std::chrono::system_clock::now();

	// addAction���������̣߳���thisָ�룬ȷ�����Բ�����������
	actionThread = std::thread(std::bind(&PPtController::addAction, this));

	// detach
	actionThread.detach();
}

PPtController::~PPtController()
{
	// �ر��߳�
	quitAddAction = true;

	// �ͷ���Դ
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
	// �ⲿ����action�Ľӿ�
	this->action = action;
	this->setactionstatus = true;
}

void PPtController::setActionNumZero()
{
	// ��ʼ��Ϊ0
	for (auto& i : *actionnum)
	{
		i = 0;
	}
}

void PPtController::executeAction()
{
	// ���������⾺��
	std::lock_guard<std::mutex> lock(m_mutex);

	executeactionststus = true;

	// �ҳ�������������Ŀ��Ӧ�������±꣬��Ӧ����������ı��1~5
	auto maxPosition = std::max_element(actionnum->begin(), actionnum->end());
	// �鿴��ǰʱ������ϴβ�����ʱ��
	auto diff = (std::chrono::system_clock::now() - lastactiontime).count() / 10000;

	if (*maxPosition != 0 && diff > timelimit)
	{
		auto maxPositionIndex = maxPosition - actionnum->begin();
		ppt->update(static_cast<int> (maxPositionIndex));

		lastactiontime = std::chrono::system_clock::now();

		// ��ʼ������Ϊ0
		setActionNumZero();
	}

	executeactionststus = false;
}

void PPtController::addAction()
{
	while (true)
	{
		// �������action����������Ӧ���±�Ŀռ�
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
		// �˳�ѭ������������
		if (quitAddAction)
		{
			break;
		}
	}
}
