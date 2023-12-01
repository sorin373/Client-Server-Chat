cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

# Define the executable
add_executable(PinnacleServer "main.cpp" "serverUtils.cpp")
target_include_directories(PinnacleServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../socket)
target_link_libraries(PinnacleServer PRIVATE Socket)

# Define the shared library
set(SRC_FILES serverUtils.cpp serverUtilsBindings.cpp)
add_library(server SHARED ${SRC_FILES})

# Link the SocketUtils library to the server shared library
target_link_libraries(server PRIVATE Socket)
