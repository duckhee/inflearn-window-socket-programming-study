cmake_minimum_required(VERSION 3.27)

project(chapter0602BroadCastReceiver LANGUAGES CXX VERSION 0.0.1)

message("chapter06-02 broad cast receiver")

add_executable(chapter0602BroadCastReceiver)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0602BroadCastReceiver LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0602BroadCastReceiver PRIVATE
        main.cpp
)