cmake_minimum_required(VERSION 3.17.0)
project(rocketsd)

# Third parties
include("${CMAKE_BINARY_DIR}/conan_paths.cmake")
include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)

# Qt5
include(conan_qt_executables_variables)
include(Qt5CoreMacros)
include(Qt5WidgetsMacros)
cmake_policy(SET CMP0100 NEW)

if (UNIX)
    target_compile_options(CONAN_PKG::qt INTERFACE -fPIC)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# GTest
find_package(GTest)
enable_testing()

# Installation stuff
if (UNIX)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "David Bourgault")
elseif(MSVC)
    set(CPACK_GENERATOR "WIX")
endif()

include(CPack)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# Functions
function(target_os_compile_options)
    cmake_parse_arguments(
        OS_COMPILE_OPTIONS
        ""
        "TARGET"
        "LINUX;WINDOWS;ALL"
        ${ARGN}
    )

    if (UNIX)
        target_compile_options(${OS_COMPILE_OPTIONS_TARGET} PRIVATE ${OS_COMPILE_OPTIONS_LINUX})
    endif()

    if (MSVC)
        target_compile_options(${OS_COMPILE_OPTIONS_TARGET} PRIVATE ${OS_COMPILE_OPTIONS_WINDOWS})
    endif()

    target_compile_options(${OS_COMPILE_OPTIONS_TARGET} PRIVATE ${OS_COMPILE_OPTIONS_ALL})
endfunction()

function(target_standard_compile_flags TARGET)
    target_os_compile_options(
        TARGET ${TARGET}
        LINUX -Wall -Werror -Wpedantic -Wno-unknown-pragmas
        WINDOWS /W4 /WX /wd4068
    )

    if (CMAKE_BUILD_TYPE STREQUAL Debug)
        if (UNIX)
            target_compile_options(${TARGET} PRIVATE --coverage)
            target_link_options(${TARGET} PRIVATE --coverage)
            target_link_libraries(${TARGET} PUBLIC gcov)
        endif()
    endif()

    target_compile_features(${TARGET} PUBLIC cxx_std_20)
endfunction()

if (UNIX)
    add_custom_target(ids
        COMMAND xmlprotoc/xmlprotoc.py --prepend xmlprotoc/id_head.h.pc --append xmlprotoc/id_tail.h.pc --out xmlprotoc/id.h protocol.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )

    add_custom_target(clean_gcda ALL COMMAND find -name "*.gcda" -delete COMMENT "Cleaning GCDA files")
    add_custom_target(coverage COMMAND gcovr -r ../src ./ -e ".*gtest.*" -e ".*/tests/.*" --html-details -o coverage.html)
endif()

add_subdirectory(src)