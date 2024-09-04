cmake_minimum_required(VERSION 3.27)

project(chapter0101EchoServer LANGUAGES CXX VERSION 0.0.1)

message("chapter01-01 echo server")

add_executable(chapter0101EchoServer)

target_sources(chapter0101EchoServer PRIVATE
        main.cpp
)