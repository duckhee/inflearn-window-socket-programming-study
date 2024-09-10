cmake_minimum_required(VERSION 3.27)

project(chapter0204SO_REUSEADDR_Socket_Server LANGUAGES CXX VERSION 0.0.1)

message("chapter02-03 SO_REUSEADDR Option Socket Echo Server")

add_executable(chapter0204SO_REUSEADDR_Socket_Server)

if (WIN32 OR WIN65)
target_link_libraries(chapter0204SO_REUSEADDR_Socket_Server LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0204SO_REUSEADDR_Socket_Server PRIVATE
        main.cpp
)