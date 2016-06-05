cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("static-stl")

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(compilation_flags "-std=c++11 -Wall -g")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(compilation_flags "-std=c++11 -Wall -g")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    set(compilation_flags "/W4 /EHsc")
else (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(FATAL_ERROR "not supported toolchain" )
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${compilation_flags}")

set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories("Catch/include")
include_directories("include")
include_directories("test")

file(GLOB sstl_srcs "include/sstl/*.cpp" "include/sstl/*.h" "include/sstl/__internal/*.cpp" "include/sstl/__internal/*.h")
file(GLOB test_srcs "test/*.cpp" "test/*.h" ${sstl_srcs})

add_executable(test-sstl ${test_srcs})
target_link_libraries(test-sstl)

add_executable(test-sstl-noexceptions ${test_srcs})
set_target_properties(test-sstl-noexceptions PROPERTIES COMPILE_DEFINITIONS "_SSTL_NOEXCEPTIONS_TEST")
target_link_libraries(test-sstl-noexceptions)
