
cmake_minimum_required(VERSION 3.27)

project(chapter0201SocketIOBuffer LANGUAGES CXX VERSION 0.0.1)

message("chapter02-01 socket IO Buffer")

add_executable(chapter0201SocketIOBuffer)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0201SocketIOBuffer wsock32 ws2_32)
endif ()

target_sources(chapter0201SocketIOBuffer PRIVATE
        main.cpp
)