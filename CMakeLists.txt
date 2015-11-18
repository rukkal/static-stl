cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("Static STL")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(compilation_flags "-std=c++11 -Wall -g")
    add_definitions(-DCOMPILER_GCC -DPLATFORM_LINUX)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(compilation_flags "-std=c++11 -Wall -g")
    add_definitions(-DCOMPILER_GCC -DPLATFORM_LINUX)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(compilation_flags "/W4 /EHsc")
    add_definitions(-DCOMPILER_MICROSOFT -DPLATFORM_WINDOWS)
else (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(FATAL_ERROR "not supported toolchain" )
endif()
add_definitions(-DETL_THROW_EXCEPTIONS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${compilation_flags}")

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories("include")
include_directories("unittest-cpp")

file(GLOB etl_srcs "include/etl/*.cpp" "include/etl/*.h" "include/etl/__internal/*.h")
add_library(etl ${etl_srcs})

if (${UNIX})
    file(GLOB unittestcpp_srcs   "unittest-cpp/UnitTest++/*.cpp"
                                 "unittest-cpp/UnitTest++/*.h"
                                 "unittest-cpp/UnitTest++/Posix/*.cpp"
                                 "unittest-cpp/UnitTest++/Posix/*.h")
elseif (${WIN32})
    file(GLOB unittestcpp_srcs   "unittest-cpp/UnitTest++/*.cpp"
                                 "unittest-cpp/UnitTest++/*.h"
                                 "unittest-cpp/UnitTest++/Win32/*.cpp"
                                 "unittest-cpp/UnitTest++/Win32/*.h")
else (${UNIX})
    message(FATAL_ERROR "not supported OS" )
endif()
add_library(unittestcpp ${unittestcpp_srcs})

file(GLOB etltest_srcs "test/*.cpp" "test/*.h")
add_executable(etltest ${etltest_srcs})

target_link_libraries(etltest etl unittestcpp)

