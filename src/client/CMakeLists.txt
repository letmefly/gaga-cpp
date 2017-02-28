cmake_minimum_required(VERSION 3.7.0)

include_directories(${PROTOBUF_INCLUDE_PATH})
include_directories(${RAKNET_INCLUDE_PATH})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../proto/protomsg)

file( GLOB SERVER_SRC *.c *.cpp *.cc *.h *.hpp )
file( GLOB COMMON_SRC ../common/*.c ../common/*.cpp ../common/*.cc ../common/*.h ../common/*.hpp )

add_executable(gaga-client
	${SERVER_SRC}
	${COMMON_SRC}
	#${PROTO_HEADER_FILE}
	#${PROTO_CPP_FILE}
)

target_link_libraries(gaga-client
	RakNetLibStatic
	protomsg
)
