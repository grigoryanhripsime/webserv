cmake_minimum_required(VERSION 3.10)
project(webserv)

set(CMAKE_CXX_STANDARD 98)

# Добавь сюда все .cpp файлы проекта
file(GLOB_RECURSE SOURCES
    srcs/*.cpp
    *.cpp
)

add_executable(webserv ${SOURCES})

