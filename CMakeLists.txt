cmake_minimum_required(VERSION 4.1)
project(analyse_v2)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenCV REQUIRED)

add_executable(analyse_v2 main.cpp)

target_link_libraries(analyse_v2 PRIVATE ${OpenCV_LIBS})
target_include_directories(analyse_v2 PRIVATE ${OpenCV_INCLUDE_DIRS})