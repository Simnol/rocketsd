cmake_minimum_required(VERSION 3.17)

set(HEADERS
    shared/interfaces/id.h
    shared/interfaces/enums.h
    shared/interfaces/radio/radio_packet.h
)

set(SOURCES
    shared/interfaces/radio/radio_packet.c
)

add_library(rocketsd.shared STATIC ${SOURCES} ${HEADERS})
target_include_directories(rocketsd.shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_standard_compile_flags(rocketsd.shared)