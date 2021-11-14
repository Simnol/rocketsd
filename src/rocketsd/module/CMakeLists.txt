cmake_minimum_required(VERSION 3.17.0)

set(SOURCES
    dumper_module.hh
    fake_module.cc
    fake_module.hh
    influx_module.cc
    influx_module.hh
    module_factory.cc
    module_factory.hh
    module.cc
    module.hh
    serial_module.cc
    serial_module.hh
)

add_library(rocketsd.module STATIC ${SOURCES} ${HEADERS})
target_standard_compile_flags(rocketsd.module)
target_link_libraries(rocketsd.module PUBLIC cute.util cute.log cute.proto rocketsd.protocol rocketsd.shared rocketsd.module.cute CONAN_PKG::qt)

add_subdirectory(cute)