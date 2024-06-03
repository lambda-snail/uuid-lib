cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)

find_package(benchmark REQUIRED)

add_executable(uuid-lib-benchmark uuid_factory_b.cpp)

set_target_properties(uuid-lib-benchmark PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(uuid-lib-benchmark
        PRIVATE
        benchmark::benchmark
        benchmark::benchmark_main
        LambdaSnail::uuid-lib
)
