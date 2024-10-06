#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include "appProtocol.h"

#define MAX_FILE_PATH                       1024

void ErrorHandle(const char *msg);

void SendFileList(SOCKET hClient);

void SendFile(SOCKET hClient, int index);

void GetFilePath(const char *name, char *path);

/** 전송이 가능한 파일 목록을 관리하기 위한 전역 변수 */
SEND_FILE_LIST g_fList = {3};

/** 파일에 대한 정보를 가지고 있는 배열 */
FILE_INFO g_aFInfo[3] = {
        {0, "Sleep Away.mp3",                4842585},
        {1, "Kalimba.mp3",                   8414449},
        {2, "Maid with the Flaxen Hair.mp3", 4113874}
};

int main(int argc, char **argv) {
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandle("Failed Window Socket Initialized...");
    }
    /** address socket */
    SOCKET hSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET) {
        ErrorHandle("접속 대기 소켓을 생성할 수 없습니다.");
    }

    /** Socket Information */
    SOCKADDR_IN serverAddr = {0,};
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(25000);
    serverAddr.sin_family = PF_INET;

    if (::bind(hSocket, (SOCKADDR *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        ErrorHandle("소켓에 IP와 포트를 바인드할 수 없습니다.");
    }

    if (::listen(hSocket, SOMAXCONN) == SOCKET_ERROR) {
        ErrorHandle("리슨 상태로 변경할 수 없습니다.");
    }

    puts("파일 송신 서버를 시작합니다.");

    SOCKADDR_IN clientAddr = {0,};
    int nClientSize = sizeof(clientAddr);

    SOCKET hClient = ::accept(hSocket, (SOCKADDR *) &clientAddr, &nClientSize);
    if (hClient == INVALID_SOCKET) {
        ErrorHandle("클라이언트 통신 소켓을 생성할 수 없습니다.");
    }
    puts("클라이언트 연결이 되었습니다.");

    MY_CMD cmd;

    while (::recv(hClient, (char *) &cmd, sizeof(MY_CMD), 0) > 0) {
        switch (cmd.nCode) {
            case CMD_GET_LIST:
                puts("클라이언트가 파일 목록을 요청했습니다.");
                SendFileList(hClient);
                break;
            case CMD_GET_FILE:
                puts("클라이언트가 파일 전송을 요청했습니다.");
                {
                    SEND_FILE_LIST file;
                    ::recv(hClient, (char *) &file, sizeof(file), 0);
                    SendFile(hClient, file.nIndex);
                }
        }
    }
    closesocket(hClient);
    closesocket(hSocket);
    ::WSACleanup();
    return 0;
}


void ErrorHandle(const char *msg) {
    printf("ERROR : %s\r\n", msg);
    fflush(stdout);
    ::WSACleanup();
    exit(1);
}

void SendFileList(SOCKET hClient) {
    /** 전달할 명령어 구조체 정의 */
    MY_CMD cmd;
    /** 파일 목록 전달하는 명령어 */
    cmd.nCode = CMD_SND_FILE_LIST;
    /** 파일에 대한 정보를 담고 있는 구조체 데이터 -> 전달할 데이터의 총 바이트 수 전달 */
    cmd.nSize = sizeof(g_fList) + sizeof(g_aFInfo);
    /** 기본 헤더 전달 */
    ::send(hClient, (const char *) &cmd, sizeof(cmd), 0);
    /** 파일 리스트 헤더 전송 */
    ::send(hClient, (const char *) &g_fList, sizeof(g_fList), 0);
    /** 파일 정보들 전송 */
    ::send(hClient, (const char *) &g_aFInfo, sizeof(g_aFInfo), 0);
}

void SendFile(SOCKET hClient, int index) {

    char pathBuffer[MAX_FILE_PATH] = {0,};

    MY_CMD cmd;
    ErrorData error;
    /** 해당 인덱스에 값이 파일 목록에 존재하는지 확인 */
    if (index < 0 || index > 2) {
        cmd.nCode = CMD_ERROR;
        cmd.nSize = sizeof(error);
        error.nErrorCode = 0;
        strcpy_s(error.szDesc, "잘못된 팡리 인덱스 입니다.");
        /** 오류 정보 클라이언트에 전달 */
        ::send(hClient, (const char *) &cmd, sizeof(cmd), 0);
        ::send(hClient, (const char *) &error, sizeof(error), 0);
        return;
    }

    /** 파일 송신 싲강르 알리는 정보를 전송 */
    cmd.nCode = CMD_BEGIN_FILE;
    cmd.nSize = sizeof(FILE_INFO);
    ::send(hClient, (const char *) &cmd, sizeof(cmd), 0);
    ::send(hClient, (const char *) &g_aFInfo[index], sizeof(FILE_INFO), 0);

    FILE *fp = nullptr;
    GetFilePath(g_aFInfo[index].szFileName, pathBuffer);
    errno_t nResult = fopen_s(&fp, pathBuffer, "rb");
    if (nResult != 0) {
        ErrorHandle("전송할 파일을 개방할 수 없습니다.");
    }

    /** 파일을 전송한다. */
    char byBuffer[SEND_MAX_BUFFER_SIZE] = {0,};
    int nRead = 0;

    while ((nRead = fread(byBuffer, sizeof(char), SEND_MAX_BUFFER_SIZE, fp)) > 0) {
        ::send(hClient, byBuffer, nRead, 0);
    }
    fclose(fp);
}


void GetFilePath(const char *name, char *path) {
    char executeBuffer[MAX_FILE_PATH] = {0,};
    char tempBuffer[MAX_FILE_PATH] = {0,};
    char *pRemoveStart = nullptr;
    int removeEndIdx = 0;
    /** 현재 프로그램 실행 경로 가져오기 */
    ::GetCurrentDirectory(MAX_FILE_PATH, executeBuffer);
    /** cmake 경로 확인 -> Clion을 사용하기 때문에 */
    pRemoveStart = strstr(executeBuffer, "\\cmake");

    if (pRemoveStart == nullptr) {
        ErrorHandle("Get File Path Failed...");
    }

    removeEndIdx = (int) (pRemoveStart - executeBuffer);

    memcpy(tempBuffer, executeBuffer, sizeof(char) * removeEndIdx);
    strcat(tempBuffer, "\\chapter05\\01_fileProtocol-server\\resources\\");
    strcat(tempBuffer, name);
    strcpy(path, tempBuffer);
}