#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <tchar.h>


using namespace std;

int main(int argc, char **argv) {
    /** window socket 인 winsock을 사용하기 위한 초기화 */
    WSAData wsaData = {0,};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Failed Win Sock Initialized ..." << endl;
        return -1;
    }
    SOCKET hServerSocket, hClientSocket;
    /** TCP 소켓 생성 */
    hServerSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    /** socket 확인 */
    if (hServerSocket == INVALID_SOCKET) {
        cout << "Failed Socket Created..." << endl;
        /** window socket 인 winsock 자원 반환 */
        WSACleanup();
        return -1;
    }

    /** socket option SO_REUSEADDR 부여 */
    int nOpt = 1;
    /** socket에 SO_REUSEADDR 부여 */
    ::setsockopt(hServerSocket, SOL_SOCKET, SO_REUSEADDR, (char *) &nOpt, sizeof(nOpt));
    /** bind 를 진행할 속성 정의 */
    SOCKADDR_IN serverInfo = {0,};
    serverInfo.sin_family = PF_INET;
    /** SO_REUSEADDR 을 사용할 경우 접속을 허용할 IP에 대해서 명시를 해주는 것이 좋다. */
//    serverInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    /** Loop Back 주소를 지정해서 사용 */
    serverInfo.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serverInfo.sin_port = htons(25000);

    /** socket 을 network 에 binding */
    int isBind = ::bind(hServerSocket, (SOCKADDR *) &serverInfo, sizeof(serverInfo));
    /** bind 확인 */
    if (isBind == SOCKET_ERROR) {
        cout << "Failed Socket bind error..." << endl;
        closesocket(hServerSocket);
        /** window socket 인 winsock 자원 반환 */
        WSACleanup();
        return -1;
    }

    /** 접속 요청에 대해서 대기 */
    int isListen = ::listen(hServerSocket, SOMAXCONN);

    /** 제대로 요청 대기 중인지 확인 */
    if (isListen == SOCKET_ERROR) {
        cout << "Failed Socket Listen..." << endl;
        closesocket(hServerSocket);
        /** window socket 인 winsock 자원 반환 */
        WSACleanup();
        return -1;
    }

    cout << "Server Listening ..." << endl;

    /** 접속한 client socket 에 대한 정보를 저장할 구조체 및 송순신 버퍼 */
    SOCKADDR_IN clientInfo = {0,};
    int clientInfoLength = sizeof(clientInfo);
    int nReceiveLength = 0;
    char pszBuffer[128] = {0,};

    /** 접속 연결 허용 후 동작 */
    while ((hClientSocket = ::accept(hServerSocket, (SOCKADDR *) &clientInfo, &clientInfoLength)) != INVALID_SOCKET) {
        puts("New Client Connected !");
        fflush(stdout);
        /** server and client data send and receive */
        while ((nReceiveLength = ::recv(hClientSocket, pszBuffer, 128, 0)) > 0) {
            /** server send to client */
            ::send(hClientSocket, pszBuffer, 128, 0);
            /** console show receive data */
            printf("Server receive from client : %s\r\n", pszBuffer);
            /** buffer 초기화 */
            memset(pszBuffer, '\0', sizeof(pszBuffer));
        }
        /** client data transformation done close */
        ::shutdown(hClientSocket, SD_BOTH);
        /** close client data socket */
        ::closesocket(hClientSocket);
    }

    ::closesocket(hServerSocket);
    /** window socket 인 winsock 자원 반환 */
    WSACleanup();
    return 0;
}