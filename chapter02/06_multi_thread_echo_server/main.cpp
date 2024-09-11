#include <iostream>
#include <tchar.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <Process.h>

using namespace std;

/** client 와 데이토 통신할 Thread */
UINT WINAPI EchoServiceThread(LPVOID pParam);

int main(int argc, char **argv) {
    WSADATA wsaData = {0,};

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Failed Win Sock Initialized Failed..." << endl;
        return -1;
    }


    SOCKET hServer = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hServer == INVALID_SOCKET) {
        cout << "Failed Create Server Socket..." << endl;
        return -1;
    }
    SOCKADDR_IN serverAddr = {0,};
//    InetPton(PF_INET, _T("127.0.0.1"), &serverAddr.sin_addr);
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(25000);

    int isBind = ::bind(hServer, (SOCKADDR *) &serverAddr, sizeof(serverAddr));
    if (isBind == SOCKET_ERROR) {
        cout << "Failed Bind server socket..." << endl;
        closesocket(hServer);
        return -1;
    }

    int isListened = ::listen(hServer, SOMAXCONN);

    if (isListened == SOCKET_ERROR) {
        cout << "Failed Listen server socket..." << endl;
        closesocket(hServer);
        return -1;
    }

    SOCKET hClient;
    SOCKADDR_IN clientAddr = {0,};
    int clientAddrLen = sizeof(clientAddr);
    HANDLE hThread;
    UINT dwThreadID = 0;

    while ((hClient = ::accept(hServer,
                               (SOCKADDR *) &clientAddr,
                               &clientAddrLen)) != INVALID_SOCKET) {
        /** Thread 생성 */
        hThread = (HANDLE) ::_beginthreadex(
                NULL, // 보안 속성 상속을 받기 위한 NULL 값 정의
                0, // 스택에 대한 기본 메모리 크기로 사용하기 위해서 0 값 대입 -> 1MB 이 기본 값
                EchoServiceThread, // Thread에서 실행할 함수에 대한 포인터
                (LPVOID) hClient, // Thread에 넘겨줄 파라미터
                0, // 초기 상태에 대한 설정
                &dwThreadID // Thread에 대한 ID 값 저장할 변수
        );
        /** Thread 회수 및 자원 반납 */
        ::CloseHandle(hThread);
    }

    ::closesocket(hServer);

    WSACleanup();
    return 0;
}


UINT WINAPI EchoServiceThread(LPVOID pParam) {
    char pszBuffer[128] = {0,};
    int nReceive = 0;
    /** socket 에 대한 형 변환 */
    SOCKET clientSocket = (SOCKET) pParam;
    puts("new Client Connected!");

    while ((nReceive = ::recv(clientSocket, pszBuffer, 128, 0)) > 0) {
        ::send(clientSocket, pszBuffer, sizeof(pszBuffer), 0);
        puts(pszBuffer);
        memset(pszBuffer, 0, sizeof(pszBuffer));
    }
    ::closesocket(clientSocket);
    return 0;
}