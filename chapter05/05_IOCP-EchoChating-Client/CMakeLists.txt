cmake_minimum_required(VERSION 3.27)

project(chapter0505IOCPEchoChattingClient LANGUAGES CXX VERSION 0.0.1)

message("chapter05-03 IOCP Echo Chatting Client")

add_executable(chapter0505IOCPEchoChattingClient)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0505IOCPEchoChattingClient LINK_PRIVATE wsock32 ws2_32)
endif ()


target_sources(chapter0505IOCPEchoChattingClient PRIVATE
        main.cpp
)