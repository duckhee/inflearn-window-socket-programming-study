#include <iostream>
#include <WinSock2.h>
#include <fcntl.h>
#include <Windows.h>

#define BUFFER_MAX              65536

void ErrorHandle(const char *msg) {
    printf("ERROR: %s\r\n", msg);
    WSACleanup();
    exit(1);
}


int main(int argc, char **argv) {
    WSAData wsaData = {0,};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Win Socket Error...\r\n");
        return 0;
    }

    SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (hSocket == INVALID_SOCKET) {
        ErrorHandle("socket Create Failed...");
    }

    SOCKADDR_IN serverAddr = {0,};
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(25000);

    if (::connect(hSocket, (SOCKADDR *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        ErrorHandle("connect server Error...");
    }

    puts("*** 파일 수신을 시작합니다.***\r\n");
    FILE *fp = NULL;
    errno_t nResult = fopen_s(&fp, "receiveData.zip", "wb");
    if (nResult != 0) {
        ErrorHandle("File Create Failed...");
    }

    char byBuffer[BUFFER_MAX];
    int nReceive;

    while ((nReceive = ::recv(hSocket, byBuffer, BUFFER_MAX, 0)) > 0) {
        fwrite(byBuffer, nReceive, 1, fp);
        puts("#");
    }

    fclose(fp);
    printf("\r\n*** 파일 송수신 종료***\r\n");
    ::closesocket(hSocket);
    ::WSACleanup();
    return 0;
}