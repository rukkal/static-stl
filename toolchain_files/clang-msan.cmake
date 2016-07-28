
include("${CMAKE_CURRENT_LIST_DIR}/clang.cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory -fsanitize-memory-track-origins -fsanitize-memory-use-after-dtor")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=memory -fsanitize-memory-track-origins -fsanitize-memory-use-after-dtor")

