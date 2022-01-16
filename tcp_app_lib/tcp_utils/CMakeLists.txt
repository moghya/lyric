cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

add_library(tcp_utils message.cpp tcp_socket.cpp tcp_connection.cpp tcp_message.cpp tcp_utils.cpp)
target_link_libraries(tcp_utils spdlog::spdlog_header_only)