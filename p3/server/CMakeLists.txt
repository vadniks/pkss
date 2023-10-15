
cmake_minimum_required(VERSION 3.26)
project(pkss3server)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB sources CONFIGURE_DEPENDS *.cpp *.hpp)
add_executable(${PROJECT_NAME} ${sources})

include_directories(/usr/include/qt6/)
find_package(Qt6 REQUIRED COMPONENTS Core Network)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt::Core Qt::Network)
