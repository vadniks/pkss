
cmake_minimum_required(VERSION 3.26)
project(pkss1 C)

add_compile_options(-O3)
add_link_options(-s)

set(CMAKE_C_STANDARD 11)

add_executable(pkss1 main.c)
target_link_libraries(${PROJECT_NAME} SDL2 SDL2_ttf)

file(COPY RobotoMono-Regular.ttf DESTINATION ${CMAKE_BINARY_DIR})
