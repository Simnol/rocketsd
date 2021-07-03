cmake_minimum_required(VERSION 3.17)

set(HEADERS
    inc/protocol/protocol.hh
    inc/protocol/protocol_parser.hh
    inc/protocol/name.hh
)

set(SOURCES
    src/protocol.cc
    src/protocol_parser.cc
    src/name.cc
)

add_library(rocketsd.protocol STATIC ${SOURCES} ${HEADERS})
target_include_directories(rocketsd.protocol PUBLIC inc/ PRIVATE src/)
target_standard_compile_flags(rocketsd.protocol)
target_link_libraries(rocketsd.protocol PUBLIC cute.log CONAN_PKG::qt)

## Tests
set( TESTS
    tests/main.cc
    tests/test_protocol.cc
)

add_executable(rocketsd.protocol.test ${TESTS})
gtest_discover_tests(rocketsd.protocol.test)

target_standard_compile_flags(rocketsd.protocol.test)
target_link_libraries(rocketsd.protocol.test PUBLIC rocketsd.protocol CONAN_PKG::gtest)