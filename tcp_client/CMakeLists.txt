cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

add_library(tcp_client tcp_client.cpp)
target_link_libraries(tcp_client tcp_utils)

