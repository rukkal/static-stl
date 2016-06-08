
SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -Werror -std=c++11" CACHE STRING "Common flags for C++ compiler")
SET(CMAKE_CXX_FLAGS_DEBUG "-g3" CACHE STRING "Debug flags for C++ compiler")
SET(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 -flto" CACHE STRING "Release flags for C++ compiler")
SET(CMAKE_CXX_FLAGS_MINSIZEREL "-DNDEBUG -Os -flto" CACHE STRING "Minimum size release flags for C++ compiler")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-DNDEBUG -Og -g3" CACHE STRING "Release with debug info flags for C++ compiler")

SET(CMAKE_C_FLAGS "-Wall -Wpedantic -Werror" CACHE STRING "Common flags for C compiler")
SET(CMAKE_C_FLAGS_DEBUG "-g3" CACHE STRING "Debug flags for C compiler")
SET(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O3 -flto" CACHE STRING "Release flags for C compiler")
SET(CMAKE_C_FLAGS_MINSIZEREL "-DNDEBUG -Os -flto" CACHE STRING "Minimum size release flags for C compiler")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "-DNDEBUG -Og -g3" CACHE STRING "Release with debug info flags for C compiler")

SET(CMAKE_EXE_LINKER_FLAGS "-flto" CACHE STRING "General flags for linker")

