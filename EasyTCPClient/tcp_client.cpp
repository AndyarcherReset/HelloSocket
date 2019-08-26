#include <iostream>
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

    //连接服务器
    sockaddr_in _sin = {};
    _sin.sin_family = AF_INET;
    _sin.sin_port = htons(4567);
    _sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    int ret = connect(_sock, (sockaddr*)(&_sin), sizeof(sockaddr_in));

    if (ret != SOCKET_ERROR)
    {
        char cmdBuf[128] = {};

        while (true)
        {
            scanf("%s", cmdBuf);
            if (strcmp(cmdBuf, "exit") == 0)
            {
                printf("user exits!");
                break;
            }
            else if (strcmp(cmdBuf, "login") == 0)
            {
                //send the message to server
                Login login;
                strcpy(login.passWord, "Andy");
                strcpy(login.userName, "Andy");
                send(_sock, (char *)&login, sizeof(Login), 0);

                //get the message from server
                LoginResult serverLogin = {};
                recv(_sock, (char *)&serverLogin, sizeof(LoginResult), 0);
                printf("Login Result: %d \n", serverLogin.result);
            }
            else if (strcmp(cmdBuf, "logout") == 0)
            {
                LoginOff longinOff;
                strcmp(longinOff.userName, "Andy");
                send(_sock, (char *)&longinOff, sizeof(LoginOff), 0);

                LoginOffResult serverLogoff = {};
                recv(_sock, (char *)&serverLogoff, sizeof(LoginOffResult), 0);
                printf("Logoff Result: %d \n", serverLogoff.result);
                
            }
            else
                printf("unknown command!");
        }
        //close
        closesocket(_sock);
    }
    
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
