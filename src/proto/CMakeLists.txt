cmake_minimum_required(VERSION 3.17)

SET( HEADERS
    inc/proto/packet.hh
    inc/proto/delimited_protobuf_stream.hh
)

set( SOURCES
    src/packet.cc
)

SET( PROTO
    packet.proto
)

protobuf_generate_cpp(PROTO_SOURCES PROTO_HEADERS ${PROTO})

set_property(SOURCE ${PROTO_HEADERS} PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE ${PROTO_SOURCES} PROPERTY SKIP_AUTOGEN ON)

add_library(cute.proto STATIC ${HEADERS} ${SOURCES} ${PROTO_SOURCES} ${PROTO_HEADERS})
target_compile_features(cute.proto PUBLIC cxx_std_20)
target_include_directories(cute.proto PUBLIC inc/ ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(cute.proto PUBLIC protobuf::libprotobuf)

## Tests
set( TESTS
    tests/main.cc
    tests/test_packet.cc
    tests/test_stream.cc
)

add_executable(cute.proto.test ${TESTS})
add_test(NAME cute.proto COMMAND cute.proto.test)

target_compile_options(cute.proto.test PUBLIC --coverage)
target_link_options(cute.proto.test PUBLIC --coverage)
target_link_libraries(cute.proto.test PUBLIC cute.proto gcov gtest)
