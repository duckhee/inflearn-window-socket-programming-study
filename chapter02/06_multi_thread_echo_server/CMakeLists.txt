cmake_minimum_required(VERSION 3.27)

project(chapter0206MultiThreadEchoServer LANGUAGES CXX VERSION 0.0.1)

message("chapter02-04 multi thread echo server")

add_executable(chapter0206MultiThreadEchoServer)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0206MultiThreadEchoServer LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0206MultiThreadEchoServer PRIVATE
        main.cpp
)