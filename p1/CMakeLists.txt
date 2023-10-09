
cmake_minimum_required(VERSION 3.26)
project(pkss1 C)

set(CMAKE_C_STANDARD 11)

add_executable(pkss1 main.c)
target_link_libraries(${PROJECT_NAME} ncurses)
