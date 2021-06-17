cmake_minimum_required(VERSION 3.17)

set(SOURCES
    src/log.cc
)

set(HEADERS
    inc/log/log.hh
)

add_library(cute.log STATIC ${SOURCES} ${HEADERS})
target_include_directories(cute.log PUBLIC inc/ PRIVATE src/)
target_standard_compile_flags(cute.log)
target_link_libraries(cute.log PUBLIC cute.util)
