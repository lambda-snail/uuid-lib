cmake_minimum_required(VERSION 3.28)
project(uuid-lib)

set(CMAKE_CXX_STANDARD 20)

set(SRC src)

add_executable(uuid-lib main.cpp
        ${SRC}/xoroshiro128.cpp
        ${SRC}/uuid.cpp
        ${SRC}/uuid_spec.cpp
)

target_include_directories(uuid-lib PUBLIC include)
