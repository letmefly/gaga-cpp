cmake_minimum_required(VERSION 3.7.0)

project(Gaga)

set(CMAKE_CXX_STANDARD 11)
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

SET(RAKNET_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/lib/raknet/include/RakNet)
SET(PROTOBUF_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/lib/protobuf-lite)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR})
#include_directories(${PROJECT_SOURCE_DIR}/lib/protobuf-lite)
#include_directories(${PROJECT_SOURCE_DIR}/src)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib/raknet)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/protobuf-lite)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
