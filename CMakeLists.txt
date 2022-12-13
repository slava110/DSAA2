cmake_minimum_required(VERSION 3.21)
project(DSAA2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

#[[include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)]]

include_directories(common)

add_executable(Main main.cpp)

add_executable(task_2_1 task_2_1/main.cpp)

add_executable(task_2_2 task_2_2/main.cpp)

add_executable(task_2_3 task_2_3/main.cpp)

add_executable(task_2_4 task_2_4/main.cpp)

add_executable(task_2_5 task_2_5/main.cpp)

#[[target_link_libraries(task_2_4 PRIVATE sfml-graphics)
target_compile_features(task_2_4 PRIVATE cxx_std_17)]]

#[[
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET task_2_4 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:task_2_4> $<TARGET_FILE_DIR:task_2_4> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS task_2_4)]]
