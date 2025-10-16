cmake_minimum_required(VERSION 3.21)

project(netcore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(BASE_BUILD_TESTS OFF CACHE BOOL "Disable tests for base" FORCE)
add_subdirectory(libs/base)

add_library(${PROJECT_NAME} STATIC
	src/io/server_socket.cpp
	src/io/socket_stream.cpp
	src/io/socketbuf.cpp
	src/accept_handler_impl.cpp
	src/app_protocol_handler_registry.cpp
	src/connection.cpp
	src/ip_address.cpp
	src/server_impl.cpp
	src/server.cpp
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PUBLIC base)
