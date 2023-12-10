cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

add_executable(PinnacleClient
    "main.cpp"
    "clientUtils.cpp"
    "../socket/socketUtils.cpp"
)