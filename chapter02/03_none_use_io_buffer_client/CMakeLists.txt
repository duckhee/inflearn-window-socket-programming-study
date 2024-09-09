cmake_minimum_required(VERSION 3.27)

project(chapter0203NoneUseIOBufferClient LANGUAGES CXX VERSION 0.0.1)

message("chapter02-02 none use Socket IO Buffer -> TCP_NODELAY")

add_executable(chapter0203NoneUseIOBufferClient)


if (WIN32 OR WIN64)
    target_link_libraries(chapter0203NoneUseIOBufferClient LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0203NoneUseIOBufferClient PRIVATE
        main.cpp
)