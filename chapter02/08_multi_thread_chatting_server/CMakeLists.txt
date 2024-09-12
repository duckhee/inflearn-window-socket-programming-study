cmake_minimum_required(VERSION 3.27)

project(chapter0208MultiThreadChattingServer LANGUAGES CXX VERSION 0.0.1)

message("chapter02-05 multi thread chatting server")

add_executable(chapter0208MultiThreadChattingServer)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0208MultiThreadChattingServer LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0208MultiThreadChattingServer PRIVATE
        main.cpp
)