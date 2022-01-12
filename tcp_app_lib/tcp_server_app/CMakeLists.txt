cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

add_library(tcp_server_app tcp_server_app.cpp)
target_link_libraries(tcp_server_app tcp_utils)
target_link_libraries(tcp_server_app tcp_server)
target_link_libraries(tcp_server_app spdlog::spdlog_header_only)

add_executable(tcp_server_app_test tcp_server_app_test.cpp)
target_link_libraries(tcp_server_app_test tcp_server_app)
