#include <iostream>
#include <WinSock2.h>
#include <Windows.h>

void ErrorHandler(const char *msg);

int main(int argc, char **argv) {
    WSADATA wsaData = {0,};
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) == 0) {
        ErrorHandler("Failed Window Socket");
    }

    SOCKET hSocket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (hSocket == INVALID_SOCKET) {
        ErrorHandler("UDP Socket Create Failed...");
    }

    SOCKADDR_IN addr = {0,};
    addr.sin_family = PF_INET;
    // broad cast로 전송하기 위한 주소를 설정
    addr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST);
    addr.sin_port = htons(2500);

    /** Broad Cast 로 전송하기 위해서는 소켓에 대한 옵션을 변경해야 한다. */
    int nOption = 1;
    ::setsockopt(
            hSocket,
            SOL_SOCKET,
            SO_BROADCAST,
            (const char *) &nOption,
            sizeof(nOption)
    );

    char szBuffer[128] = {0,};
    while (1) {
        putchar('>');
        gets_s(szBuffer);
        if (strcmp(szBuffer, "EXIT") == 0) {
            break;
        }
        // 방송 주소로 메세지 전송
        // 모든 Peer들이 동시에 메시지를 수신한다.
        ::sendto(hSocket, szBuffer, sizeof(szBuffer), 0, (const SOCKADDR *) &addr, sizeof(addr));
    }
    ::closesocket(hSocket);
    ::WSACleanup();
    return 0;
}


void ErrorHandler(const char *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    fflush(stderr);
    exit(1);
}