cmake_minimum_required(VERSION 3.27)

project(chapter0401NotProtocolFileSendServer LANGUAGES CXX VERSION 0.0.1)

message("chapter04-01 not protocol file send server")

add_executable(chapter0401NotProtocolFileSendServer)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0401NotProtocolFileSendServer LINK_PRIVATE wsock32 ws2_32)
endif ()


target_sources(chapter0401NotProtocolFileSendServer PRIVATE
        main.cpp
)