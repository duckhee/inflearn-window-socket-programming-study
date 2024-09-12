cmake_minimum_required(VERSION 3.27)

project(chapter0209MultiThreadChattingClient LANGUAGES CXX VERSION 0.0.1)

message("chapter02-05 multi thread chatting client")

add_executable(chapter0209MultiThreadChattingClient)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0209MultiThreadChattingClient LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0209MultiThreadChattingClient PRIVATE
        main.cpp
)