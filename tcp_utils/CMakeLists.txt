cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

add_library(tcp_utils tcp_utils.cpp tcp_connection.cpp tcp_message.cpp tcp_message.h)
