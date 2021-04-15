#pragma once
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QTextEdit>
#include <iostream>
#include <string>


class Socket
{
    Q_OBJECT

private:
    QTcpSocket* mp_clientSocket = NULL;//��ʼ���յ�socket�ͻ�������
    QTcpServer* mp_TCPServer;//�����socket
    QTcpSocket* mp_TCPSocket;

public slots:
    void on_m_connectServerBtn_clicked();//����˽�������
    void on_sendDataBtn_clicked();//���ѽ������ӵķ���˷�������
    void ClientRecvData();//����˽�������
    void OnBtnInitSocket();//��ʼ�����񣬿�ʼ��ָ���˿ڼ���
    void OnBtnSendData();//��������
    void ServerReadData();//��������
    void ServerNewConnection();//�����µ�socket����
    void sServerDisConnection();//socket�Ͽ�ʱ�ĶϿ���ʾ����ʵ���ô�
};

