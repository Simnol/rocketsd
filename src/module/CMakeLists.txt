cmake_minimum_required(VERSION 3.17.0)

set( SOURCES
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
    src/cute_module.hh
    src/fake_module.hh
    src/influx_module.hh
    src/serial_module.hh
)

add_library(rocketsd.module STATIC ${SOURCES} ${HEADERS})
target_include_directories(rocketsd.module PUBLIC inc PRIVATE src)
target_standard_compile_flags(rocketsd.module)
target_link_libraries(rocketsd.module PUBLIC cute.util cute.log cute.proto rocketsd.protocol rocketsd.shared CONAN_PKG::qt)
