cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("Modern ETL")


set(CMAKE_CXX_COMPILER "g++")
set(GCC_CXX_FLAGS "-std=c++11 -Wall -g -DCOMPILER_GCC -DETL_THROW_EXCEPTIONS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_CXX_FLAGS}")

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories("include")
include_directories("unittest-cpp")

file(GLOB etl_srcs "include/etl/*.cpp")
add_library(etl ${etl_srcs})

file(GLOB unittestcpp_srcs "unittest-cpp/UnitTest++/*.cpp" "unittest-cpp/UnitTest++/Posix/*.cpp")
add_library(unittestcpp ${unittestcpp_srcs})

file(GLOB etltest_srcs "test/*.cpp")
file(GLOB unittestcpp_srcs )
add_executable(etltest ${etltest_srcs})

target_link_libraries(etltest etl unittestcpp)

