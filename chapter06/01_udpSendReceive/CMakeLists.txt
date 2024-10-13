cmake_minimum_required(VERSION 3.27)

project(chapter0601UDPSendReceive LANGUAGES CXX VERSION 0.0.1)

message("chapter06-01 UDP send and receive")

add_executable(chapter0601UDPSendReceive)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0601UDPSendReceive LINK_PRIVATE
            wsock32 ws2_32
    )
endif ()

target_sources(chapter0601UDPSendReceive PRIVATE
        main.cpp
)