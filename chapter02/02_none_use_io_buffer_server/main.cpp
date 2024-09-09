#include <iostream>
#include <WinSock2.h>

#include <Windows.h>

using namespace std;

int main(int argc, char **argv) {
    WSAData wsa = {0,};
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "Failed Window Socket Initialized ..." << endl;
        return -1;
    }
    /** 접속에 대한 요청을 처리할 socket 변수 및 생성 */
    SOCKET hServerSocket = {0,};
    hServerSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    /** 소켓 생성 확인 */
    if (hServerSocket == INVALID_SOCKET) {
        cout << "Failed Socket Created..." << endl;
        return -1;
    }
    SOCKADDR_IN serverInfo = {0,};
    serverInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    serverInfo.sin_port = htons(25000);
    serverInfo.sin_family = PF_INET;

    /** socket bind host network adapter */
    int isBind = ::bind(hServerSocket, (SOCKADDR *) &serverInfo, sizeof(serverInfo));

    if (isBind == SOCKET_ERROR) {
        cout << "Failed Bind Socket ..." << endl;
        return -1;
    }

    /** socket listening */
    int isListening = ::listen(hServerSocket, SOMAXCONN);

    if (isListening == SOCKET_ERROR) {
        cout << "Failed Listening socket ..." << endl;
        return -1;
    }

    std::cout << "Server Socket Listening!" << std::endl;
    SOCKADDR_IN clientInfo = {0,};
    int clientInfoSize = sizeof(clientInfo);
    SOCKET hClient = 0;

    /** receive data temp buffer */
    char pszBuffer[128] = {0,};
    /** receive data size */
    int nReceive = 0;


    /** client connect */
    while ((hClient = ::accept(hServerSocket, (SOCKADDR *) &clientInfo, &clientInfoSize)) != INVALID_SOCKET) {
        puts("Client Connected!");
        fflush(stdout);
        while ((nReceive = ::recv(hClient, pszBuffer, sizeof(pszBuffer), 0)) > 0) {
            /** 데이터 전송 */
            ::send(hClient, pszBuffer, 128, 0);
            puts(pszBuffer);
            memset(pszBuffer, '\0', sizeof(pszBuffer));
        }
        /** client send receive socket closed */
        ::shutdown(hClient, SD_BOTH);
        /** client socket closed */
        ::closesocket(hClient);
    }

    /** server listening socket closed */
    ::closesocket(hServerSocket);

    WSACleanup();
    return 0;
}