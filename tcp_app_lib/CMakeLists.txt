cmake_minimum_required(VERSION 3.17)
project(tcp_app_lib)

set(CMAKE_CXX_STANDARD 14)
include_directories(../third_party/spdlog/include/spdlog/)

add_subdirectory(tcp_utils)
add_subdirectory(tcp_server)
add_subdirectory(tcp_client)
add_subdirectory(tcp_server_app)
add_subdirectory(tcp_server_app_client)

add_executable(tcp_app_lib_tester test.cpp)
target_link_libraries(tcp_app_lib_tester  tcp_utils)
target_link_libraries(tcp_app_lib_tester  tcp_client)
target_link_libraries(tcp_app_lib_tester  tcp_server)
target_link_libraries(tcp_app_lib_tester  tcp_server_app)
target_link_libraries(tcp_app_lib_tester  tcp_server_app_client)
target_link_libraries(tcp_app_lib_tester  spdlog::spdlog_header_only)