cmake_minimum_required(VERSION 3.7.0)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/server)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/proto)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/client)

add_subdirectory(proto)
add_subdirectory(server)
add_subdirectory(client)


