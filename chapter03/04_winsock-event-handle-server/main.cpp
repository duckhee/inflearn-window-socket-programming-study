#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <list>

using namespace std;

SOCKET g_hServerSocket;


int g_nListIndex;

WSAEVENT g_aListEvent[WSA_MAXIMUM_WAIT_EVENTS];
SOCKET g_aListSocket[WSA_MAXIMUM_WAIT_EVENTS];

void CloseAll();

BOOL ConsoleHandler(DWORD dwType);

int main(int argc, char **argv) {
    WSAData wsaData = {0,};
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        puts("ERROR: Win socket Initialized...");
        return 0;
    }

    if (::SetConsoleCtrlHandler((PHANDLER_ROUTINE) ConsoleHandler, TRUE) == FALSE) {
        puts("ERROR: Ctrl + C Event Add Failed...");
        return 0;
    }

    g_hServerSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (g_hServerSocket == INVALID_SOCKET) {
        puts("ERROR: Create Server Socket Failed...");
        return 0;
    }

    SOCKADDR_IN serverAddr = {0,};
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(25000);

    int isBind = ::bind(g_hServerSocket, (SOCKADDR *) &serverAddr, sizeof(serverAddr));
    if (isBind == SOCKET_ERROR) {
        puts("ERROR: bind() Failed...");
        return 0;
    }

    int isListened = ::listen(g_hServerSocket, SOMAXCONN);
    if (isListened == SOCKET_ERROR) {
        puts("ERROR: listen() Failed...");
        return 0;
    }

    g_nListIndex = 0;
    g_aListSocket[g_nListIndex] = g_hServerSocket;
    g_aListEvent[g_nListIndex] = ::WSACreateEvent();

    if (::WSAEventSelect(g_hServerSocket, g_aListEvent[g_nListIndex], FD_ACCEPT) == SOCKET_ERROR) {
        puts("ERROR: WSAEventSelect() Failed...");
        return 0;
    }

    puts("WSA Event Multi plexing Server Start!");
    DWORD dwIndex;
    /** network event 에 대한 이벤트를 담기 위한 변수 */
    WSANETWORKEVENTS netEvent;
    while (TRUE) {
        /** WSAEvent 에 대한 변화 감지하는 함수 -> 변화가 일어난 이벤트의 index를 반환을 한다. */
        dwIndex = ::WSAWaitForMultipleEvents(
                g_nListIndex + 1, // 감시할 이벤트 개수
                g_aListEvent, // 이벤트를 담고 있는 배열
                FALSE, // 모든 이벤트가 발행이 될때까지 대기하지 않기 위한 값
                100, // 이베트 감지를 대기할 시간
                FALSE // 호출자 스레드 값을 변경하지 않기 위한 값
        );
        /** 실패한 값이 대기가 실패할 경우 */
        if (dwIndex == WSA_WAIT_FAILED) {
            continue;
        }
        /** 이벤트가 발생한 소켓의 인덱스 및 이벤트 발생한 이유를 확인하기 위한 함수 */
        if (::WSAEnumNetworkEvents(g_aListSocket[dwIndex], g_aListEvent[dwIndex], &netEvent) == SOCKET_ERROR) {
            continue;
        }
        /** 연결 시도일 경우 */
        if (netEvent.lNetworkEvents & FD_ACCEPT) {
            /** 에러가 있을 경우 */
            if (netEvent.iErrorCode[FD_ACCEPT_BIT] != 0) {
                continue;
            }
            /** 최대 연결 가능한 클라이언트 수 넘어갔을 경우 */
            if (g_nListIndex >= WSA_MAXIMUM_WAIT_EVENTS) {
                puts("ERROR: 최대 접속 클라인트 초과 !");
                continue;
            }

            SOCKADDR_IN clientAddr = {0,};
            int nAddrLen = sizeof(clientAddr);
            SOCKET hClient = ::accept(g_hServerSocket, (SOCKADDR *) &clientAddr, &nAddrLen);
            if (hClient != INVALID_SOCKET) {
                ++g_nListIndex;
                g_aListSocket[g_nListIndex] = hClient;
                g_aListEvent[g_nListIndex] = ::WSACreateEvent();
                puts("새 클라리언트가 연결 되었습니다.");
            }
            /** 생성된 client socket 에 대해서 감지할 이벤트 등록 */
            ::WSAEventSelect(hClient, g_aListEvent[g_nListIndex], FD_READ | FD_CLOSE);
        }
            /** client 연결 해제 이벤트 경우 */
        else if (netEvent.lNetworkEvents & FD_CLOSE) {
            ::WSACloseEvent(g_aListEvent[dwIndex]);
            ::shutdown(g_aListSocket[dwIndex], SD_BOTH);
            ::closesocket(g_aListSocket[dwIndex]);

            /** 당은 소켓 정리 및 배열 정리 */
            for (DWORD i = dwIndex; i < g_nListIndex; ++i) {
                g_aListEvent[i] = g_aListEvent[i + 1];
                g_aListSocket[i] = g_aListSocket[i + 1];
            }
            g_nListIndex--;
            printf("클라이언트 연결 종료. 연결 가능한 client 수 : %d.\r\n", (WSA_MAXIMUM_WAIT_EVENTS - g_nListIndex));
        }
            /** 데이터 전송 이벤트 */
        else if (netEvent.lNetworkEvents & FD_READ) {
            TCHAR pszBuffer[1024] = {0,};
            int nReceiveLen = ::recv(g_aListSocket[dwIndex], (char *) pszBuffer, sizeof(pszBuffer), 0);

            /** 전체 클라이언트에게 메세지 전송 */
            for (DWORD i = 1; i < g_nListIndex; ++i) {
                ::send(g_aListSocket[i], (char *) pszBuffer, nReceiveLen, 0);
            }
        }
    }
    CloseAll();

    WSACleanup();
    return 0;
}


void CloseAll() {
    for (int i = 0; i < g_nListIndex; ++i) {
        ::shutdown(g_aListSocket[i], SD_BOTH);
        ::closesocket(g_aListSocket[i]);
        ::WSACloseEvent(g_aListEvent[i]);
    }
}

BOOL ConsoleHandler(DWORD dwType) {
    if (dwType == CTRL_C_EVENT) {
        CloseAll();
        puts("Close All Client");

        ::WSACleanup();
        exit(0);
        return TRUE;
    }
    return FALSE;
}