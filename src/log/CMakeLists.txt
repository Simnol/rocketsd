cmake_minimum_required(VERSION 3.17)

set(SOURCES
    src/log.cc
)

set(HEADERS
    inc/log/log.hh
)

add_library(cute.log STATIC ${SOURCES} ${HEADERS})
set_property(TARGET cute.log PROPERTY CXX_STANDARD 20)
target_include_directories(cute.log PUBLIC inc/ PRIVATE src/)
target_compile_options(cute.log PRIVATE -Wall -Wpedantic -Werror)
