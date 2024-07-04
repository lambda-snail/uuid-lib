cmake_minimum_required(VERSION 3.28)
project(uuid-lib)

set(CMAKE_CXX_STANDARD 20)

set(UUID_LIB_BUILD_EXAMPLE 1)
set(UUID_LIB_BUILD_BENCHMARK 1)

add_subdirectory(test)

#if(UUID_LIB_BUILD_EXAMPLE)
    add_subdirectory(example)
#endif ()

#if(UUID_LIB_BUILD_BENCHMARK)
    add_subdirectory(benchmark)
#endif ()

set(SRC src)
add_library(uuid-lib
    ${SRC}/xoroshiro128.cpp
    ${SRC}/uuid.cpp
    ${SRC}/uuid_spec.cpp
    ${SRC}/uuid_factory.cpp
)

add_library(LambdaSnail::uuid-lib ALIAS uuid-lib)

target_include_directories(uuid-lib PUBLIC include)
