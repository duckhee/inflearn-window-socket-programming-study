cmake_minimum_required(VERSION 3.27)

project(chapter0202NoneUseBufferIOServer LANGUAGES CXX VERSION 0.0.1)

message("chapter02-02 none use socket IO Buffer Server -> TCP_NODELAY")

add_executable(chapter0202NoneUseBufferIOServer)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0202NoneUseBufferIOServer LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0202NoneUseBufferIOServer PRIVATE
        main.cpp
)