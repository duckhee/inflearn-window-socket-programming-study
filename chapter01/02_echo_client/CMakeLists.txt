cmake_minimum_required(VERSION 3.27)

project(chapter0102EchoClient LANGUAGES CXX VERSION 0.0.1)

message("chapter01-02 echo client")

add_executable(chapter0102EchoClient)

if(WIN32)
    target_link_libraries(chapter0102EchoClient wsock32 ws2_32)
endif()

target_sources(chapter0102EchoClient PRIVATE
        main.cpp
)