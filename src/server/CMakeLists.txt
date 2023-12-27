project(httpServer)

set(FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR "/usr")
include_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/include)
link_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/lib)

add_executable(${PROJECT_NAME}
    "main.cpp" 
    "serverUtils.cpp" 
    "database/database.cpp" 
    "interface/interface.cpp"
    "../socket/socketUtils.cpp"
    "declarations.cpp"
)

target_link_libraries(${PROJECT_NAME} mysqlcppconn)
