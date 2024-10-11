#ifndef INFLEARN_WINDOW_SOCKET_PROGRAMMING_STUDY_APPPROTOCOL_H
#define INFLEARN_WINDOW_SOCKET_PROGRAMMING_STUDY_APPPROTOCOL_H

#define SOCKET_BUFFER_MAX           1024 * 8 // 8KB

#define MAX_THREAD_CNT              4 // client 처리를 위한 작업자 Thread 갯수


typedef struct _USER_SESSION {
    SOCKET hSocket;
    char buffer[SOCKET_BUFFER_MAX];
} USER_SESSION;

#endif
