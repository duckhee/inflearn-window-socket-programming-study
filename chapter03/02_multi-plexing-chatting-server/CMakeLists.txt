cmake_minimum_required(VERSION 3.27)

project(chapter0302MultiPlexingChattingServer LANGUAGES CXX VERSION 0.0.1)

message("chapter03-02 multi plexing chatting server using select")

add_executable(chapter0302MultiPlexingChattingServer)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0302MultiPlexingChattingServer LINK_PRIVATE wsock32 ws2_32)
endif ()


target_sources(chapter0302MultiPlexingChattingServer PRIVATE
        main.cpp
)