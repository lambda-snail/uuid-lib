cmake_minimum_required(VERSION 3.28)
project(GuidDev)

set(CMAKE_CXX_STANDARD 20)

add_executable(GuidDev main.cpp
        xoroshiro128.cpp xoroshiro128.h
        uuid.h uuid.cpp
)
