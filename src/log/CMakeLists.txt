cmake_minimum_required(VERSION 3.17)

set(SOURCES
    log.cc
    log.hh
    ostream_logsink.cc
    ostream_logsink.hh
)

add_library(cute.log STATIC ${SOURCES})
target_standard_compile_flags(cute.log)
target_link_libraries(cute.log PUBLIC cute.util)
