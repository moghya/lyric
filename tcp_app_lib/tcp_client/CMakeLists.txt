cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

add_library(tcp_client tcp_client.cpp)
target_link_libraries(tcp_client tcp_utils)
target_link_libraries(tcp_client  spdlog::spdlog_header_only)

add_executable(tcp_client_test tcp_client_test.cpp)
target_link_libraries(tcp_client_test tcp_utils)
target_link_libraries(tcp_client_test tcp_client)
target_link_libraries(tcp_client_test spdlog::spdlog_header_only)
