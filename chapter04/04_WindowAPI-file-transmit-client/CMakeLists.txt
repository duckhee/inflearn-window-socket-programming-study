cmake_minimum_required(VERSION 3.27)

project(chapter0404WindowAPIFileTransmitClient LANGUAGES CXX VERSION 0.0.1)

message("chapter04-02 Window API File Transmit Client")

add_executable(chapter0404WindowAPIFileTransmitClient)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0404WindowAPIFileTransmitClient LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0404WindowAPIFileTransmitClient PRIVATE
        main.cpp
)