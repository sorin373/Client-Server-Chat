cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

add_executable(PinnacleServer 
    "main.cpp" 
    "serverUtils.cpp" 
    "database/database.cpp" 
    "../socket/socketUtils.cpp"
)
