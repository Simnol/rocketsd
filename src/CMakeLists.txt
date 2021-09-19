include_directories(AFTER ${CMAKE_SOURCE_DIR}/src)

add_subdirectory(checksum)
add_subdirectory(log)
add_subdirectory(proto)
add_subdirectory(protocol)
add_subdirectory(rocketsd)
add_subdirectory(shared)
add_subdirectory(util)