#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <windows.h>


using namespace std;


int main(int argc, char **argv) {
    /** client socket */
    SOCKET hClientSocket;
    /** server information struct */
    SOCKADDR_IN serverInfo = {0,};
    /** data buffer */
    char pszBuffer[128] = {0,};
    /** WinSocket Initialized */
    WSAData WsaData = {0,};
    if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0) {
        cout << "Failed WinSock Initialized..." << endl;
        return -1;
    }

    /** create socket */
    hClientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    /** check socket */
    if (hClientSocket == INVALID_SOCKET) {
        cout << "Failed Socket Created..." << endl;
        /** win sock resource return os */
        WSACleanup();
        return -1;
    }

    /** setting server information */
    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serverInfo.sin_port = htons(25000);

    int isConnected = ::connect(hClientSocket, (SOCKADDR *) &serverInfo, sizeof(serverInfo));

    if (isConnected == SOCKET_ERROR) {
        cout << "Failed Connected Server..." << endl;
        closesocket(hClientSocket);
        /** win sock resource return os */
        WSACleanup();
        return -1;
    }

    /** server and client data link */
    while (true) {
        gets_s(pszBuffer, 128);
        if (strcmp(pszBuffer, "EXIT") == 0) {
            cout << "Client disconnect server ..." << endl;
            break;
        }
        ::send(hClientSocket, pszBuffer, 128, 0);
        memset(pszBuffer, '\0', sizeof(pszBuffer));

        ::recv(hClientSocket, pszBuffer, 128, 0);
        printf("Server From : %s\r\n", pszBuffer);
    }
    /** close data link */
    ::shutdown(hClientSocket, SD_BOTH);
    ::closesocket(hClientSocket);

    /** win sock resource return os */
    WSACleanup();
    return 0;
}