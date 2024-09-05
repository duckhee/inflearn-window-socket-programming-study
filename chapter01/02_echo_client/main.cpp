#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <SDKDDKVer.h>
#include <windows.h>
#include <tchar.h>

#pragma comment(lib, "ws2_32")


int main(int argc, char **argv) {
    char pszBuffer[128] = {0,};

    /** window socket 에 대한 초기화 */
    WSADATA wsa = {0,};
    if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        puts("윈도우 소켓을 초기화할 수 없습니다.");
        return 0;
    }

    /** client socket 생성 */
    SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    /** Socket 생성 상태 확인 -> 소켓이 생성이 안되었을 경우 */
    if (hSocket == INVALID_SOCKET) {
        puts("ERROR : 소켓을 생성할 수 없습니다.");
        return 0;
    }

    /** 접속할 서버 정보를 담을 구조체 */
    SOCKADDR_IN serverAddr = {0,};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(25000);
    /** 접속할 서버 주소 설정 */
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
//    InetPton(AF_INET, _T("127.0.0.1"), &serverAddr.sin_addr.S_un.S_addr);
    /** 서버와 연결 진행 */
    int isConnected = ::connect(
            hSocket, // 연결을 시도할 SOCKET에 대한 인자를 넣어준다.
            (SOCKADDR *) &serverAddr, // 서버 주소를 담고 있는 구조체를 넣어준다.
            sizeof(serverAddr) // 서버 정보를 담고 있는 구조체의 크기를 인자로 넣어준다.
    );
    /** 서버와 연결 실패 시 동작 */
    if (isConnected == SOCKET_ERROR) {
        puts("ERROR: 서버에 연결할 수 없습니다.");
        /** close socket */
        ::closesocket(hSocket);
        return 0;
    }

    /** 데이터 전송 및 수신 */
    while (true) {
        /** 사용자로부터 입력을 받는다. */
        gets_s(pszBuffer);
        /** EXIT 값이 있는지 확인한다. */
        if (strcmp(pszBuffer, "EXIT") == 0) {
            break;
        }
        /** server 에 데이터 전송 */
        ::send(hSocket, pszBuffer, strlen(pszBuffer) + 1, 0);
        /** 데이터 전송 후 버퍼 초기화 */
        memset(pszBuffer, '\0', sizeof(pszBuffer));
        /** server에서 데이터 받기 */
        ::recv(hSocket, pszBuffer, sizeof(pszBuffer), 0);
        printf("From Server : %s\r\n", pszBuffer);
    }

    /** data 전송 중지 통보 */
    ::shutdown(hSocket, SD_BOTH);
    /** socket 반환 */
    ::closesocket(hSocket);
    /** window socket 제거 */
    ::WSACleanup();
    return 0;
}