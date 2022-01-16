cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

add_subdirectory(proto)
target_include_directories(key_value_store_app_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(utils)
add_subdirectory(key_value_store)
add_subdirectory(app)
add_subdirectory(client)

add_library(key_value_store_app_client
            key_value_store_app_client.cpp
            utils.cpp)
target_link_libraries(key_value_store_app_client key_value_store_app_proto)
target_link_libraries(key_value_store_app_client tcp_utils)
target_link_libraries(key_value_store_app_client tcp_server_app_client)