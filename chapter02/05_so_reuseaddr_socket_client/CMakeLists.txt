cmake_minimum_required(VERSION 3.27)

project(chapter0205SO_REUSEADDR_SOCKET_CLIENT LANGUAGES CXX VERSION 0.0.1)

message("chapter02-03 SO_REUSEAdDR socket Client")

add_executable(chapter0205SO_REUSEADDR_SOCKET_CLIENT)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0205SO_REUSEADDR_SOCKET_CLIENT LINK_PRIVATE wsock32 ws2_32)
endif ()

target_sources(chapter0205SO_REUSEADDR_SOCKET_CLIENT PRIVATE
        main.cpp
)