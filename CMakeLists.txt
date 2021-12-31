cmake_minimum_required(VERSION 3.17)
project(key_value_store)

set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined  -g -O1")
add_subdirectory(tcp_app_lib)
add_subdirectory(key_value_store_app)

add_executable(key_value_store main.cpp)
target_link_libraries(key_value_store key_value_store_app)
target_link_libraries(key_value_store tcp_server)