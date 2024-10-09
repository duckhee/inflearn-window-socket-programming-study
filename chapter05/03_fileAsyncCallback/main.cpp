#include <iostream>
//#include <WinSock2.h>
#include <Windows.h>
#include <tchar.h>

void CALLBACK FileIoComplete(DWORD errorCode, DWORD dwTransfered, LPOVERLAPPED lOl);

DWORD WINAPI IoThreadFunction(LPVOID pParam);

int main(int argc, char **argv) {
    HANDLE hFile = ::CreateFile(
            _T("TextFile.txt"), // 생성할 파일 이름
            GENERIC_WRITE, // 파일을 쓰기 모드로 열기
            0, // 공유 모드 설정
            NULL, // 보안 속성에 대한 설정 -> 프로그램 보안 속성 상속
            CREATE_ALWAYS, // 항상 파일 새로 생성
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // 파일에 대한 속성 정의
            NULL // HANDLE에 대한 이름 부여 시 사용
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        puts("ERROR: Failed File Open");
        return 0;
    }

    /** Thread 생성 */
    HANDLE hThread = NULL;
    DWORD dwThreadID = 0;
    hThread = ::CreateThread(
            NULL, // Thread에 대한 보안 속성 정의
            0, // Thread에 대한 Stack 크기 설정 0은 기본 값 1MB 사용
            IoThreadFunction, // Thread에서 동작할 함수
            hFile, // Thrad에 넘겨줄 파라임터
            0, // Thread 생성 시에 옵션을 인자로 받는다.
            &dwThreadID // 생성된 Thread의 아이디 값을 담아줄 포인터 변수를 인자로 받는다.
    );

    /** 쓰레드가 완료가 될 때 까지 대기 */
    ::WaitForSingleObject(hThread, INFINITE);

    return 0;
}

DWORD WINAPI IoThreadFunction(LPVOID pParam) {
    // 메모리를 동적 할당을 하고 값을 채운다.
    /** 이 메모리는 완료 함수에서 해제를 한다. */
    char *pszBuffer = new char[16];
    memset(pszBuffer, 0, sizeof(char) * 16);
    strcpy_s(pszBuffer, sizeof(char) * 16, "Hello IOCP!");

    /** OVERLAPPED 구조체의 hEvent 멤버를 포인터 변수로 사용 한다. */
    LPOVERLAPPED pOverLapped = NULL;
    pOverLapped = new OVERLAPPED;
    memset(pOverLapped, NULL, sizeof(OVERLAPPED));

    /** 쓰기를 시작할 위치 Offset */
//    pOverLapped->Offset = 1024 * 1024 * 512; // 512MB 위치부터 쓰기 시작
    pOverLapped->Offset = 1024 * 1024 * 24; // 24MB 위치부터 쓰기 시작
    pOverLapped->hEvent = pszBuffer; // Pointer 변수로 사용

    /** 비동기 파일 쓰기 */
    puts("IoThreadFunction() - 중첩된 쓰기 시도");
    ::WriteFileEx(
            (HANDLE) pParam, // 쓸 파일에 대한 HANDLE
            pszBuffer, // 쓸 데이터의 버퍼 주소
            sizeof(char) * 16, // 쓸 데이터의 총 바이트 수
            pOverLapped, // 중첩된 요청의 인자
            FileIoComplete // 파일 쓰기 완료 시 동작할 함수
    );
    /** 비동기 완료가 될 때까지 대기 */
    for (; ::SleepEx(1, TRUE) != WAIT_IO_COMPLETION;);

    puts("IoThreadFunction() - return");
    return 0;
}

void CALLBACK FileIoComplete(DWORD errorCode, DWORD dwTransfered, LPOVERLAPPED pOl) {
    printf("FileIoComplete() callback - [%dByte] 쓰기 완료 - %s\r\n", dwTransfered, pOl->hEvent);
    /** 쓰레드 생성 함수 내에서 할당한 동적 메모리 해제 */
    delete[] pOl->hEvent;
    delete pOl;
    puts("FileIoComplete() - Return\r\n");

}

