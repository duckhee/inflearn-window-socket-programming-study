cmake_minimum_required(VERSION 3.27)

project(chapter0703FtpSimpleClient LANGUAGES CXX VERSION 09.0.1)

message("chapter07-03 Simple FTP Client")

add_executable(chapter0703FtpSimpleClient)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0703FtpSimpleClient LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0703FtpSimpleClient PRIVATE
        main.cpp
)