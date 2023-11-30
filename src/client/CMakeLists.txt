cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

add_executable(PinnacleClient
    "main.cpp"
    "clientUtils.cpp"
)

target_include_directories(PinnacleClient PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../socket)
target_link_libraries(PinnacleClient PRIVATE Socket)