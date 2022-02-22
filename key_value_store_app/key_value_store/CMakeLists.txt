cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

add_subdirectory(proto)
target_include_directories(key_value_store_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

add_library(key_value_store key_value_store.cpp)
target_link_libraries(key_value_store key_value_store_proto)
target_link_libraries(key_value_store spdlog::spdlog_header_only)