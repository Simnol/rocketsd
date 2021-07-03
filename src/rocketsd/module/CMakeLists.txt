cmake_minimum_required(VERSION 3.17.0)

set( SOURCES
    fake_module.cc
    influx_module.cc
    module_factory.cc
    module.cc
    serial_module.cc
)

SET( HEADERS
    module_factory.hh
    module.hh
    fake_module.hh
    influx_module.hh
    serial_module.hh
)

add_library(rocketsd.module STATIC ${SOURCES} ${HEADERS})
target_include_directories(rocketsd.module PUBLIC inc PRIVATE src)
target_standard_compile_flags(rocketsd.module)
target_link_libraries(rocketsd.module PUBLIC cute.util cute.log cute.proto rocketsd.protocol rocketsd.shared rocketsd.module.cute CONAN_PKG::qt)

add_subdirectory(cute)