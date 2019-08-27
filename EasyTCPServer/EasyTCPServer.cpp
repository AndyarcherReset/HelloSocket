// HelloSocket.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "pch.h"
#include <iostream>
// 不引用重复define
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <vector>



//Windows系统中导入lib
#pragma comment (lib, "ws2_32.lib")

enum CMD
{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOFF,
    CMD_LOGOFF_RESULT,
    CMD_NEW_USER_JOIN,
    CMD_ERROR
};

struct DataHeader
{
    short dataLength;//数据长度
    short cmd;
};

//DataPackage
struct Login : public DataHeader
{
    Login()
    {
        dataLength = sizeof(Login);
        cmd = CMD_LOGIN;
    }
    char userName[32];
    char passWord[32];
};

struct LoginResult : public DataHeader
{
    LoginResult()
    {
        dataLength = sizeof(LoginResult);
        cmd = CMD_LOGIN_RESULT;
        result = 0;
    }
    int result;
    
};

struct LoginOff : public DataHeader
{
    LoginOff()
    {
        dataLength = sizeof(LoginOff);
        cmd = CMD_LOGOFF;
    }
    char userName[32];
};

struct LoginOffResult : public DataHeader
{
    LoginOffResult()
    {
        dataLength = sizeof(LoginOffResult);
        cmd = CMD_LOGOFF_RESULT;
        result = 0;
    }
    int result;

};

struct NewUserJoin : public DataHeader
{
    NewUserJoin()
    {
        dataLength = sizeof(NewUserJoin);
        cmd = CMD_NEW_USER_JOIN;
        socketID = 0;
    }
    int socketID;

};

std::vector<SOCKET> g_clients;

int processor(SOCKET _cSock)
{
    //缓冲区
    char szRecv[1024] = {};
    //get the data from client
    int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
    DataHeader *header = (DataHeader *)szRecv;
    if (nLen <= 0)
    {
        printf("client <SOCKET %d> quits!\n", _cSock);
        return -1;
    }

    switch (header->cmd)
    {
    case CMD_LOGIN:
    {
        //报头已被接受，内存需偏离
        recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
        Login* login = (Login*)szRecv;
        printf("收到客户端<SOCKET %d>请求： %d 数据长度： %d \n username = %s , password = %s \n",
            _cSock, login->cmd, login->dataLength, login->userName, login->passWord);

        //to do: check the username and password

        //send the message
        LoginResult loginResult;
        send(_cSock, (char *)&loginResult, sizeof(loginResult), 0);
    }
    break;
    case CMD_LOGOFF:
    {

        int nLen = recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
        LoginOff *loginOff = (LoginOff*)szRecv;
        printf("收到客户端<SOCKET %d>请求： %d 数据长度： %d \n ", _cSock, loginOff->cmd, loginOff->dataLength);

        LoginOffResult loginOffResult;
        send(_cSock, (char *)&loginOffResult, sizeof(LoginOffResult), 0);
    }
    break;
    default:
    {
        DataHeader header = { 0, CMD_ERROR };
        send(_cSock, (char *)&header, sizeof(DataHeader), 0);
    }
    break;
    }
    // close client 
    //closesocket(_cSock);
}
int main()
{
    WORD ver = MAKEWORD(2, 2);
    WSADATA dat;
    WSAStartup(ver, &dat);

    // 创建套接字
    SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // 绑定端口
    sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(4567); // host to net unsigned short
    _sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1"); //INADDR_ANY -> 任意ip
    if (bind(_sock, (sockaddr*)&_sin, sizeof(_sin)) !=  SOCKET_ERROR)
    {
        //监听网络端口
        printf("bind the port successfully! \n");

        if (SOCKET_ERROR != listen(_sock, 5))
        {
            printf("listen successfully! \n");

            while (true)
            {
                //伯克利socket
                fd_set fdRead; //描述符集合
                fd_set fdWrite;
                fd_set fdExp;
                
                //清理集合
                FD_ZERO(&fdRead);
                FD_ZERO(&fdWrite);
                FD_ZERO(&fdExp);

                //将描述符（socket）加入集合
                FD_SET(_sock, &fdRead);
                FD_SET(_sock, &fdWrite);
                FD_SET(_sock, &fdExp);

                //for (size_t n = 0;n < g_clients.size() ; ++n)
                //{
                //    FD_SET(g_clients[n], &fdRead);
                //}
                
                for (int n = (int)g_clients.size()-1;n >= 0; n--)
                {
                    FD_SET(g_clients[n], &fdRead);
                }

                //nfds 是一个整数值，是指fd_set集合中所有描述符（socket）的范围，而不是数量，
                //既是所有文件描述符+1， 在Windows中可任意
                timeval t = { 0,0 }; //timeout 为0，非阻塞模式
                int ret = select(_sock + 1/*windows下任意*/, &fdRead, &fdWrite, &fdExp, &t);
                if (ret < 0)
                {
                    printf("select任务结束\n");
                    break;
                }

                //判断描述符（socket）是否在集合中
                if (FD_ISSET(_sock, &fdRead))//检查是否有可读
                {
                    FD_CLR(_sock, &fdRead); //只清理计数器，而没有清理数据

                    //accept
                    SOCKET _cSock = INVALID_SOCKET;
                    sockaddr_in clientAddr = {};
                    int nAddrLne = sizeof(sockaddr_in);

                    _cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLne);
                    if (_cSock == INVALID_SOCKET)
                        printf("Invalid socket!\n");
                    else
                    {
                        for (int n = (int)g_clients.size() - 1; n >= 0; n--)
                        {
                            NewUserJoin userJoin;
                            send(g_clients[n], (const char*)&userJoin, sizeof(userJoin), 0);
                        }
                        printf("new client: %s joins! \n", inet_ntoa(clientAddr.sin_addr));
                        g_clients.push_back(_cSock);
                    }
                }

                for (size_t n = 0 ; n < fdRead.fd_count ; n++)
                {
                    if (-1 == processor(fdRead.fd_array[n]))
                    {
                        auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
                        if (iter != g_clients.end())
                            g_clients.erase(iter);
                    }
                }

                //printf("空闲时间处理其他业务...\n");
            }
        }
    }
    else
    {
        printf("failed to bind port");
    }
    
    for (size_t n = 0;n < g_clients.size(); ++n)
    {
        closesocket(g_clients[n]);
    }

    closesocket(_sock);

    WSACleanup();

    getchar();
    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
