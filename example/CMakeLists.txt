cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)

project(uuid-lib-example)

add_executable(uuid-lib-example main.cpp)

target_link_libraries(uuid-lib-example PUBLIC LambdaSnail::uuid-lib)