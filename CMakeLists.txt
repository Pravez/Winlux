cmake_minimum_required(VERSION 2.8)
project(Fred&Cie C CXX)

set(CMAKE_C_STANDARD 99)

find_package(Threads REQUIRED)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -lpthread -Werror")

enable_testing()

add_subdirectory(src)
add_subdirectory(tst)