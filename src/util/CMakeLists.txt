cmake_minimum_required(VERSION 3.17)

set(SOURCES
    json.cc
    json.hh
    switch.cc
    switch.hh
    time.hh
)

add_library(cute.util STATIC ${SOURCES})
target_standard_compile_flags(cute.util)
target_link_libraries(cute.util PUBLIC cute.log CONAN_PKG::qt CONAN_PKG::nlohmann_json)

## Tests
set(TESTS
    tests/main.cc
    tests/test_json.cc
    tests/test_switch.cc
)

add_executable(cute.util.test ${TESTS})
gtest_discover_tests(cute.util.test)

target_standard_compile_flags(cute.util.test)
target_link_libraries(cute.util.test PUBLIC cute.util CONAN_PKG::gtest)