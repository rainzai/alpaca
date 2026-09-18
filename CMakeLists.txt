cmake_minimum_required(VERSION 4.4.3)
project(alpaca)

find_package(raylib REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} raylib)