cmake_minimum_required(VERSION 3.28)
project(uuid-lib)

set(CMAKE_CXX_STANDARD 20)

set(UUID_LIB_BUILD_EXAMPLE 1 CACHE BOOL "Build the example project")
set(UUID_LIB_BUILD_BENCHMARK 1 CACHE BOOL "Build the benchmark project")

set(UUID_LIB_USE_SIMDI 1 CACHE BOOL "Enable SIMD instructions if available on the target hardware")

add_subdirectory(test)

if(UUID_LIB_USE_SIMDI)
    MESSAGE("Adding SIMD support")
    add_compile_definitions(UUID_LIB_USE_SIMD)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        MESSAGE("Clang or GCC: Adding compiler flags for using simd intrinsics")
        SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -march=native")
        #elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    endif()
endif()

#if(UUID_LIB_BUILD_EXAMPLE)
    add_subdirectory(example)
#endif ()

#if(UUID_LIB_BUILD_BENCHMARK)
    add_subdirectory(benchmark)
#endif ()

set(SRC src)
add_library(uuid-lib
    ${SRC}/uuid.cpp
    ${SRC}/uuid_factory.cpp
)

add_library(LambdaSnail::uuid-lib ALIAS uuid-lib)

target_include_directories(uuid-lib PUBLIC include)
