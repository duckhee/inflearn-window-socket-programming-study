#include <iostream>
#include <memory>
#include <WinSock2.h>
#include <Windows.h>
#include <fcntl.h>

#define BUFFER_MAX                         1024 * 64
#define PATH_BUFFER_SIZE                   1024

void ErrorHandler(const char *message) {
    printf("ERROR : %s\r\n", message);
    ::WSACleanup();
    exit(1);
}

void GetFilePath(const char *name, char *pathBuffer);

int main(int argc, char **argv) {
    WSAData wsaData = {0,};

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandler("ERROR: WSAStartup()!");
        return 0;
    }

    FILE *fp = NULL;
    char filePath[PATH_BUFFER_SIZE] = {0,};
    GetFilePath("Sleep Away.zip", filePath);
    errno_t nResult = fopen_s(&fp, filePath, "rb");
    if (nResult != 0) {
        ErrorHandler("file Open Failed ...");
    }

    SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET) {
        ErrorHandler("server socket created failed...");
    }

    SOCKADDR_IN serverAdder = {0,};
    serverAdder.sin_family = PF_INET;
    serverAdder.sin_port = htons(25000);
    serverAdder.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    if (::bind(hSocket, (SOCKADDR *) &serverAdder, sizeof(serverAdder)) == SOCKET_ERROR) {
        ErrorHandler("server socket bind Failed...");
    }

    if (::listen(hSocket, SOMAXCONN) == SOCKET_ERROR) {
        ErrorHandler("server socket listen Failed...");
    }

    SOCKADDR_IN clientAddr = {0,};
    int clientLen = sizeof(clientAddr);

    SOCKET hClient = ::accept(hSocket, (SOCKADDR *) &clientAddr, &clientLen);

    if (hClient == INVALID_SOCKET) {
        ErrorHandler("client accept Failed...");
    }

    char byBuffer[BUFFER_MAX] = {0,};
    int nRead, nSent, i = 0;

    while ((nRead = fread(byBuffer, sizeof(char), BUFFER_MAX, fp)) > 0) {
        nSent = ::send(hClient, byBuffer, nRead, 0);
        printf("[%04d] 전송한 바이트 : %d\r\n", ++i, nSent);
        fflush(stdout);
    }
    /** 수신 완료 대기 */
    ::Sleep(100);

    ::closesocket(hClient);
    ::closesocket(hSocket);
    puts("클라이언트 연결이 끊겼습니다.");
    /** 파일 닫아주기 */
    fclose(fp);
    WSACleanup();
    return 0;
}

void GetFilePath(const char *name, char *pathBuffer) {
    char tempBuffer[PATH_BUFFER_SIZE] = {0,};
    char *pRemoveStart = nullptr;
    int removeEndIdx = 0;

    ::GetCurrentDirectory(PATH_BUFFER_SIZE, tempBuffer);
    pRemoveStart = strstr(tempBuffer, "\\cmake");
    removeEndIdx = pRemoveStart - tempBuffer;

    memcpy_s(pathBuffer, PATH_BUFFER_SIZE, tempBuffer, removeEndIdx);
    strcat(pathBuffer, "\\chapter04\\01_not-protocol-file-send-server\\");
    strcat(pathBuffer, name);
}