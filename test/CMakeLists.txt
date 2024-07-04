cmake_minimum_required(VERSION 3.28)

enable_testing()

set(CMAKE_CXX_STANDARD 20)

project(uuid-lib-test CXX)

find_package(GTest REQUIRED)

add_executable(uuid-lib-test main.cpp uuid_tests.cpp)

#target_include_directories(uuid_lib_test PRIVATE ${GTEST_INCLUDE_DIR})
#include_directories(${GTEST_INCLUDE_DIR})

set_target_properties(uuid-lib-test PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(uuid-lib-test
        PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        GTest::gmock_main

        LambdaSnail::uuid-lib)

add_test(AllTestsInMain uuid-lib-test)