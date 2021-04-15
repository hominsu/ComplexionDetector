#include "socketwindow.h"

SocketWindow::SocketWindow(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::SocketWindow)
{
	ui->setupUi(this);
	ui->lineEditIP->setText("127.0.0.1");
	connect(ui->pushButtonOK, &QPushButton::clicked, this, &SocketWindow::sendIPSlot);
	connect(ui->pushButtonCancel, &QPushButton::clicked, this, &SocketWindow::sendCancelSlot);
}

SocketWindow::~SocketWindow()
{
	delete ui;
}

void SocketWindow::sendIPSlot()
{
	emit sendIPSiginal(ui->lineEditIP->text());
}

void SocketWindow::sendCancelSlot()
{
	emit sendCancelSignal();
}
