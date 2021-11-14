cmake_minimum_required(VERSION 3.17.0)

set( SOURCES
    cute_module.cc
    cute_module.hh
    qt_socket_adapter.cc
    qt_socket_adapter.hh
)

add_library(rocketsd.module.cute STATIC ${SOURCES} ${HEADERS})
target_standard_compile_flags(rocketsd.module.cute)
target_link_libraries(rocketsd.module.cute PUBLIC cute.util cute.log cute.proto rocketsd.protocol rocketsd.shared CONAN_PKG::qt)
