#include <iostream>
#include <WinSock2.h>
#include <MSWSock.h>
#include <Windows.h>
#include <tchar.h>

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

void GetFilePath(const char *name, char *pathBuffer);

int main(int argc, char **argv) {
    WSADATA wsaData = {0,};
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandler("Failed Initialized Win Socket");
    }
    char filePath[PATH_BUFFER_SIZE] = {0,};
    GetFilePath("Sleep Away.zip", filePath);
    HANDLE hFile = ::CreateFile(
            _T(filePath), // 열 파일에 대한 파일 이름 및 경로
            GENERIC_READ, // 읽기 목적으로 파일 열기 위한 인자
            FILE_SHARE_READ, // 읽기에 대한 공유 허용
            NULL, // 보안 속성에 대해서 상속을 받아서 사용
            OPEN_EXISTING, // 파일이 존재할 경우에 열기
            FILE_FLAG_SEQUENTIAL_SCAN, // 큰 파일을 열 때 성능 향상이 있다. (순차적 접근 파일인 것을 알려주는 값)
            NULL // 해당 Handler에 대한 이름 부여할 때 사용 -> 주로 프로세스로 열 경우 사용
    );
    /** 파일에 대해서 열 때 에러 발생 처리 */
    if (hFile == INVALID_HANDLE_VALUE) {
        ErrorHandler("전송할 파일을 열 수 없습니다.");
    }

    /** Server Socket 생성 */
    SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET) {
        ::CloseHandle(hFile);
        ErrorHandler("Server Socket Create Failed..");
    }

    /** Socket 정보 생성 및 설정 */
    SOCKADDR_IN serverAddr = {0,};
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(25000);

    if (::bind(hSocket, (SOCKADDR *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        ::CloseHandle(hFile);
        ErrorHandler("Socket Bind Error");
    }

    if (::listen(hSocket, SOMAXCONN) == SOCKET_ERROR) {
        ::CloseHandle(hFile);
        ErrorHandler("Failed Socket Listen...");
    }

    SOCKADDR_IN clientAddr = {0,};
    int clientAddrLen = sizeof(clientAddr);

    SOCKET hClient = ::accept(hSocket, (SOCKADDR *) &clientAddr, &clientAddrLen);

    if (hClient == INVALID_SOCKET) {
        ::CloseHandle(hFile);
        ErrorHandler("Failed Accept Client...");
    }
    MY_FILE_DATA fData = {"Sleep Away.zip", 0};
    fData.dwSize = ::GetFileSize(hFile, NULL);
    /** 파일 전송에 사용할 버퍼 생성 */
    TRANSMIT_FILE_BUFFERS tfb = {0,};
    /** 파일 전송 전에 전달할 데이터 설정 */
    tfb.Head = &fData;
    tfb.HeadLength = sizeof(fData);

    /** Win API File Send */
    BOOL transmitFlag = ::TransmitFile(
            hClient, // 전달할 SOCKET
            hFile, // 전달할 파일 HANDLE
            0,  // 전달할 파일의 사이즈 0의 값을 했을 때 전체 전달
            1024 * 64, // 헌번에 전달할 버퍼 크기
            NULL, // 비동기 IO 방식 사용 시에 OVERLAPPED 객체를 넣어주면 된다,
            &tfb, // 파일 전송 전에 전달할 정보
            0 // 기타 옵션에 대한 값
    );

    if (transmitFlag == FALSE) {
        ::CloseHandle(hFile);
        ErrorHandler("File Transmit Failed...");
    }
    /** client 연결 종료 시 */
    ::recv(hClient, NULL, 0, 0);
    puts("Client Connection Closed...");
    ::closesocket(hClient);
    ::closesocket(hSocket);
    ::CloseHandle(hFile);
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
    strcat(pathBuffer, "\\chapter04\\03_WindowAPI-file-transmit-server\\");
    strcat(pathBuffer, name);
}