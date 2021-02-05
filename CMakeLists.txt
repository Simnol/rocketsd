cmake_minimum_required(VERSION 3.7.0)
project(rocketsd)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
cmake_policy(SET CMP0100 NEW)

set (CMAKE_PREFIX_PATH /opt/Qt/5.14.1/gcc_64/)

find_package(Qt5 COMPONENTS Core Network Xml SerialPort REQUIRED)
find_package(Protobuf REQUIRED)
find_package(nlohmann_json 3.7.0 REQUIRED)
find_package(Curses REQUIRED)

include_directories(${CURSES_INCLUDE_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(libs)

enable_testing()

add_subdirectory(src)