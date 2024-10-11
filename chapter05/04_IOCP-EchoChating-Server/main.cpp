#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include "AppProtocol.h"
#include <list>
#include <iterator>

/** Thread 동기화 객체 */
CRITICAL_SECTION g_cs;
// 연결된 클라이언트 관리를 위한 소켓 리스트
std::list<SOCKET> g_listClient;
// 서버 주소 접근을 위한 SOCKET
SOCKET g_hSocket;
/** IOCP 객체 관리를 위한 HANDLE */
HANDLE g_hIocp;

/** 연결된 클라이언트 모두에게 전송하는 함수 */
void SendMessageAll(const char *pszMsg, int nSize);

/** 연결된 모든 클라이언트 및 서버 소켓 종료 함수 */
void CloseAll();

/** Client 해제 함수 */
void CloseClient(SOCKET hSocket);

/** 서버 종료 함수 */
void ReleaseServer(void);

/** Console 에 이벤트 처리 함수 */
BOOL CtrlHandler(DWORD dwType);

/** 처리 Thread Function */
DWORD WINAPI ThreadComplete(LPVOID pParam);

/** 서버 연결 처리 Thread Function */
DWORD WINAPI ThreadAcceptLoop(LPVOID pParam);

int main(int argc, char **argv) {
    /** WIN SOCK 초기화 */
    WSADATA wsa = {0,};
    if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        puts("ERROR : 윈도우 소켓 초기화 실패 ");
        return 0;
    }

    /** Critical Section 초기화 */
    ::InitializeCriticalSection(&g_cs);
    /** Console Event 처리 함수 등록 */
    if (::SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler, TRUE) == FALSE) {
        puts("ERROR : Ctrl + C 처리를 등록할 수 없습니다.");
    }

    /** IOCP 생성 */
    /** IOCP 객체에 대해서 생성 */
    g_hIocp = ::CreateIoCompletionPort(
            INVALID_HANDLE_VALUE, // 등록을 하는 것이 아니라 관리를 위한 객체 생성이므로 INVALID_HANDLE_VALUE를 넣어준다.
            NULL, // IOCP에 등록을 하기 위한 것이 아니기 때문에 NULL 값을 인자로 넣어준다.
            0, // 식별자 에 대한 값을 넣어주는 것 -> 관리를 위한 체계를 넣어주는 것이기 때문에 0의 값
            0 // Thread의 갯수를 OS에 맡기기 위한 0의 값을 넣어준다.
    );
    /** IOCP 생성 실패 시 */
    if (g_hIocp == NULL) {
        puts("ERROR: IOCP를 생성할 수 없습니다.!");
        return 0;
    }
    /** IOCP에서 사용할 Thread 미리 생성 */
    HANDLE hThread;
    DWORD dwThreadID;
    for (int i = 0; i < MAX_THREAD_CNT; ++i) {
        dwThreadID = 0;
        hThread = ::CreateThread(
                NULL,
                0,
                ThreadComplete,
                (LPVOID) NULL,
                0,
                &dwThreadID
        );
        ::CloseHandle(hThread);
    }

    /** Server listen 소켓 생성 */
    g_hSocket = ::WSASocketW(
            AF_INET,
            SOCK_STREAM,
            IPPROTO_TCP,
            NULL,
            0,
            WSA_FLAG_OVERLAPPED
    );
    /** bind 하기 위한 설정 */
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(25000);

    if (::bind(g_hSocket, (SOCKADDR *) &serverAddr, sizeof(serverAddr)) == INVALID_SOCKET) {
        puts("ERROR: 포트가 이미 사용 중 입니다.");
        ReleaseServer();
        return 0;
    }

    if (::listen(g_hSocket, SOMAXCONN) == SOCKET_ERROR) {
        puts("ERROR: 리슨 상태로 전환할 수 없습니다.");
        ReleaseServer();
        return 0;
    }
    /** 연결 처리를 위한 처리 Thread 생성 */
    hThread = ::CreateThread(
            NULL,
            0,
            ThreadAcceptLoop,
            (LPVOID) NULL,
            0,
            &dwThreadID
    );
    ::CloseHandle(hThread);

    puts("***채팅 서버를 시작 합니다!***");
    //_tmain() 함수가 반환하지 않도록 대기한다.
    while (1) {
        getchar();
    }

    return 0;
}


/** 연결된 클라이언트 모두에게 전송하는 함수 */
void SendMessageAll(const char *pszMsg, int nSize) {
    std::list<SOCKET>::iterator it;
    /** Thread 동기화를 위한 임계영역 설정 */
    ::EnterCriticalSection(&g_cs);
    for (it = g_listClient.begin(); it != g_listClient.end(); ++it) {
        ::send(*it, pszMsg, nSize, 0);
    }
    /** 임계영역 Lock 해제 */
    ::LeaveCriticalSection(&g_cs);
}

/** 연결된 모든 클라이언트 및 서버 소켓 종료 함수 */
void CloseAll() {
    std::list<SOCKET>::iterator it;
    ::EnterCriticalSection(&g_cs);
    for (it = g_listClient.begin(); it != g_listClient.end(); ++it) {
        ::shutdown(*it, SD_BOTH);
        ::closesocket(*it);
    }
    ::LeaveCriticalSection(&g_cs);
}

/** Client 해제 함수 */
void CloseClient(SOCKET hSocket) {
    ::shutdown(hSocket, SD_BOTH);
    ::closesocket(hSocket);

    ::EnterCriticalSection(&g_cs);
    g_listClient.push_back(hSocket);
    ::LeaveCriticalSection(&g_cs);
}

/** 서버 종료 함수 */
void ReleaseServer(void) {
    /** client 연결 종료 */
    CloseAll();
    ::Sleep(500);

    /** Listen socket 종료 */
    ::shutdown(g_hSocket, SD_BOTH);
    ::closesocket(g_hSocket);
    g_hSocket = NULL;

    /**
     * IOCP 핸들을 닫는다.
     * IOCP 핸들을 당아주면 GQCS 함수가 FALSE 를 반환하면서
     * GetLastError에서 ERROR_ABANDONED_WAIT_0를 반환한다.
     * IOCP에 대한 쓰레드가 모두 종료가 된다.
     * */
    ::CloseHandle(g_hIocp);
    g_hIocp = NULL;

    /** IOCP 종료 대기를 위한 Sleep */
    ::Sleep(500);
    /** Critical Section 에 대한 삭제 */
    ::DeleteCriticalSection(&g_cs);
}

/** Console 에 이벤트 처리 함수 */
BOOL CtrlHandler(DWORD dwType) {
    if (dwType == CTRL_C_EVENT) {
        ReleaseServer();
        puts("***채팅 서버를 종료합니다!***");
        ::WSACleanup();
        exit(0);
        return TRUE;
    }
    return FALSE;
}

/** 처리 Thread Function */
DWORD WINAPI ThreadComplete(LPVOID pParam) {
    DWORD dwTransferredSize = 0;
    DWORD dwFlag = 0;
    USER_SESSION *pSession = NULL;
    LPWSAOVERLAPPED pWol = NULL;
    BOOL bResult;
    puts("[IOCP 작업자 스레드 시작]");

    while (1) {
        /** IOCP Queue에서 요청에 대한 Dequeue를 한다. */
        bResult = ::GetQueuedCompletionStatus(
                g_hIocp, //  Dequeue할 IOCP 에 대한 HANDLE 인자.
                &dwTransferredSize, // 수시한 데이터 크기
                (PULONG_PTR) &pSession, // 수신한 데이터가 저장된 메모리
                &pWol, // OVERLAPPED 객체를 넣어준다. -> 비동기 처리하기 위한 요청
                INFINITE // 이벤트를 무한정 대기
        );
        /** 정상적으로 Dequeue 한 경우 */
        if (bResult == TRUE) {
            // 클라이언트가 소켓을 정상적으로 닫고 연결을 끊은 경우
            if (dwTransferredSize == 0) {
                CloseClient(pSession->hSocket);
                delete pWol;
                delete pSession;
                puts("\tGQCS: 클라이언트가 정상적으로 연경르 종료함");
            }
                // 클라이언트가 보낸 데이터를 수신한 경우
            else {
                SendMessageAll(pSession->buffer, dwTransferredSize);
                memset(pSession->buffer, 0, sizeof(pSession->buffer));

                /** 처리 후 다시 IOCP 에 등록 */
                DWORD dwReceiveSize = 0;
                DWORD dwFlag = 0;
                WSABUF wsaBuffer = {0,};
                wsaBuffer.buf = pSession->buffer;
                wsaBuffer.len = sizeof(pSession->buffer);

                /** 데이터 수신 */
                ::WSARecv(
                        pSession->hSocket, // 클라이언트 소켓 핸들
                        &wsaBuffer, // WSABUF 구조체 배열의 주소
                        1, // 배열 요소의 개수
                        &dwReceiveSize, // 전달 받은 데이터의 크기를 받을 포인터 변수
                        &dwFlag, //
                        pWol,
                        NULL
                );
                /** 대기 상태가 아닌 에러일 경우 */
                if (::WSAGetLastError() != WSA_IO_PENDING) {
                    puts("\tGQCS: ERROR: WSARecv()");
                }

            }
        }
            /** 비정상 상태일 경우 */
        else {
            /** 완료 큐에서 완료 패킷을 꺼내지 못하고 반환한 경우 */
            if (pWol == NULL) {
                /** IOCP 핸들이 닫힌 경우(서버를 종료하는 경우)도 해당이 된다. */
                puts("\tGQCS: IOCP 핸들이 닫혔습니다.");
                break;
            }
                /** client 비정상적으로 종료 및 서버가 먼저 연결 종료 */
            else {
                if (pSession != NULL) {
                    CloseClient(pSession->hSocket);
                    delete pWol;
                    delete pSession;
                }
                puts("\tGQCS: 서버 종료 혹은 비정상적 연결 종료");
            }
        }
    }
    puts("[IOCP 작업자 쓰레드 종료]");
    return 0;
}

/** 서버 연결 처리 Thread Function */
DWORD WINAPI ThreadAcceptLoop(LPVOID pParam) {
    LPWSAOVERLAPPED pWol = NULL;
    DWORD dwReceiveSize, dwFlag;
    USER_SESSION *pNewUser;
    int nAddrSize = sizeof(SOCKADDR);
    WSABUF wsaBuffer;
    SOCKADDR clientAddr;
    SOCKET hClient;
    int nReceiveResult = 0;

    while ((hClient = ::accept(g_hSocket, &clientAddr, &nAddrSize)) != INVALID_SOCKET) {
        puts("새 클라이언트가 연결 되었습니다.");
        ::EnterCriticalSection(&g_cs);
        g_listClient.push_back(hClient);
        ::LeaveCriticalSection(&g_cs);

        /** 새 클라이언트에 대한 세션 객체 생성 */
        pNewUser = new USER_SESSION;
        ::ZeroMemory(pNewUser, sizeof(USER_SESSION));
        pNewUser->hSocket = hClient;

        /** 비동기 수신 처리를 위한 OVERLAPPED 구조체 생성 */
        pWol = new WSAOVERLAPPED;
        ::ZeroMemory(pWol, sizeof(WSAOVERLAPPED));

        /** 연결된 클라이언트 소켓 핸들을 IOCP 에 등록 */
        ::CreateIoCompletionPort(
                (HANDLE) hClient, // IOCP에 등록할 HANDLE 객체
                g_hIocp, // 등록할 IOCP에 대한 HANDLE 객체
                (ULONG_PTR) pNewUser, // 등록 시 사용할 키 값
                0 // OS에서 Thread 생성하도록 설정
        );

        dwReceiveSize = 0;
        dwFlag = 0;
        wsaBuffer.buf = pNewUser->buffer;
        wsaBuffer.len = sizeof(pNewUser->buffer);

        /** client 가 보낸 정보를 비동기 수신 한다. */
        nReceiveResult = ::WSARecv(hClient, &wsaBuffer, 1, &dwReceiveSize, &dwFlag, pWol, NULL);

        if (::WSAGetLastError() != WSA_IO_PENDING) {
            puts("ERROR: ACCEPT WSARecv() != WSA_IO_PENDING");
        }
    }
    return 0;
}
