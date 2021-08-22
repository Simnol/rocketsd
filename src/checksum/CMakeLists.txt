add_executable(checksum main.cc)
target_link_libraries(checksum PUBLIC rocketsd.protocol rocketsd.shared)