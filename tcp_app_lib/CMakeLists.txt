cmake_minimum_required(VERSION 3.17)
project(tcp_app_lib)

set(CMAKE_CXX_STANDARD 14)
add_subdirectory(tcp_utils)
add_subdirectory(tcp_server)
add_subdirectory(tcp_client)
add_subdirectory(tcp_server_app)
add_subdirectory(tcp_server_app_client)