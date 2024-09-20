cmake_minimum_required(VERSION 3.27)

project(chapter0301OverLappedIO LANGUAGES CXX)

message("chapter03-01 OverLapped IO Example")

add_executable(chapter0301OverLappedIO)

target_sources(chapter0301OverLappedIO PRIVATE
        main.cpp
)