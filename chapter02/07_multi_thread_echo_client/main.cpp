#include <iostream>
#include <tchar.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <process.h>


using namespace std;


int main(int argc, char **argv) {
    WSAData wsaData = {0,};
    SOCKET hClient;
    SOCKADDR_IN serverAddr = {0,};
    int serverAddrLen = 0;
    char pszBuffer[128] = {0,};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Failed WinSock Initialized..." << endl;
        return -1;
    }

    hClient = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (hClient == INVALID_SOCKET) {
        cout << "Failed Create Socket ..." << endl;
        return -1;
    }

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(25000);

    int isConnected = ::connect(hClient, (SOCKADDR *) &serverAddr, sizeof(serverAddr));

    if (isConnected == SOCKET_ERROR) {
        cout << "Failed Socket connection..." << endl;
        closesocket(hClient);
        return -1;
    }

    while (true) {
        puts("Connected Server!");
        gets_s(pszBuffer, 128);
        if (strcmp(pszBuffer, "EXIT") == 0) {
            cout << "Connection Closed..." << endl;
            break;
        }

        ::send(hClient, pszBuffer, 128, 0);
        memset(pszBuffer, '\0', sizeof(pszBuffer));
        ::recv(hClient, pszBuffer, 128, 0);
        printf("Receive From Server : %s\r\n", pszBuffer);
    }

    ::shutdown(hClient, SD_BOTH);
    ::closesocket(hClient);
    WSACleanup();
    return 0;
}