#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include "AppProtocol.h"


void ErrorHandler(const char *msg);

void GetFileList(SOCKET hSocket);

void GetFile(SOCKET hSocket);

int main(int argc, char **argv) {
    SOCKET hSocket;
    SOCKADDR_IN serverAddr = {0,};

    /** Win Socket Initialized... */
    WSAData wsaData = {0,};
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandler("Failed Initialized WinSock");
    }

    /** socket 생성 */
    hSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET) {
        ErrorHandler("Socket Create Failed...");
    }

    /** server 정보 */
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(25000);

    /** server 연결 */
    if (::connect(hSocket, (SOCKADDR *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        ErrorHandler("Connected Server Failed...");
    }

    GetFileList(hSocket);
    GetFile(hSocket);

    ::closesocket(hSocket);
    ::WSACleanup();
    return 0;
}

void ErrorHandler(const char *msg) {
    printf("ERROR : %s\r\n", msg);
    fflush(stdout);
    ::WSACleanup();
    exit(1);
}


void GetFileList(SOCKET hSocket) {
    /** 파일 목록 요청하는 명령어 */
    MY_CMD command = {CMD_GET_LIST, 0};
    /** 파일 목록 요청 명령어 전송 */
    ::send(hSocket, (const char *) &command, sizeof(command), 0);

    /** 명령에 따른 응답 받기 */
    ::recv(hSocket, (char *) &command, sizeof(command), 0);
    if (command.nCode != CMD_SND_FILE_LIST) {
        ErrorHandler("서버에서 파일 리스트를 수신하지 못했습니다.");
    }

    /** 파일 정보를 담을 구조체 */
    SEND_FILE_LIST fileList;
    ::recv(hSocket, (char *) &fileList, sizeof(fileList), 0);

    /** 수신한 파일 목록을 출력 -> 파일 데이터 형태로 끊어서 읽는다. */
    /** 출력만 할 용도로 사용 되기 때문에 구조체 하나로 사용을 한다. -> 메모리 절약 */
    FILE_INFO fInfo;
    for (unsigned int i = 0; i < fileList.nCount; ++i) {
        ::recv(hSocket, (char *) &fInfo, sizeof(fInfo), 0);
        printf("%d\t%s\t%d\r\n", fInfo.nIndex, fInfo.szFileName, fInfo.dwFileSize);
    }
}

void GetFile(SOCKET hSocket) {

    /** 요청할 파일에 대한 인덱스 값 */
    int nIndex;
    printf("수신할 파일의 인덱스(0 ~ 2)를 입력하세요 : ");
    fflush(stdout);
    scanf_s("%d", &nIndex);

    // 한번에 전송을 하기 위한 메모리 할당
    /** 여러 헤더에 대한 정보를 한번에 보내기 위해서 동적으로 메모리 할당을 해준다. -> 핵심은 빈 공간 없이 연속적인 형태로 보내야 한다는 것이다. */
    BYTE *pCommand = new BYTE[sizeof(MY_CMD) + sizeof(GET_FILE)];
    MY_CMD *pCmd = (MY_CMD *) pCommand;
    pCmd->nCode = CMD_GET_FILE;
    pCmd->nSize = sizeof(GET_FILE);

    GET_FILE *pFile = (GET_FILE *) (pCommand + sizeof(MY_CMD));
    pFile->nIndex = nIndex;

    ::send(hSocket, (const char *) pCommand, sizeof(MY_CMD) + sizeof(GET_FILE), 0);
    delete[] pCommand;

    /** 상세 정보에 대해서 수신 확인 */
    MY_CMD cmd = {0,};
    FILE_INFO fInfo = {0,};
    ::recv(hSocket, (char *) &cmd, sizeof(cmd), 0);
    if (cmd.nCode == CMD_ERROR) {
        ErrorData error = {0,};
        ::recv(hSocket, (char *) &error, sizeof(error), 0);
        ErrorHandler(error.szDesc);
    } else {
        ::recv(hSocket, (char *) &fInfo, sizeof(fInfo), 0);
    }

    // 파일을 수신한다.
    printf("%s 파일 수신을 시작합니다!\r\n", fInfo.szFileName);
    FILE *fp = NULL;
    errno_t nResult = fopen_s(&fp, fInfo.szFileName, "wb");
    if (nResult != 0) {
        ErrorHandler("파일을 생성할 수 없습니다.");
    }

    char byBuffer[SEND_MAX_BUFFER_SIZE] = {0,};
    int nRecv;
    DWORD dwTotalRecv = 0;

    while ((nRecv = ::recv(hSocket, byBuffer, SEND_MAX_BUFFER_SIZE, 0)) > 0) {
        fwrite(byBuffer, nRecv, 1, fp);
        dwTotalRecv += nRecv;
        putchar('#');

        if (dwTotalRecv >= fInfo.dwFileSize) {
            putchar('\n');
            puts("파일 수신 완료 !");
            break;
        }
    }

    fclose(fp);
}