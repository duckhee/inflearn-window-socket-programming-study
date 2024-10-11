cmake_minimum_required(VERSION 3.27)

project(chapter04IOCPEchoChattingServer LANGUAGES CXX VERSION 0.0.1)

message("chapter03-IOCP Echo Chatting Server")

add_executable(chapter04IOCPEchoChattingServer)

if (WIN32 OR WIN64)
    target_link_libraries(chapter04IOCPEchoChattingServer LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter04IOCPEchoChattingServer PRIVATE
        AppProtocol.h
        main.cpp
)