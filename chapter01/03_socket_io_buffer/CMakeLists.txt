
cmake_minimum_required(VERSION 3.27)

project(chapter0103SocketIOBuffer LANGUAGES CXX VERSION 0.0.1)

message("chapter01-03 socket IO Buffer")

add_executable(chapter0103SocketIOBuffer)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0103SocketIOBuffer wsock32 ws2_32)
endif ()

target_sources(chapter0103SocketIOBuffer PRIVATE
        main.cpp
)