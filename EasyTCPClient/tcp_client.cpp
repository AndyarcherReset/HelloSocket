#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>

//Windowsϵͳ�е���lib
#pragma comment (lib, "ws2_32.lib")


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
                break;
            }
            else
            {
                send(_sock, cmdBuf, strlen(cmdBuf), 0);
            }

            //recv the message
            char recvBuf[256] = {};
            int nlen = recv(_sock, recvBuf, 256, 0);

            if (nlen > 0)
            {
                printf(recvBuf);
            }
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
