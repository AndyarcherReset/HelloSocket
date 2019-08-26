#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>

//Windowsϵͳ�е���lib
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
    short dataLength;//���ݳ���
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

    // �����׽���
    SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //���ӷ�����
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

// ���г���: Ctrl + F5 ����� >����ʼִ��(������)���˵�
// ���Գ���: F5 ����� >����ʼ���ԡ��˵�

// ������ʾ: 
//   1. ʹ�ý��������Դ�������������/�����ļ�
//   2. ʹ���Ŷ���Դ�������������ӵ�Դ�������
//   3. ʹ��������ڲ鿴���������������Ϣ
//   4. ʹ�ô����б��ڲ鿴����
//   5. ת������Ŀ��>���������Դ����µĴ����ļ�����ת������Ŀ��>�����������Խ����д����ļ���ӵ���Ŀ
//   6. ��������Ҫ�ٴδ򿪴���Ŀ����ת�����ļ���>���򿪡�>����Ŀ����ѡ�� .sln �ļ�
