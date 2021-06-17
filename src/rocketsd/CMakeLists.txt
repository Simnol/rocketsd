cmake_minimum_required(VERSION 3.17)

set(HEADERS
    src/app.hh
)

set(SOURCES
    src/app.cc
    src/main.cc
)

add_executable(rocketsd ${SOURCES} ${HEADERS})
target_include_directories(rocketsd PUBLIC inc/ PRIVATE src/)
target_standard_compile_flags(rocketsd.module)
target_link_libraries(rocketsd PUBLIC rocketsd.module rocketsd.protocol cute.proto CONAN_PKG::qt)
