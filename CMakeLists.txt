cmake_minimum_required(VERSION 2.8)
project(Fred&Cie C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_COMPILER "gcc")


find_package(Threads REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -lpthread -Werror -g")

enable_testing()

add_subdirectory(src)
add_subdirectory(tst)

add_executable(thread_test testthread.c)