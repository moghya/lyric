cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

add_library(key_value_store key_value_store.cpp)
target_link_libraries(key_value_store spdlog::spdlog_header_only)