cmake_minimum_required(VERSION 3.21)
project(DSAA2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

include_directories(common)

add_executable(Main main.cpp)

add_executable(task_2_1 task_2_1/main.cpp)

add_executable(task_2_2 task_2_2/main.cpp)

add_executable(task_2_3 task_2_3/main.cpp)

add_executable(task_2_3_g task_2_3/main_g.cpp)

add_executable(task_2_4 task_2_4/main.cpp)

add_executable(task_2_5 task_2_5/main.cpp)

add_executable(task_2_6 task_2_6/main.cpp)