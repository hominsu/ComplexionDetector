#ifndef _CLIENT_H
#define _CLIENT_H

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <vector>
#include <Winsock2.h> 
#include <Ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define BUFSIZE 1024


class Client
{
private:
    // socket版本
    WORD wVersionRequested;
    WSADATA wsaData;
    SOCKET s_socket;    // 描述服务器的套接字
    struct sockaddr_in serverAddr;
    
    char buf[BUFSIZE];  // 缓冲数组
    int rval = 0;   // 接受反馈
    
    std::ifstream ifs;  // 文件流指针，用于打开文件

public:
    size_t      fileSize;    // 描述文件大小
    size_t      sentBytes;   // 描述已发送的字节
    std::string name;   // 发送的文件名

public:
    Client(const char addr[], const int port);
    ~Client();

    // TCP连接建立以及相关信息传输
    bool work(const char filename[]);

private:
    // 通过套接字发送文件
    bool sendFile(const char filename[]);
};

#endif
