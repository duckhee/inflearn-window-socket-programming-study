cmake_minimum_required(VERSION 3.27)

project(chapter0501FileProtocolClient LANGUAGES CXX VERSION 0.0.1)

message("chapter05-01 file protocol client")

add_executable(chapter0501FileProtocolClient)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0501FileProtocolClient LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0501FileProtocolClient PRIVATE
        AppProtocol.h
        main.cpp
)