cmake_minimum_required(VERSION 3.27)

project(chapter0503FileAsyncCallback LANGUAGES CXX VERSION 0.0.1)

message("chapter05-02 Async File Callback - Write")

add_executable(chapter0503FileAsyncCallback)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0503FileAsyncCallback LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0503FileAsyncCallback PRIVATE
        main.cpp
)