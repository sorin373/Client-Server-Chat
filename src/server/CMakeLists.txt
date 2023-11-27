cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

add_executable(PinnacleServer
    "server.cpp"
)

target_include_directories(PinnacleServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../socket)
target_link_libraries(PinnacleServer PRIVATE Socket)