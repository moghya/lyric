cmake_minimum_required(VERSION 3.17)
project(lyric)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined  -g -O1")
include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

add_subdirectory(third_party/asyncplusplus)
add_subdirectory(third_party/spdlog)
add_subdirectory(tcp_app_lib)
add_subdirectory(key_value_store_app)

add_executable(app app.cpp)
target_link_libraries(app key_value_store_app)
target_link_libraries(app spdlog::spdlog_header_only)

add_executable(client client.cpp)
target_link_libraries(client key_value_store_app_client)