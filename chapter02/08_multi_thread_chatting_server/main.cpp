#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <windows.h>
#include <Process.h>
#include <list>

using namespace std;

/** 임계 구간 설정을 위한 CRITICAL_SECTION */
CRITICAL_SECTION g_cs;
/** 연결을 담당할 SOCKET */
SOCKET hServerSocket;
/** 데이터 전송을 위한 사용자 관리 Linked List */
std::list<SOCKET> g_clientList;

BOOL AddUser(SOCKET clientSocket);

void SendChattingMsg(char *pszParam);

UINT WINAPI ClientHandler(LPVOID pParam);

BOOL CtrlHandler(DWORD dwType);

void ShutDown();

int main(int argc, char **argv) {
    WSADATA wsaData = {0,};
    /** 윈도우 소켓 초기화 */
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "Failed Window Socket Initialized..." << endl;
        return -1;
    }

    /** Critical section 에 대한 초기화 */
    ::InitializeCriticalSection(&g_cs);

    /** console 에 키 이벤트 등록 */
    if (::SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE) == FALSE) {
        puts("ERROR: Ctrl + C Key Event Registered Failed...");
    }

    /** 연결을 위한 socket 생성 */
    hServerSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hServerSocket == INVALID_SOCKET) {
        cout << "Failed Create Server Socket..." << endl;
        return -1;
    }

    SOCKADDR_IN serverAddr = {0,};
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(25000);
    InetPton(PF_INET, _T("127.0.0.1"), &serverAddr.sin_addr);
//    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    int isBind = ::bind(hServerSocket, (SOCKADDR *) &serverAddr, sizeof(serverAddr));
    if (isBind == SOCKET_ERROR) {
        cout << "Failed Bind Socket Error..." << endl;
        closesocket(hServerSocket);
        return -1;
    }

    int isListened = ::listen(hServerSocket, SOMAXCONN);
    if (isListened == SOCKET_ERROR) {
        cout << "Failed Server Listened..." << endl;
        closesocket(hServerSocket);
        return -1;
    }

    SOCKADDR_IN clientAddr = {0,};
    int clientAddrLen = sizeof(clientAddr);
    HANDLE hThread;
    UINT threadID = 0;
    SOCKET hClient;

    while ((hClient = ::accept(hServerSocket, (SOCKADDR *) &clientAddr, &clientAddrLen)) != INVALID_SOCKET) {
        if (AddUser(hClient) == FALSE) {
            puts("ERROR: Connected User Failed");
            CtrlHandler(CTRL_C_EVENT);
            break;
        }
        hThread = (HANDLE) ::_beginthreadex(
                NULL,
                0,
                ClientHandler,
                (LPVOID) hClient,
                0,
                &threadID
        );
        ::CloseHandle(hThread);
    }

    ::puts("Close Chatting Server");
    ::closesocket(hServerSocket);
    ShutDown();
    /** 윈도우 소켓 자원 봔환 */
    WSACleanup();
    return 0;
}


BOOL AddUser(SOCKET clientSocket) {
    /** 임계 구간 설정 */
    ::EnterCriticalSection(&g_cs);
    /** 자료 구조에 데이터 삽입 */
    g_clientList.push_back(clientSocket);
    /** 임계 구간 해제 */
    ::LeaveCriticalSection(&g_cs);

    return TRUE;
}


void SendChattingMsg(char *pszParam) {
    /** 문자열에 대한 길이 가져오기 */
    int nLength = sizeof(pszParam);
    /** list를 순회하기 위한 객체 변수 생성 */
    std::list<SOCKET>::iterator it;
    /** 자료 구조를 순회하기 전에 임계 구간 설정 */
    ::EnterCriticalSection(&g_cs);
    for (it = g_clientList.begin(); it != g_clientList.end(); ++it) {
        ::send(*it, pszParam, sizeof(char) * (nLength + 1), 0);
    }
    /** 임계 구간 해제 */
    ::LeaveCriticalSection(&g_cs);
}

UINT WINAPI ClientHandler(LPVOID pParam) {
    SOCKET hClientSocket = (SOCKET) pParam;
    char pszBuffer[128] = {0,};
    int nReceive = 0;

    puts("New Client Connected!");
    while ((nReceive = ::recv(hClientSocket, pszBuffer, sizeof(pszBuffer), 0)) > 0) {
        puts(pszBuffer);
        ::SendChattingMsg(pszBuffer);
        memset(pszBuffer, 0, sizeof(pszBuffer));
    }
    puts("Client Closed...");
    /** 특정 Client 삭제 시 자료구조인 Linked List에 접근하기 때문에 CRITICAL_SECTION 에 대한 임계구간 설정 */
    ::EnterCriticalSection(&g_cs);
    g_clientList.remove(hClientSocket);
    ::LeaveCriticalSection(&g_cs);
    ::closesocket(hClientSocket);
    return 0;
}

void ShutDown() {
    std::list<SOCKET>::iterator it;
    ::shutdown(hServerSocket, SD_BOTH);
    ::EnterCriticalSection(&g_cs);
    for (it = g_clientList.begin(); it != g_clientList.end(); ++it) {
        closesocket(*it);
    }
    /** list 초기화 */
    g_clientList.clear();
    ::LeaveCriticalSection(&g_cs);
    puts("Client All Closed");
    /** 클라이언트 종료 대기 */
    ::Sleep(100);
    /** CRITICAL_SECTION 제거 */
    ::DeleteCriticalSection(&g_cs);
    /** 연결 소켓 제거 */
    ::closesocket(hServerSocket);
}

/** 종료 관련 코드 특정 키인 CTRL + C 가 입력 시 동작 */
BOOL CtrlHandler(DWORD dwType) {
    if (dwType == CTRL_C_EVENT) {
        std::list<SOCKET>::iterator it;
        ::shutdown(hServerSocket, SD_BOTH);
        ::EnterCriticalSection(&g_cs);
        for (it = g_clientList.begin(); it != g_clientList.end(); ++it) {
            closesocket(*it);
        }
        g_clientList.clear();
        ::LeaveCriticalSection(&g_cs);

        puts("All Client Closed...");
        ::Sleep(100);
        ::DeleteCriticalSection(&g_cs);
        ::closesocket(hServerSocket);
        ::WSACleanup();
        exit(0);
        return 0;
    }
    return FALSE;
}