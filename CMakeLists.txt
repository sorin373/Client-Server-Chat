cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(NetPinnacle VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(NOT CMAKE_CXX_COMPILE_FEATURES)
    message(FATAL_ERROR "The compiler does not support C++17.")
endif()

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
    )
endif()

add_subdirectory(src/server)
add_subdirectory(src/client)
add_subdirectory(src/socket)
