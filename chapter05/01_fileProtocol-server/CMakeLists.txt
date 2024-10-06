cmake_minimum_required(VERSION 3.27)

project(chapter0501FileProtocolServer LANGUAGES CXX VERSION 0.0.1)

message("chapter05-01 file protocol server")

add_executable(chapter0501FileProtocolServer)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0501FileProtocolServer LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0501FileProtocolServer PRIVATE
        appProtocol.h
        main.cpp
)