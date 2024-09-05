#include <iostream>
#include <winsock2.h>
#include <Windows.h>
#include <tchar.h>

#pragma comment(lib, "ws2_32")

int main(int argc, char **argv) {

    /** window socket 에 대한 초기화 */
    WSADATA wsa = {0,};
    if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        puts("윈도우 소켓을 초기화할 수 없습니다.");
        return 0;
    }

    /** socket 을 생성하는 함수 socket */
    SOCKET hSocket = ::socket(
            AF_INET, // IPv4 형태의 아이피 주소를 가지는 형태인 것을 설정 -> L3 스위치에 대한 정보를 나타낸다고 볼 수 있다.
            SOCK_STREAM, // 소켓의 형태에 대해서 정의하는 값을 인자로 받는다. -> L4 스위치에 대한 정보를 나타낸다고 볼 수 있다.
            IPPROTO_TCP // 프로토콜에 대해서 인자로 준다. -> TCP 통신을 하기 때문에 TCP 에 대한 프로토콜 값 넣어주기
//            0
    );
    /** socket 생성 실패 시 동작 */
    if (hSocket == INVALID_SOCKET) {
        puts("ERROR: 접속 대기 소켓을 생성할 수 없습니다.");
        return 0;
    }
    /** 소켓에 넣어줄 정보를 가지고 있는 구조체 */
    SOCKADDR_IN serverAddr = {0,};
    /** 서버의 주소 체계 정의 */
    serverAddr.sin_family = PF_INET;
    /** 서버의 주소를 설정하는 과정 -> htonl은 네트워크 방식으로 변경을 해주는 것을 말한다. */
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    /** 서버 소켕에 대한 포트 정보 설정 */
    serverAddr.sin_port = htons(25000);

    /** 생성된 소켓을 네트워크 연결을 위한 설정 진행 */
    /** bind 실패 시 */
    if (::bind(
            hSocket, // 연결할 소켓에 대한 handle 객체
            (SOCKADDR *) &serverAddr, // 소켓에 대한 주소 정보 체계
            sizeof(serverAddr) // 서버 정보를 담고 있는 구조체의 크기
    ) == SOCKET_ERROR) {
        puts("ERROR : 소켓에 IP 주소와 포트를 바인딩할 수 없습니다.");
        return 0;
    }

    /** client의 접속을 대기하기 위한 Listen */
    /** 대기 상태 확인 */
    if (::listen(
            hSocket, // 연결 대기상태로 만들 socket에 대한 구조체
            SOMAXCONN // 대기 처리를 해줄 수 있는 Queue 의 크기 값을 설정
    ) == SOCKET_ERROR) {
        puts("ERROR : socket 대기 상태 실패");
        return 0;
    }
    std::cout << "Server Socket Listening!" << std::endl;
    /** 클라이언트 정보를 담을 구조체 */
    SOCKADDR_IN clientAddr = {0,};
    int nAddressLength = sizeof(clientAddr);
    SOCKET hClient = 0;
    /** 통신을 위한 버퍼 선언 */
    char szBuffer[128] = {0,};
    int nReceive = 0;

    /** 연결 후 동작 에 대한 처리 */
    while ((hClient = ::accept(
            hSocket,
            (SOCKADDR *) &clientAddr,
            &nAddressLength
    )) != INVALID_SOCKET) {
        puts("새로운 클라이언트 연결이 되었습니다.");
        fflush(stdout);
        /** 데이터를 수신*/
        while ((nReceive = ::recv(hClient, szBuffer, sizeof(szBuffer), 0)) > 0) {
//        while (::recv(hClient, pszBuffer, sizeof(pszBuffer), 0) > 0) {
            /** 수신한 데이터 전송 */
            ::send(hClient, szBuffer, sizeof(szBuffer), 0);
            /** 수신 받은 데이터 출력 */
            puts(szBuffer);
            /** 버퍼 초기화 */
            memset(szBuffer, '\0', sizeof(szBuffer));
        }
        /** 연결 종료 */
        ::shutdown(hSocket, SD_BOTH);
        ::closesocket(hClient);
        puts("클라이언트 연결이 종료가 되었습니다.");
    }

    /** 접속을 감지하는 소켓 종료 */
    ::closesocket(hSocket);
    /** window socket 제거 */
    ::WSACleanup();
    return 0;
}