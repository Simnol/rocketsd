cmake_minimum_required(VERSION 3.17.0)
project(rocketsd)

cmake_policy(SET CMP0100 NEW)

include("${CMAKE_BINARY_DIR}/conan_paths.cmake")
include("${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
conan_basic_setup(TARGETS)


include(conan_qt_executables_variables)
include(Qt5CoreMacros)
include(Qt5WidgetsMacros)

find_package(GTest)
enable_testing()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (UNIX)
    target_compile_options(CONAN_PKG::qt PUBLIC -fPIC)
endif()

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
    target_compile_features(${TARGET} PUBLIC cxx_std_20)
endfunction()

add_subdirectory(src)

if (UNIX)
    add_custom_target(ids
        COMMAND xmlprotoc/xmlprotoc.py --prepend xmlprotoc/id_head.h.pc --append xmlprotoc/id_tail.h.pc --out xmlprotoc/id.h protocol.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    )

    add_custom_target(clean_gcda ALL COMMAND find -name "*.gcda" -delete COMMENT "Cleaning GCDA files")
    add_custom_target(coverage COMMAND gcovr -r ../src ./ -e ".*gtest.*" -e ".*/tests/.*" --html-details -o coverage.html)
endif()