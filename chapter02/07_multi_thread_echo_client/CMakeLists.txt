cmake_minimum_required(VERSION 3.27)

project(chapter0207MultiThreadEchoClient LANGUAGES CXX VERSION 0.0.1)

message("chapter02-04 multi thread echo client")

add_executable(chapter0207MultiThreadEchoClient)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0207MultiThreadEchoClient LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0207MultiThreadEchoClient PRIVATE
        main.cpp
)