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
set_property(TARGET rocketsd.protocol PROPERTY CXX_STANDARD 20)
target_include_directories(rocketsd.protocol PUBLIC inc/ PRIVATE src/)
target_compile_options(rocketsd.protocol PRIVATE -Wall -Wpedantic -Werror --coverage)
target_link_libraries(rocketsd.protocol PUBLIC cute.log Qt5::Core Qt5::Xml)

## Tests
set( TESTS
    tests/main.cc
    tests/test_protocol.cc
)

add_executable(rocketsd.protocol.test ${TESTS})
add_test(NAME rocketsd.protocol COMMAND rocketsd.protocol.test)

set_property(TARGET rocketsd.protocol.test PROPERTY CXX_STANDARD 20)
target_compile_options(rocketsd.protocol.test PUBLIC --coverage)
target_link_options(rocketsd.protocol.test PUBLIC --coverage)
target_link_libraries(rocketsd.protocol.test PUBLIC rocketsd.protocol gcov gtest)