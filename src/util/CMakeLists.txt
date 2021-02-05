cmake_minimum_required(VERSION 3.17)

set(HEADERS
    inc/util/json.hh
    inc/util/time.hh
    inc/util/switch.hh
)

set(SOURCES
    src/json.cc
    src/switch.cc
)

add_library(cute.util STATIC ${SOURCES} ${HEADERS})
set_property(TARGET cute.util PROPERTY CXX_STANDARD 20)
target_include_directories(cute.util PUBLIC inc/ PRIVATE src/)
target_compile_options(cute.util PRIVATE -Wall -Wpedantic -Werror --coverage)
target_link_libraries(cute.util PUBLIC cute.log Qt5::Core)

## Tests
set( TESTS
    tests/main.cc
    tests/test_json.cc
    tests/test_switch.cc
)

add_executable(cute.util.test ${TESTS})
add_test(NAME cute.util COMMAND cute.util.test)

set_property(TARGET cute.util.test PROPERTY CXX_STANDARD 20)
target_compile_options(cute.util.test PUBLIC --coverage)
target_link_options(cute.util.test PUBLIC --coverage)
target_link_libraries(cute.util.test PUBLIC cute.util gcov gtest)