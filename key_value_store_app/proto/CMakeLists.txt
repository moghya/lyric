cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 14)

set(Protobuf_IMPORT_DIRS ../key_value_store/proto)
protobuf_generate_cpp(PROTO_SRC PROTO_HEADER key_value_store_app.proto)
add_library(key_value_store_app_proto SHARED ${PROTO_HEADER} ${PROTO_SRC})
target_link_libraries(key_value_store_app_proto key_value_store_proto)
target_link_libraries(key_value_store_app_proto ${PROTOBUF_LIBRARY})