#ifndef INFLEARN_WINDOW_SOCKET_PROGRAMMING_STUDY_APPPROTOCOL_H
#define INFLEARN_WINDOW_SOCKET_PROGRAMMING_STUDY_APPPROTOCOL_H


#define ERROR_MSG_MAX_SIZE              256
#define FILE_MAX_NAME_SIZE              256
#define SEND_MAX_BUFFER_SIZE            1024 * 64

typedef enum _CMD_Code {
    CMD_ERROR = 50,                 // 에러
    CMD_GET_LIST = 100,            // Client -> Server 파일 리스트 요구
    CMD_GET_FILE,                   // Client -> Server 파일 전송 요구
    CMD_SND_FILE_LIST = 200,        // Server -> Client 파일 리스트 전송
    CMD_BEGIN_FILE                  // Server -> Client 파일 전송을 시작을 알림
} CMDCode_t;

typedef struct _MYCMD {
    int nCode; // 명령 코드
    int nSize; // 데이터의 바이트 단위 크기
} MY_CMD;

typedef struct _ErrorData {
    int nErrorCode; // 에러 코드 -> 확장을 위한 멤버
    char szDesc[ERROR_MSG_MAX_SIZE];  // 에러 메세지
} ErrorData;

typedef struct _SEND_FILE_LIST {
    unsigned int nCount; // 전송 받거나 전송 하거나 하기 위한 파일의 인덱스
} SEND_FILE_LIST;

typedef struct _GET_FILE {
    unsigned int nIndex;
} GET_FILE;

typedef struct _FILE_INFO {
    unsigned int nIndex; // 파일의 인덱스
    char szFileName[FILE_MAX_NAME_SIZE]; // 파일의 일므
    DWORD dwFileSize; // 파일의 바이트 단위 크기
} FILE_INFO;


#endif
