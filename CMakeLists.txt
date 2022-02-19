cmake_minimum_required(VERSION 3.0)

project(cm2024)

add_executable(cm2024 main.cpp)

install(TARGETS cm2024 RUNTIME DESTINATION bin)
