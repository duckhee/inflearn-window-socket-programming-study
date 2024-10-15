cmake_minimum_required(VERSION 3.27)

project(chapter0701DNSQuery LANGUAGES CXX VERSION 0.0.1)

message("chapter07-01 DNS Query")

add_executable(chapter0701DNSQuery)

if (WIN32 OR WIN64)
    target_link_libraries(chapter0701DNSQuery LINK_PRIVATE wsock32 ws2_32)
endif ()


target_sources(chapter0701DNSQuery PRIVATE
    main.cpp
)