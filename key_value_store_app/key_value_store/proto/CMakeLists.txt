cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

protobuf_generate_cpp(PROTO_SRC PROTO_HEADER key_value_store.proto)
add_library(key_value_store_proto SHARED ${PROTO_HEADER} ${PROTO_SRC})
target_link_libraries(key_value_store_proto ${PROTOBUF_LIBRARY})
