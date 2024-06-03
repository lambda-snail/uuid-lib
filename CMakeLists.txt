cmake_minimum_required(VERSION 3.28)
project(uuid-lib)

set(CMAKE_CXX_STANDARD 20)

set(UUID_LIB_BUILD_EXAMPLE 1)

#if(UUID_LIB_BUILD_EXAMPLE)
    add_subdirectory(example)
#endif ()

set(SRC src)
add_library(uuid-lib
    ${SRC}/xoroshiro128.cpp
    ${SRC}/uuid.cpp
    ${SRC}/uuid_spec.cpp
)

target_include_directories(uuid-lib PUBLIC include)
