cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(key_value_store)

add_library(key_value_store_app
            key_value_store_app.cpp
            utils.cpp)
target_link_libraries(key_value_store_app key_value_store)
target_link_libraries(key_value_store_app tcp_utils)
target_link_libraries(key_value_store_app tcp_server_app)

add_library(key_value_store_app_client
            key_value_store_app_client.cpp
            utils.cpp)
target_link_libraries(key_value_store_app_client tcp_utils)
target_link_libraries(key_value_store_app_client tcp_server_app_client)