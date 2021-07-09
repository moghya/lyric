cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

add_library(tcp_server tcp_server.cpp)
target_link_libraries(tcp_server tcp_server_app)
target_link_libraries(tcp_server tcp_utils)

