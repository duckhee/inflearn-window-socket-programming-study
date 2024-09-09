#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <Windows.h>


using namespace std;


int main(int arg, char **argv) {
    WSADATA wsa = {0,};
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "Failed Window socket Initialized..." << endl;
        return -1;
    }

    SOCKET hClient;
    SOCKADDR_IN serverInfo = {0,};
    char pszBuffer[128] = {0,};

    hClient = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hClient == INVALID_SOCKET) {
        cout << "Failed Create socket" << endl;
        return -1;
    }
    /** server information setting */
    serverInfo.sin_family = PF_INET;
    InetPton(AF_INET, _T("127.0.0.1"), &serverInfo.sin_addr.S_un.S_addr);
    serverInfo.sin_port = htons(25000);

    /** connect server */
    int isConnected = ::connect(hClient, (SOCKADDR *) &serverInfo, sizeof(serverInfo));

    /** 서버와 연결 실패 시 동작 */
    if (isConnected == SOCKET_ERROR) {
        cout << "Failed connected server..." << endl;
        puts("ERROR: 서버와 연결할 수 없습니다.");
        fflush(stdout);
        ::closesocket(hClient);
        return -1;
    }

    /** socket 의 kernel IO Buffer 사용하지 않도록 설정 */
    int nOpt = 1;
    /** 옵션을 설정하는 함수 */
    int isTCPNoDelay = ::setsockopt(
            hClient, // 
            IPPROTO_TCP, // 어느 수준에서 적용을 할 것인지에 대한 값을 인자로 받는다.
            TCP_NODELAY, // socket io buffering 을 사용하지 않기 위한 설정 
            (char *) &nOpt, // 설정 값이 담겨 있는 버퍼에 대한 인자 
            sizeof(nOpt) // 버퍼의 크기 알져귀
    );

    while (true) {
        gets_s(pszBuffer, 128);
        if (strcmp(pszBuffer, "EXIT") == 0) {
            break;
        }
        /** 데이터 보내기 */
//        ::send(hClient, pszBuffer, 128, 0);
        /** 데이터를 1byte로 보내기 */
        size_t bufferSize = strlen(pszBuffer);
        for(int i = 0; i < bufferSize; i++)
        {
            ::send(hClient, pszBuffer+i, 1, 0);
        }
        /** 버퍼 초기화 */
        memset(pszBuffer, '\0', sizeof(pszBuffer));
        /** 데이터 전송 */
        ::recv(hClient, pszBuffer, 128, 0);
        /** 데이터 읽어오기 */
        printf("From Server : %s\r\n", pszBuffer);
    }
    /** 소켓 데이터 송 수신 종료 */
    ::shutdown(hClient, SD_BOTH);
    /** 소켓 닫기 */
    ::closesocket(hClient);
    WSACleanup();
    return 0;
}