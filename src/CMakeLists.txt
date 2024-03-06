cmake_minimum_required(VERSION 3.22 FATAL_ERROR)
project(httpServer VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

if(NOT CMAKE_CXX_COMPILE_FEATURES)
    message(FATAL_ERROR "The compiler does not support C++17.")
endif()

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
    )
endif()

set(FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR "/usr")
include_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/include)
link_directories(${FULL_PATH_TO_MYSQL_CONNECTOR_CPP_DIR}/lib)

add_executable(${PROJECT_NAME}
    "main.cpp"
    "socket/socketUtils.cpp"
    "server/serverUtils.cpp" 
    "server/interface/interface.cpp"
    "server/global.cpp"
)

target_link_libraries(${PROJECT_NAME} mysqlcppconn)

set(DIR "${CMAKE_CURRENT_SOURCE_DIR}/interface")
set(DESTINATION_DIR "${CMAKE_BINARY_DIR}")

file(REMOVE_RECURSE ${DESTINATION_DIR})

file(COPY ${DIR} DESTINATION ${DESTINATION_DIR})

# Install targets, interface directory, and DLL files with proper permissions
file(GLOB DLL_FILES "${CMAKE_BINARY_DIR}/Release/*.dll")
install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/interface/ DESTINATION ${PROJECT_NAME}/interface FILES_MATCHING PATTERN "*" PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
install(FILES ${CMAKE_SOURCE_DIR}/uninstall.sh DESTINATION ${PROJECT_NAME} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
install(FILES ${DLL_FILES} DESTINATION ${PROJECT_NAME} PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

# Run a script to set permissions after installation
install(
    CODE "execute_process(COMMAND sudo ${CMAKE_COMMAND} -E bash ${CMAKE_CURRENT_SOURCE_DIR}/setPermissions.sh)"
)

# Include necessary system libraries
include(InstallRequiredSystemLibraries)

# Cpack installer configuration
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE)
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_GENERATOR "DEB;RPM;TGZ")
set(CPACK_PACKAGE_CONTACT "sorin.andrei.tudose@gmail.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Sorin <sorin.andrei.tudose@gmail.com")

set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/uninstall.sh")
set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/uninstall.sh")

include(CPack)