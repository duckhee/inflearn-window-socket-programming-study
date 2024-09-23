cmake_minimum_required(VERSION 3.27)

project(chapter0303MultiPlexingChattingClient LANGUAGES CXX VERSION 0.0.1)

message("chapter03-02 multi plexing chatting client")

add_executable(chapter0303MultiPlexingChattingClient)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0303MultiPlexingChattingClient LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0303MultiPlexingChattingClient PRIVATE
        main.cpp
)