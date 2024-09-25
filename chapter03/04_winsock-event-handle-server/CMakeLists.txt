cmake_minimum_required(VERSION 3.27)

project(chapter0304WinSockEventHandleServer LANGUAGES CXX VERSION 0.0.1)

message("chapter03-04 win sock event handler")

add_executable(chapter0304WinSockEventHandleServer)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0304WinSockEventHandleServer LINK_PRIVATE wsock32 ws2_32)

endif ()

target_sources(chapter0304WinSockEventHandleServer PRIVATE
        main.cpp
)