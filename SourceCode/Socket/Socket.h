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
    QTcpSocket* mp_clientSocket = NULL;//初始化空的socket客户端连接
    QTcpServer* mp_TCPServer;//服务端socket
    QTcpSocket* mp_TCPSocket;

public slots:
    void on_m_connectServerBtn_clicked();//服务端建立连接
    void on_sendDataBtn_clicked();//给已建立连接的服务端发送数据
    void ClientRecvData();//服务端接收数据
    void OnBtnInitSocket();//初始化服务，开始在指定端口监听
    void OnBtnSendData();//发送数据
    void ServerReadData();//接收数据
    void ServerNewConnection();//建立新的socket连接
    void sServerDisConnection();//socket断开时的断开提示，无实际用处
};

