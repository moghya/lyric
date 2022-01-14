cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})

protobuf_generate_cpp(PROTO_SRC PROTO_HEADER key_value_store_app.proto)
add_library(key_value_store_app_proto STATIC ${PROTO_HEADER} ${PROTO_SRC})
target_link_libraries(key_value_store_app_proto ${PROTOBUF_LIBRARY})