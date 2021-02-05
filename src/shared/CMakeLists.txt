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
set_property(TARGET rocketsd.shared PROPERTY CXX_STANDARD 20)
target_include_directories(rocketsd.shared PUBLIC .)
target_compile_options(rocketsd.shared  PRIVATE -Wall -Wpedantic -Werror --coverage)