cmake_minimum_required(VERSION 3.10)

project(Chess_Engine C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

file(GLOB SRC_FILES src/*.c tests/*.c)
add_executable(main main.c ${SRC_FILES})