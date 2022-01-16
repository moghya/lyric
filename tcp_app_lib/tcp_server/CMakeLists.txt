cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

add_library(tcp_server tcp_server.cpp)
target_link_libraries(tcp_server tcp_utils)
target_link_libraries(tcp_server spdlog::spdlog_header_only)

add_executable(tcp_server_test tcp_server_test.cpp)
target_link_libraries(tcp_server_test tcp_utils)
target_link_libraries(tcp_server_test tcp_server)
target_link_libraries(tcp_server_test spdlog::spdlog_header_only)
