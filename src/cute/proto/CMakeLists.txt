cmake_minimum_required(VERSION 3.17)

set(SOURCES
    packet.hh
    delimited_protobuf_stream.hh
    packet.cc
)

SET(PROTO
    packet.proto
)

protobuf_generate_cpp(PROTO_SOURCES PROTO_HEADERS ${PROTO})

set_property(SOURCE ${PROTO_HEADERS} PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE ${PROTO_SOURCES} PROPERTY SKIP_AUTOGEN ON)

add_library(cute.proto STATIC ${SOURCES} ${PROTO_SOURCES} ${PROTO_HEADERS})
target_include_directories(cute.proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_compile_features(cute.proto PUBLIC cxx_std_20)
target_link_libraries(cute.proto PUBLIC CONAN_PKG::protobuf)

protobuf_generate_python(PROTO_PYTHON ${PROTO})
add_custom_target(cute.proto.python DEPENDS ${PROTO_PYTHON})

# Tests
set(TESTS
    tests/main.cc
    tests/test_packet.cc
    tests/test_stream.cc
)

add_executable(cute.proto.test ${TESTS})
target_compile_features(cute.proto PUBLIC cxx_std_20)
target_link_libraries(cute.proto.test PUBLIC cute.proto CONAN_PKG::gtest)
gtest_discover_tests(cute.proto.test)
