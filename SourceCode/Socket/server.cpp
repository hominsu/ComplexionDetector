#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "server.h"


// 构造函数，初始化socket
Server::Server(const int port)
{
    // 初始化socket版本
    wVersionRequested = MAKEWORD(2, 2);
    WSAStartup(wVersionRequested, &wsaData);

    // 建立描述服务器的套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        std::cout << "Can not create socket!" << std::endl;
        exit(0);
    }

    // 设置地址簇为Internet协议族
    server.sin_family = AF_INET;
    // 设置端口
    server.sin_port = htons(port);
    // server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    // 设置IP地址任何地址
    server.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    // 填充0
    memset(server.sin_zero, 0, sizeof(server.sin_zero));

    // 将本机IP和端口与sockfd绑定
    rval = bind(sockfd, (struct sockaddr*)&server, sizeof(server));
    if (rval == -1) 
    {
        // 异常处理
        std::cout << "Can not create connect!" << std::endl;
        exit(0);
    }

    // 监听sockfd套接字，限定最大等待队列数为5
    listen(sockfd, 5);
}

// 析构函数，如果套接字存在，就关闭
Server::~Server()
{
    if(s_socket != INVALID_SOCKET)
    {
        closesocket(s_socket);
    }
    closesocket(sockfd);
    // 清除Socket信息
    WSACleanup();
}

bool Server::work()
{
    char filename[BUFSIZE] = {0};
    int tcpAddrLenth = sizeof(tcpAddr);

    // s_socket接受sockfd返回的socket描述符，tcpaddr存放客户端的信息
    if((s_socket = accept(sockfd, (struct sockaddr*)&tcpAddr, &tcpAddrLenth)) == INVALID_SOCKET)
    {
        // 异常处理
        std::cout << "Accept exception";
        exit(1);
    }

    // 转换IP地址
    strcpy(clientIp, inet_ntoa(tcpAddr.sin_addr));
    clientPort = ntohs(tcpAddr.sin_port);

    // 测量延时
    //measureDelay();
    memset(buf, 0, BUFSIZE);

    // 接收文件名
    recv(s_socket, filename, BUFSIZE, 0);
    strcpy(file, filename);

    // 发送确认信息
    strcpy(buf, "recv");
    send(s_socket, buf, BUFSIZE, 0);

    // 接受文件
    if(!recvFile(filename, time))
    {
        return false;
    }
    // 接收成功后就关闭套接字
    closesocket(s_socket);

    return true;
}

bool Server::recvFile(const char filename[], double& time)
{
    // 获取当前系统时间
    auto start = std::chrono::system_clock::now();

    // readLen标识从文件流中读取到buf缓冲数组中的字节数
    int readLen = 0;
    recvSize = 0;

    // 创建接受的文件流
    ofs.open(filename, std::ios::out | std::ios::binary);
    if(!ofs.is_open())
    {
        std::cout << "Can not open " << filename << std::endl;
        exit(0);
    }

    // 初始化buf缓冲数组为0
    memset(buf, 0, sizeof(buf));
    do
    {
        readLen = recv(s_socket, buf, BUFSIZE, 0);
        if(readLen == -1)
	    {
            // 异常处理
	        ofs.close();
            return false;
	    }
        else if(readLen == 0)
        {
            // readLen为0说明远端已经传输完成并且断开了TCP连接
            break;
        }
        recvSize += readLen;

        // 将buf缓冲数组的内容写入到文件流
	    ofs.write(buf, readLen);
        // 初始化buf缓冲数组为0
        memset(buf, 0, sizeof(buf));
    } while(true);

    // 接收成功后就关闭套接字
    ofs.close();

    fileSize = recvSize;

    // 获取当前系统时间，与开始时间相减得到接收文件所用时间
    std::chrono::duration<double> diff = std::chrono::system_clock::now() - start;
    time = diff.count();

    return true;
}

