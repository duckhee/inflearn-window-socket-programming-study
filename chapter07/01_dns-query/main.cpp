// DnsQuerySample.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include <iostream>
#include <winsock2.h>
#include <tchar.h>


int main(int argc, char **argv) {
    //윈속 초기화
    WSADATA wsa = {0};
    if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        puts("ERROR: 윈속을 초기화 할 수 없습니다.");
        return 0;
    }

    //현재 DNS 설정을 근거로 Naaver의 IP주소를 질의한다.
    hostent *pHost = ::gethostbyname("www.naver.com");
    if (pHost == NULL) {
        puts("ERROR: Naver의 IP주소를 알 수 없습니다.");
        ::WSACleanup();
        return 0;
    }

    printf("Official name: %s\n", pHost->h_name);

    for (int i = 0; pHost->h_aliases[i] != NULL; ++i)
        printf("\t별칭: %s\n", pHost->h_aliases[i]);

    for (int i = 0; pHost->h_addr_list[i] != NULL; ++i)
        printf("\tIP주소: %s\n",
               inet_ntoa(*(in_addr *) pHost->h_addr_list[i]));

    //윈속 해제
    ::WSACleanup();
    return 0;
}

