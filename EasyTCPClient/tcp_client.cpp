#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <thread>

//Windowsϵͳ�е���lib
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

int processor(SOCKET _cSock)
{
    //������
    char szRecv[1024] = {};
    //get the data from client
    int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
    DataHeader *header = (DataHeader *)szRecv;
    if (nLen <= 0)
    {
        printf("��������Ͽ�����!\n", _cSock);
        return -1;
    }

    switch (header->cmd)
    {
    case CMD_LOGIN_RESULT:
    {
        recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
        LoginResult* loginResult = (LoginResult*)szRecv;
        printf("�յ����������ݣ� %d  ���ݳ��ȣ� %d \n ", loginResult->cmd, loginResult->dataLength);
    }
    break;
    case CMD_LOGOFF_RESULT:
    {
        recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
        LoginOffResult* loginResult = (LoginOffResult*)szRecv;
        printf("�յ����������ݣ� %d  ���ݳ��ȣ� %d \n ", loginResult->cmd, loginResult->dataLength);
    }
    break;
    case CMD_NEW_USER_JOIN:
    {
        recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
        NewUserJoin* newUserJoin = (NewUserJoin*)szRecv;
        printf("�յ����������ݣ� %d  ���ݳ��ȣ� %d \n ", newUserJoin->cmd, newUserJoin->dataLength);
    }
    break;
    }
    // close client 
    //closesocket(_cSock);
}
bool g_bRun = true;
void cmdThread(SOCKET _sock)
{
    while (true)
    {
        char cmdBuf[256] = {};
        scanf("%s", cmdBuf);

        if (strcmp(cmdBuf, "exit") == 0)
        {
            printf("�˳�cmdThread�߳�!");
            g_bRun = false;
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
            printf("Login Result: %d \n ���ݳ��ȣ� %d \n", serverLogin.result, serverLogin.dataLength);
        }
        else if (strcmp(cmdBuf, "logout") == 0)
        {
            LoginOff longinOff;
            strcmp(longinOff.userName, "Andy");
            send(_sock, (char *)&longinOff, sizeof(LoginOff), 0);

            LoginOffResult serverLogoff = {};
            recv(_sock, (char *)&serverLogoff, sizeof(LoginOffResult), 0);
            printf("Logoff Result: %d \n ���ݳ��ȣ� %d \n", serverLogoff.result, serverLogoff.dataLength);

        }
        else
            printf("unknown command!");
    }
}
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
        
        std::thread  t1(cmdThread, _sock);
        t1.detach();

        while (g_bRun)
        {
            fd_set fdRead;
            FD_ZERO(&fdRead);
            FD_SET(_sock, &fdRead);

            timeval t = { 0,0 };
            int ret = select(_sock, &fdRead, NULL, NULL, &t);

            if (ret < 0)
            {
                printf("select���������\n");
            }

            if (FD_ISSET(_sock, &fdRead))
            {
                FD_CLR(_sock, &fdRead);

                if (-1 == processor(_sock))
                {
                    printf("select���������\n");
                    break;
                }
            }

            //printf("����ʱ�䴦������ҵ��...\n");
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
