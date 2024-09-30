cmake_minimum_required(VERSION 3.27)

project(chapter0403WindowAPIFileTransmitServer LANGUAGES CXX VERSION 0.0.1)

message("chapter04-02 window API File Transmit Server")

add_executable(chapter0403WindowAPIFileTransmitServer)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0403WindowAPIFileTransmitServer LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0403WindowAPIFileTransmitServer PRIVATE
        main.cpp
)