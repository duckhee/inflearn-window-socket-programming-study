// FtpClientSample.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include <iostream>
#include <tchar.h>
#include <Windows.h>
#include <WinInet.h>

#pragma comment(lib, "Wininet.lib")


int main(int argc, char **argv) {
    //WinINet 라이브러리를 초기화 한다.
    HINTERNET hInet = ::InternetOpen(_T("TEST FTP"),
                                     INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

    //FTP 서버에 연결한다.
    HINTERNET hFtp = ::InternetConnect(hInet,
                                       _T("127.0.0.1"), 21,
                                       NULL, NULL,
                                       INTERNET_SERVICE_FTP,
                                       0, 1234);
    if (hFtp == NULL) {
        ::InternetCloseHandle(hInet);
        puts("ERROR: FTP 서버에 연결할 수 없습니다.");
        return 0;
    }

    //FTP 루트 디렉토리에 들어있는 모든 파일들을 열거한다.
    //첫 번째 파일 출력.
    WIN32_FIND_DATA findData = {0};
    TCHAR szFirstFile[_MAX_FNAME];
    HINTERNET hFtpFile = ::FtpFindFirstFile(hFtp,
                                            _T("*.*"),
                                            &findData,
                                            INTERNET_FLAG_NEED_FILE | INTERNET_FLAG_RELOAD,
                                            0);
    if (hFtpFile != NULL) {
        puts(findData.cFileName);
        wsprintf(szFirstFile, _T("%s"), findData.cFileName);

        //첫 번째를 제외한 나머지 파일들 출력.
        while (::InternetFindNextFile(hFtpFile, &findData))
            puts(findData.cFileName);
    }

    //첫 번째 파일을 FTP로 다운로드 받기 위해
    //FTP 서버의 파일을 개방한다.
    HINTERNET hFtpFileFirst = FtpOpenFile(hFtp,
                                          szFirstFile,
                                          GENERIC_READ,
                                          FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD,
                                          0);
    if (hFtpFileFirst != NULL) {
        //로컬 디스크에 파일을 생성한다.
        HANDLE hFile = ::CreateFile(szFirstFile,
                                    GENERIC_WRITE, 0, NULL,
                                    CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            BYTE byBuffer[8192];
            DWORD dwRead, dwWritten;

            //FTP 서버에서 8KB씩 읽어와 파일에 쓴다.
            puts("다운로드 시작");
            while (::InternetReadFile(hFtpFileFirst,
                                      byBuffer, sizeof(byBuffer), &dwRead)) {
                //더 이상 읽을 데이터가 없으므로 다 읽은 것이다.
                if (dwRead == 0) {
                    puts("\n다운로드 완료");
                    break;
                }

                ::WriteFile(hFile, byBuffer, dwRead, &dwWritten, NULL);
                putchar('#');
            }

            ::CloseHandle(hFile);
        }

        ::InternetCloseHandle(hFtpFileFirst);
    }

    ::InternetCloseHandle(hFtpFile);
    ::InternetCloseHandle(hFtp);
    ::InternetCloseHandle(hInet);
    return 0;
}

