#include "Socket.h"

void Socket::ServerNewConnection()
{
    mp_TCPSocket = mp_TCPServer->nextPendingConnection();
    if (!mp_TCPSocket)
    {
        return;
    }
    else
    {
        connect(mp_TCPSocket, SIGNAL(readyRead()), this, SLOT(ServerReadData()));//服务端socket连接 读取信号 接收读取客户端的数据
        connect(mp_TCPSocket, SIGNAL(disconnected()), this, SLOT(sServerDisConnection()));//断开连接的提示，无实际用处
    }
}

void Socket::ServerReadData()
{
    char buffer[1024] = { 0 };
    mp_TCPSocket->read(buffer, 1024);
    if (strlen(buffer) > 0)
    {
        QString showNsg = buffer;
        ui->m_recvDataTextEdit->append(showNsg);
    }
}

void Socket::OnBtnInitSocket()
{
    mp_TCPServer = new QTcpServer();
    int port = ui->m_portLineEdit->text().toInt();
}

void Socket::OnBtnSendData()
{

    char sendMsgChar[1024] = { 0 };
    QString sendMsg = ui->m_inputTextEdit->toPlainText();

    strcpy_s(sendMsgChar, sendMsg.toStdString().c_str());
    if (mp_TCPSocket->isValid())
    {
        int sendRe = mp_TCPSocket->write(sendMsgChar, strlen(sendMsgChar));

    }
}

void Socket::sServerDisConnection()
{
    return;
}

void Socket::on_m_connectServerBtn_clicked()
{
    mp_clientSocket = new QTcpSocket();
     QString ip = ui->m_serverIPLineEdit->text();
     int port = ui->m_serverPortLineEdit->text().toInt();
     mp_clientSocket->connectToHost(ip, port);
     if(!mp_clientSocket->waitForConnected(30000))
     {
         return;
     }
     connect(mp_clientSocket, SIGNAL(readyRead()), this, SLOT(ClientRecvData()));//socket连接 读取信号 槽连接到接收读取的数据

}
void Socket::on_sendDataBtn_clicked()
{
    char sendMsgChar[1024] = {0};//发送的数据
    strcpy_s(sendMsgChar, sendMsg.toStdString().c_str());
    int sendRe  = mp_clientSocket->write(sendMsgChar, strlen(sendMsgChar));
    if(sendRe == -1)
    {
        return;
    }

}
void Socket::ClientRecvData()
{
    char recvMsg[1024] = {0};//接收的数据

    int recvRe = mp_clientSocket->read(recvMsg,1024);
    QString showQstr = recvMsg;
}