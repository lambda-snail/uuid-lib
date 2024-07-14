cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_BUILD_TYPE Release)

find_package(benchmark REQUIRED)

add_executable(uuid-lib-benchmark
        uuid_bm.cpp
        uuid_equality_bm.cpp
        uuid_cmp_bm.cpp)

set_target_properties(uuid-lib-benchmark PROPERTIES LINKER_LANGUAGE CXX)

if(WIN32)
    set(LAMBDASNAIL_EXTRA_LINK_ARGS "rpcrt4.lib")
endif()

target_link_libraries(uuid-lib-benchmark
        PRIVATE
        benchmark::benchmark
        benchmark::benchmark_main
        LambdaSnail::uuid-lib
        ${LAMBDASNAIL_EXTRA_LINK_ARGS}
)
