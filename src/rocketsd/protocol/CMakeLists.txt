cmake_minimum_required(VERSION 3.17)

set(SOURCES
    name.cc
    name.hh
    protocol_parser.cc
    protocol_parser.hh
    protocol.cc
    protocol.hh
)

add_library(rocketsd.protocol STATIC ${SOURCES})
target_standard_compile_flags(rocketsd.protocol)
target_link_libraries(rocketsd.protocol PUBLIC cute.log CONAN_PKG::qt)

## Tests
set(TESTS
    tests/main.cc
    tests/test_protocol.cc
)

add_executable(rocketsd.protocol.test ${TESTS})
gtest_discover_tests(rocketsd.protocol.test)

target_standard_compile_flags(rocketsd.protocol.test)
target_link_libraries(rocketsd.protocol.test PUBLIC rocketsd.protocol CONAN_PKG::gtest)