
cmake_minimum_required(VERSION 3.26)
project(pkss2server C)

set(CMAKE_C_STANDARD 11)

add_compile_options(-O3)
add_link_options(-s)

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} SDL2 SDL2_net)
