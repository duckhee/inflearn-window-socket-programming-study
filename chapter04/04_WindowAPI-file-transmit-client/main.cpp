#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <Windows.h>
#include <tchar.h>

#define BUFFER_MAX                          1024 * 64
#define PATH_BUFFER_SIZE                    1024

typedef struct _MY_FILE_DATA {
    char szName[_MAX_FNAME]; // file name
    DWORD dwSize; // file size
} MY_FILE_DATA;

void ErrorHandler(const char *msg) {
    printf("ERROR : %s\r\n", msg);
    ::WSACleanup();
    exit(1);
}

int main(int argc, char **argv) {
    WSADATA wsaData = {0,};
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandler("Failed Window Socket Initialized...");
    }

    SOCKET hSock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN serverAddr = {0,};
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(25000);

    if (::connect(hSock, (SOCKADDR *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        ErrorHandler("Connect Server Failed...");
    }
    /** 수신할 파일명, 크기 정보를 먼저 받는다. */
    MY_FILE_DATA fData = {0,};

    if (::recv(hSock, (char *) &fData, sizeof(fData), 0) < sizeof(fData)) {
        ErrorHandler("Failed File Meta Data Receive...");
    }

    puts("*** File Receive Start ***");
    /** 파일 생성 */
    HANDLE hFile = ::CreateFile(
            fData.szName, // 생성할 파일에 대한 이름
            GENERIC_WRITE, // 파일에 대해서 쓰기용으로 열기
            0, // 공유 모드에 대한 설정
            NULL, // 보안 속성에 대해서 설정
            CREATE_ALWAYS, // 파일에 대해서 항상 생성하는 형태로 엵기
            0, // 파일 속성에 대해서 설정
            NULL //
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        ErrorHandler("Failed File Create...");
    }

    char byBuffer[BUFFER_MAX] = {0,}; // 64KB
    int nReceive;
    DWORD dwTotalReceive = 0, dwRead = 0;

    while (dwTotalReceive < fData.dwSize) {
        if ((nReceive = ::recv(hSock, byBuffer, BUFFER_MAX, 0)) > 0) {
            dwTotalReceive += nReceive;
            /** 파일 쓰기 */
            ::WriteFile(
                    hFile, // 쓸 파일에 대한 HANDLE 객체
                    byBuffer, // 파일에 쓸 데이터 버퍼
                    nReceive, // 실제로 쓰기 위한 데이터의 크기
                    &dwRead, // 파일에 쓴 바이트에 대해서 담아주기 위한 변수
                    NULL // 비동기 입출력일 경우 사용
            );
            printf("Receive : %d / %d\r\n", dwTotalReceive, fData.dwSize);
            fflush(stdout);
        } else {
            puts("ERROR: File Receive Failed...");
            break;
        }
    }

    ::CloseHandle(hFile);
    ::closesocket(hSock);
    printf("*** file Receive End***\r\n");

    ::WSACleanup();
    return 0;
}