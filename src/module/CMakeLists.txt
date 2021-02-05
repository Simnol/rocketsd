cmake_minimum_required(VERSION 3.17.0)

set( SOURCES
    src/curses/curse_logstream.cc
    src/curses/curse_module.cc
    src/curses/curse_window.cc
    src/cute_module.cc
    src/fake_module.cc
    src/influx_module.cc
    src/module_factory.cc
    src/module.cc
    src/serial_module.cc
)

SET( HEADERS
    inc/module/module_factory.hh
    inc/module/module.hh
    src/curse_module.hh
    src/curses/curse_logstream.hh
    src/curses/curse_window.hh
    src/cute_module.hh
    src/fake_module.hh
    src/influx_module.hh
    src/serial_module.hh
)

add_library(rocketsd.module STATIC ${SOURCES} ${HEADERS})
set_property(TARGET rocketsd.module PROPERTY CXX_STANDARD 20)
target_link_libraries(rocketsd.module PUBLIC cute.util cute.log cute.proto rocketsd.protocol rocketsd.shared Qt5::Core Qt5::Network Qt5::SerialPort ${CURSES_LIBRARY})
target_include_directories(rocketsd.module PUBLIC inc PRIVATE src)
target_compile_options(rocketsd.module PRIVATE -Wall -Wpedantic -Werror --coverage)
