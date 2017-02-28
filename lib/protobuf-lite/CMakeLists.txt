cmake_minimum_required(VERSION 3.7.0)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(libprotobuf_lite_files
  ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/extension_set.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/generated_message_util.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/message_lite.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/repeated_field.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/wire_format_lite.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/stubs/atomicops_internals_x86_gcc.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/stubs/atomicops_internals_x86_msvc.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/stubs/common.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/stubs/once.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/stubs/stringprintf.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/io/coded_stream.cc 
  ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/io/zero_copy_stream.cc   
  ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/io/zero_copy_stream_impl_lite.cc
)

FILE(GLOB PROTOBUF_INCLUDE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/*.h ${CMAKE_CURRENT_SOURCE_DIR}/google/protobuf/stubs/*.h)

add_library(libprotobuf-lite
  ${PROTOBUF_INCLUDE_FILE}
  ${libprotobuf_lite_files}
)
