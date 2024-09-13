#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <windows.h>
#include <process.h>
#include <string.h>

using namespace std;

UINT WINAPI ChattingReceiveHandler(LPVOID pParam);

int main(int argc, char **argv) {
    WSAData wsaData;
    SOCKET hClientSocket;
    SOCKADDR_IN serverAddr = {0,};
    HANDLE hThread;
    UINT threadID;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Failed Window Socket Initialized..." << endl;
        return -1;
    }

    hClientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(25000);

    int isConnected = ::connect(hClientSocket, (SOCKADDR *) &serverAddr, sizeof(serverAddr));
    if (isConnected == SOCKET_ERROR) {
        cout << "Failed Connected..." << endl;
        closesocket(hClientSocket);
        return -1;
    }
    /** 데이터 수신을 위한 worker thread */
    hThread = (HANDLE) ::_beginthreadex(
            NULL,
            0,
            ChattingReceiveHandler,
            (LPVOID) hClientSocket,
            0,
            &threadID
    );

    ::CloseHandle(hThread);
    char pszBuffer[128] = {0,};
    puts("Connected Server!");
    /** 사용자 입력에 대해서 처리 */
    while (true) {
        memset(pszBuffer, '\0', sizeof(pszBuffer));
        gets_s(pszBuffer);
        if (strcmp(pszBuffer, "EXIT") == 0) {
            break;
        }
        ::send(hClientSocket, pszBuffer, strlen(pszBuffer) + 1, 0);
    }
//    ::shutdown(hClientSocket, SD_BOTH);
    ::closesocket(hClientSocket);
    ::Sleep(100);
    WSACleanup();
    return 0;
}


UINT WINAPI ChattingReceiveHandler(LPVOID pParam) {
    SOCKET hClient = (SOCKET) pParam;
    char szBuffer[128] = {0,};
    while (::recv(hClient, szBuffer, sizeof(szBuffer), 0) > 0) {
        printf(" -> %s\r\n", szBuffer);
        memset(szBuffer, 0, sizeof(szBuffer));
    }
    puts("Close Receive Data Socket...");
    return 0;
}