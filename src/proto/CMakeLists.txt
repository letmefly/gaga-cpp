cmake_minimum_required(VERSION 3.0.0)

include_directories(${PROTOBUF_INCLUDE_PATH})

FILE(GLOB ALL_PROTO_SRC ${CMAKE_CURRENT_SOURCE_DIR}/protosrc/*.proto)

set(PROTOC_EXECUTABLE ${PROJECT_SOURCE_DIR}/tools/protoc/protoc.exe)

foreach(protoName ${ALL_PROTO_SRC})
	execute_process(COMMAND ${PROTOC_EXECUTABLE} -I=${CMAKE_CURRENT_SOURCE_DIR}/protosrc 
												 --cpp_out=${CMAKE_CURRENT_SOURCE_DIR}/protomsg
												 ${protoName}
	                OUTPUT_VARIABLE PROTOC_OUTPUT
	)
endforeach()

FILE(GLOB PROTO_HEADER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/protomsg/*.h)
FILE(GLOB PROTO_CPP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/protomsg/*.cc)

add_library(protomsg
	${PROTO_HEADER_FILE}
	${PROTO_CPP_FILE}
)

target_link_libraries(protomsg
	libprotobuf-lite
)
