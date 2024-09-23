#include <iostream>
#include <WinSock2.h>
#include <tchar.h>
#include <Windows.h>
#include <string>
#include <process.h>
#include <list>

#define BUFFER_MAX                  1024

SOCKET g_hServerSocket;
std::list<SOCKET> g_listClient;

void SendMessageBrodCast(char *pszMsg, int nSize);

void CloseAllClient();

BOOL consoleHandler(DWORD dwType);

int main(int argc, char **argv) {
    WSAData wsaData;
    SOCKADDR_IN serverAddr = {0,};

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Failed Win Socket Initialized..." << std::endl;
        return -1;
    }

    /** console handler setting */
    if (::SetConsoleCtrlHandler((PHANDLER_ROUTINE) consoleHandler, TRUE) == FALSE) {
        puts("ERROR: Ctrl + C 처리에 대한 Handler 등록할 수 없습니다.");
    }

    /** 접속 대기 소켓 생성 */
    g_hServerSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (g_hServerSocket == INVALID_SOCKET) {
        puts("ERROR: 접속 대기 소켓을 생성할 수 없습니다.");
        return -1;
    }

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(25000);
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    if (::bind(g_hServerSocket, (SOCKADDR *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        puts("ERROR: 소켓에 주소를 바인드할 수 없습니다.");
        return 0;
    }

    if (::listen(g_hServerSocket, SOMAXCONN) == SOCKET_ERROR) {
        puts("ERROR: 리슨 상태로 전환할 수 없습니다.");
        return -1;
    }

    /** socket을 한번에 관리하기 위해서 list 자료형에 넣어주기 */
    g_listClient.push_back(g_hServerSocket);

    /** 변화 감지를 위한 변수들 */
    UINT nCount;
    FD_SET fdRead;
    std::list<SOCKET>::iterator it;

    puts("I/O Multi Plexing Chatting Server Strat");

    do {
        /** File Descriptor Initialized */
        FD_ZERO(&fdRead);
        for (it = g_listClient.begin(); it != g_listClient.end(); ++it) {
            FD_SET(*it, &fdRead);
        }
        /** 변화가 발생할 때까지 대기 */
        ::select(
                0,
                &fdRead,
                NULL,
                NULL,
                NULL
        );
        /** 변화가 감지된 소켓 셋 확인 */
        nCount = fdRead.fd_count;
        for (int nIndex = 0; nIndex < nCount; ++nIndex) {
            /** socket에 대한 변화 감시 플레그 확인 */
            if (!FD_ISSET(fdRead.fd_array[nIndex], &fdRead)) {
                continue;
            }
            /** 서버에 연결 요청일 경우 처리 */
            if (fdRead.fd_array[nIndex] == g_hServerSocket) {
                SOCKADDR_IN clientAddr = {0,};
                int clientAddrSize = sizeof(clientAddr);
                SOCKET hClientSocket = ::accept(g_hServerSocket, (SOCKADDR *) &clientAddr, &clientAddrSize);
                if (hClientSocket != INVALID_SOCKET) {
                    FD_SET(hClientSocket, &fdRead);
                    g_listClient.push_back(hClientSocket);
                }
            }
                /** client 전송 데이터 처리인 경우 */
            else {
                char pszBuffer[BUFFER_MAX] = {0,};
                int nReceive = ::recv(fdRead.fd_array[nIndex], (char *) pszBuffer, sizeof(pszBuffer), 0);
                /** 연결 종료 시 */
                if (nReceive <= 0) {
                    ::closesocket(fdRead.fd_array[nIndex]);
                    /** FD SET 에서 제거*/
                    FD_CLR(fdRead.fd_array[nIndex], &fdRead);
                    g_listClient.remove(fdRead.fd_array[nIndex]);
                    puts("client Connection Closed...");
                }
                    /** message 전송 */
                else {
                    SendMessageBrodCast(pszBuffer, nReceive);
                }
            }
        }
    } while (g_hServerSocket != NULL);

    CloseAllClient();
    puts("Server Closed...");
    return 0;
}


void SendMessageBrodCast(char *pszMsg, int nSize) {
    std::list<SOCKET>::iterator it;
    for (it = g_listClient.begin(); it != g_listClient.end(); ++it) {
        ::send(*it, pszMsg, nSize, 0);
    }
}

void CloseAllClient() {
    std::list<SOCKET>::iterator it;

    for (it = g_listClient.begin(); it != g_listClient.end(); ++it) {
        ::shutdown(*it, SD_BOTH);
        ::closesocket(*it);
    }
}

BOOL consoleHandler(DWORD dwType) {
    if (dwType == CTRL_C_EVENT) {
        ::closesocket(g_hServerSocket);
        ::CloseAllClient();
        puts("All Client Close ...");

        ::WSACleanup();
        exit(0);
    }
    return FALSE;
}

