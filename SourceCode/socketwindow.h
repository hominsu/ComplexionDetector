#pragma once

#include <QWidget>
#include "ui_socketwindow.h"

class SocketWindow : public QWidget
{
	Q_OBJECT

public:
	SocketWindow(QWidget *parent = Q_NULLPTR);
	~SocketWindow();

private:
	Ui::SocketWindow ui;
};
