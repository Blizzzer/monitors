cmake_minimum_required(VERSION 3.15)
project(zad4)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_executable(zad4 main.cpp monitor.h)