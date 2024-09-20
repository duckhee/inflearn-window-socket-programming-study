#include <iostream>
#include <tchar.h>
#include <Windows.h>

using namespace std;


int main(int argc, char **argv) {
    /** 중첩된 요청이 가능하도록 팡리 생성 */
    HANDLE hFile = ::CreateFile(
            _T("TestFile.txt"),
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
            NULL
    );

    if (hFile == INVALID_HANDLE_VALUE) {
        cout << "Failed Open File ... TestFile.txt" << endl;
        return 0;
    }
    /** 실제로 파일에 작성이 된 길이를 담아줄 변수 */
    DWORD dwRead;
    /** 중첩된 요청을 하기 위한 구조체 배열 */
    OVERLAPPED aOl[3] = {0,};
    /** 완료 시 발행할 이벤트에 대한 구조체 배열 */
    HANDLE aEvent[3] = {0,};

    for (int i = 0; i < 3; ++i) {
        /** 중첩된 요청에 대해서 각각의 이벤트 등록 하기 위한 이벤트 생성 */
        aEvent[i] = ::CreateEvent(
                NULL,
                FALSE,
                FALSE,
                NULL
        );
        /** 이벤트에 대한 값을 등록 */
        aOl[i].hEvent = aEvent[i];
    }

    aOl[0].Offset = 0; // 0
    aOl[1].Offset = 1024 * 1024 * 128; // 128MB
    aOl[2].Offset = 16; // 16byte

    for (int i = 0; i < 3; ++i) {
        printf("%d OverLapped File Write Try... \r\n", i);
        /** 파일 쓰기 요청에 대한 OS에 전달 */
        ::WriteFile(
                hFile,
                "0123456789",
                10,
                &dwRead,
                &aOl[i]
        );

        if (::GetLastError() != ERROR_IO_PENDING) {
            exit(0);
        }
    }

    DWORD dwResult = 0;

    for (int i = 0; i < 3; ++i) {
        /** 이벤트 완료 대기 */
        dwResult = ::WaitForMultipleObjects(3, aEvent, FALSE, INFINITE);
        printf(" -> %d OverLapped Write Done\r\n", (int) (dwResult - WAIT_OBJECT_0));
    }

    for (int i = 0; i < 3; ++i) {
        ::CloseHandle(aEvent[i]);
    }

    ::CloseHandle(hFile);

    return 0;
}