cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("static-stl")

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
