#include <iostream>
#include <WinSock2.h>
#include <Windows.h>

void ErrorHandle(const char *msg);


char g_szRemoteAddress[32];
int g_nRemotePort;
int g_nLocalPort;

/** 원격지로 메시지를 전소어하는 스레드 함수 */
DWORD WINAPI ThreadSendto(LPVOID pParam);

int main(int argc, char **argv) {
    WSADATA wsaData = {0,};
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandle("WINDOW Socket create failed...");
    }
    /** 소켓 생성 */
    SOCKET hSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (hSocket == INVALID_SOCKET) {
        ErrorHandle("[main] UDP Socket Create Failed...");
    }
    /** 원격지 IP 와 포트 정보, 로컬 포트 정보를 입력 받는다.*/
    printf("원격지 IP주소를 입력하세요.: ");
    gets_s(g_szRemoteAddress);
    fflush(stdin);
    printf("원격지 포트번호를 입력하세요.: ");
    scanf_s("%d", &g_nRemotePort);
    fflush(stdin);
    printf("로컬 포트번호를 입력하세요.: ");
    scanf_s("%d", &g_nLocalPort);

    /** socket 에 연결하기 위한 포트 바인딩 */
    SOCKADDR_IN addr = {0,};
    addr.sin_family = PF_INET;
    /** 어떤 아이피에서 들어올지 모르기 때문에 */
    addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(g_nLocalPort);
    if (::bind(hSocket, (SOCKADDR *) &addr, sizeof(addr)) == SOCKET_ERROR) {
        ErrorHandle("socket IP and Port Binding Failed...");
    }
    /** 송신 Thread 생성 */
    DWORD dwThreadID = 0;
    HANDLE hThread = ::CreateThread(
            NULL,
            0,
            ThreadSendto,
            (LPVOID) hSocket,
            0,
            &dwThreadID
    );
    ::CloseHandle(hThread);

    /** 수신 기능 */
    char szBuffer[128];
    SOCKADDR_IN remoteAddr;
    int nLenSock = sizeof(remoteAddr), nResult;
    while ((nResult = ::recvfrom(hSocket, szBuffer, sizeof(szBuffer), 0, (SOCKADDR *) &remoteAddr, &nLenSock)) > 0) {
        printf("-> %s\n", szBuffer);
        memset(szBuffer, 0, sizeof(szBuffer));
    }
    puts("UDP 통신 종료.");
    ::closesocket(hSocket);
    ::WSACleanup();
    return 0;
}

void ErrorHandle(const char *msg) {
    fputs(msg, stderr);
    fputc('\r\n', stderr);
    fflush(stderr);
    exit(1);
}


DWORD WINAPI ThreadSendto(LPVOID pParam) {
    /** UDP를 이용한 전송을 할 때에는 따로 연결 상태를 관리하지 않기 때문에 전송할 때마다 소켓을 생성해야 한다. */
    SOCKET hSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (hSocket == INVALID_SOCKET) {
        ErrorHandle("UDP Socket Create Failed...");
    }
    /** 전송할 데이터를 담아줄 Buffer */
    char szBuffer[128];
    /** 원격지 Peer 에 대한 정보를 담을 구조체 */
    SOCKADDR_IN remoteAddr = {0,};
    remoteAddr.sin_family = PF_INET;
    remoteAddr.sin_addr.S_un.S_addr = inet_addr(g_szRemoteAddress);
    remoteAddr.sin_port = htons(g_nRemotePort);

    while (true) {
        /** 사용자로부터 입력 받기 */
        gets_s(szBuffer);
        if (strcmp(szBuffer, "EXIT") == 0) {
            break;
        }
        /** UDP 전송 */
        ::sendto(hSocket, szBuffer, strlen(szBuffer) + 1, 0, (SOCKADDR *) &remoteAddr, sizeof(remoteAddr));
    }
    /** 수신을 위한 UDP 소켓을 닫는다. */
    ::closesocket((SOCKET) pParam);
    ::closesocket(hSocket);
    return 0;
}