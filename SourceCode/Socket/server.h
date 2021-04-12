#ifndef _SERVER_H
#define _SERVER_H

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <Winsock2.h> 
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define BUFSIZE 1024

class Server
{
private:
    // socket版本
    WORD wVersionRequested;
    WSADATA wsaData;
    SOCKET sockfd, s_socket;    // 描述服务器的套接字
    struct sockaddr_in server;  // 描述服务器信息的结构体
    struct sockaddr_in tcpAddr; // 描述客户端的结构体
    
    char buf[BUFSIZE];  // 缓冲数组
    int rval = 0;       // 接受反馈

    std::ofstream ofs;  // 文件流指针，用于打开文件

public:
    char    file[BUFSIZE];  // 存放文件名
    char    clientIp[128];  // 存放客户端IP
    int     clientPort;     // 客户端端口

public:
    size_t fileSize;    // 描述文件大小
    size_t recvSize;    // 描述已接收的字节
    double time;    // 接受文件所用时间

public:
    Server(const int port);
    ~Server();
    
    // TCP连接建立以及相关信息传输
    bool work();

private:
    // 通过套接字接受文件
    bool recvFile(const char filename[], double& time);
    
};

#endif
