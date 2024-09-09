#include <iostream>
#include <WinSock2.h>
#include <Windows.h>


#pragma comment(lib, "ws2_32")

int main(int argc, char **argv) {
    /** 윈도우 소켓을 사용할 시에 초기화를 위한 구조체 */
    WSADATA wsa = {0,};
    /** window socket 초기화 실패 시 */
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("윈도우 소켓 초기화 실패 했습니다.\r\n");
        return -1;
    }
    /** socket 생성 -> TCP 소켓 생성 */
    SOCKET hSocket = ::socket(
            AF_INET,
            SOCK_STREAM,
            IPPROTO_TCP
    );

    /** socket 생성이 제대로 되었는지 확인 */
    if (hSocket == INVALID_SOCKET) {
        puts("ERROR: 소켓 생성에 실패 했습니다.");
        return 0;
    }

    /** socket IO Buffer 에 대해서 확인하기 위한 값을 담을 버퍼 및 변수 */
    int nBufferSize = 0;
    int nLength = sizeof(nBufferSize);
    /** socket에 적용이 도어 있는 option 가져오기 */
    int getSendBufferSizeFlag = ::getsockopt(
            hSocket, // 옵션을 알고 싶은 socket에 대한 HANDLE 객체를 인자로 받는다.
            SOL_SOCKET, // 가져올 옵션에 대한 범위를 지정하는 옵션 -> socket 수준의 옵션을 가져오기 위한 SOL_SOCKET
            SO_SNDBUF, // 가져올 옵션 값 -> Send Buffer를 가져오기 위한 옵션 값이다.
            (char *) &nBufferSize, // 옵션 값을 담아줄 버퍼를 인자로 받는다.
            &nLength // 버퍼의 크기를 인자로 받는다.
    );

    /** 옵션을 가져오기에 성고 시 */
    if (getSendBufferSizeFlag != SOCKET_ERROR) {
        printf("Send Buffer Size : %d\r\n", nBufferSize);
    }
    /** 수신 버퍼의 공간을 저장하기 위한 초기화 */
    nBufferSize = 0;
    nLength = sizeof(nBufferSize);
    /** 수신 버퍼에 대한 공간을 가져오기 */
    int getReceiveBufferSizeFlag = ::getsockopt(
            hSocket, // 옵션을 알고 싶은 socket에 대한 HANDLE 객체를 인자로 받는다.
            SOL_SOCKET, // 가져올 옵션에 대한 범위를 지정하는 옵션 -> socket 수준의 옵션을 가져오기 위한 SOL_SOCKET
            SO_RCVBUF, // 가져올 옵션 값 -> Receive Buffer를 가져오기 위한 옵션 값
            (char *) &nBufferSize, // 옵션 값을 담아줄 버퍼를 인자로 받는다.
            &nLength // 버퍼의 크기를 인자로 받는다.
    );

    if (getReceiveBufferSizeFlag != SOCKET_ERROR) {
        printf("Receive Buffer Size : %d\r\n", nBufferSize);
    }
    /** socket 닫아주기 */
    ::closesocket(hSocket);
    /** 윈도우 소켓을 OS에게 반환 */
    WSACleanup();
    return 0;
}