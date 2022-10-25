cmake_minimum_required(VERSION 3.21)
project(DSAA2)

set(CMAKE_CXX_STANDARD 11)

include_directories(common)

add_executable(Main main.cpp)

add_executable(task_2_1 task_2_1/main.cpp)

add_executable(task_2_2 task_2_2/main.cpp)