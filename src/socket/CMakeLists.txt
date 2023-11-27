cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

add_library(socketUtils socketUtils.cpp)

add_library(Socket socketUtils.cpp)
target_include_directories(Socket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})