cmake_minimum_required(VERSION 3.17)

set(HEADERS
    src/app.hh
)

set(SOURCES
    src/app.cc
    src/main.cc
)

add_executable(rocketsd ${SOURCES} ${HEADERS})
set_property(TARGET rocketsd PROPERTY CXX_STANDARD 20)
target_include_directories(rocketsd PUBLIC inc/ PRIVATE src/)
target_compile_options(rocketsd PRIVATE -Wall -Wpedantic -Werror --coverage)
target_link_libraries(rocketsd PUBLIC rocketsd.module rocketsd.protocol cute.proto Qt5::Core gcov)
