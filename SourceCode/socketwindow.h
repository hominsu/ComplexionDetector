#pragma once

#include <QDialog>
#include <QTextEdit>
#include "ui_socketwindow.h"

class SocketWindow : public QDialog
{
	Q_OBJECT

public:
	SocketWindow(QWidget *parent = Q_NULLPTR);
	~SocketWindow();

private:
	Ui::SocketWindow *ui;

signals:
	void sendIPSiginal(QString IPAddr);
	void sendCancelSignal();

private slots:
	void sendIPSlot();
	void sendCancelSlot();
};
