cmake_minimum_required(VERSION 3.17)

set(HEADERS
    app.hh
)

set(SOURCES
    app.cc
    main.cc
)

add_executable(rocketsd ${SOURCES} ${HEADERS})
target_include_directories(rocketsd PUBLIC inc/ PRIVATE src/)
target_standard_compile_flags(rocketsd)
target_link_libraries(rocketsd PUBLIC rocketsd.module rocketsd.protocol cute.proto CONAN_PKG::qt)
