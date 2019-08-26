// HelloSocket.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "pch.h"
#include <iostream>
// 不引用重复define
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>



//Windows系统中导入lib
#pragma comment (lib, "ws2_32.lib")

enum CMD
{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOFF,
    CMD_LOGOFF_RESULT,
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
        printf("bind the port successfully!");
        if (SOCKET_ERROR != listen(_sock, 5))
        {
            printf("listen successfully!");

            //accept
            sockaddr_in clientAddr = {};
            int nAddrLne = sizeof(sockaddr_in);

            SOCKET _cSock = INVALID_SOCKET;

            _cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLne);
            printf("new client: %s \n", inet_ntoa(clientAddr.sin_addr));

            while (true)
            {
                DataHeader header = {};
                //get the data from client
                int nLen = recv(_cSock, (char*)&header, sizeof(DataHeader), 0);
                if (nLen <= 0 )
                {
                    printf("client quits!");
                    break;
                }
     
                switch (header.cmd)
                {
                case CMD_LOGIN:
                {
                    Login login = {};
                    //报头已被接受，内存需偏离
                    recv(_cSock, (char*)&login + sizeof(DataHeader), sizeof(Login) - sizeof(DataHeader), 0);
                    printf("收到命令： %d 数据长度： %d \n username = %s , password = %s \n", 
                        login.cmd, login.dataLength, login.userName, login.passWord);

                    //to do: check the username and password

                    //send the message
                    LoginResult loginResult;
                    send(_cSock, (char *)&loginResult, sizeof(loginResult), 0);
                }
                break;
                case CMD_LOGOFF:
                {
                    LoginOff loginOff = {};
                    int nLen = recv(_cSock, (char*)&loginOff + sizeof(DataHeader), sizeof(LoginOff) - sizeof(DataHeader), 0);
                    printf("收到命令： %d 数据长度： %d \n ", loginOff.cmd, loginOff.dataLength);

                    LoginOffResult loginOffResult;
                    send(_cSock, (char *)&loginOffResult, sizeof(LoginOffResult), 0);
                }
                break;
                default:
                    header.cmd = CMD_ERROR;
                    header.dataLength = 0;
                    send(_cSock, (char *)&header, sizeof(DataHeader), 0);
                break;
                }
            }

            // close client 
            closesocket(_cSock);
        }
    }
    else
    {
        printf("failed to bind port");
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
