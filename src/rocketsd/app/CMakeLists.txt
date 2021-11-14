cmake_minimum_required(VERSION 3.17)

set(SOURCES
    app.cc
    app.hh
    main.cc
)

add_executable(rocketsd ${SOURCES} ${HEADERS})
target_standard_compile_flags(rocketsd)
target_link_libraries(rocketsd PUBLIC rocketsd.module cute.proto CONAN_PKG::qt)

install(TARGETS rocketsd DESTINATION bin)

if(UNIX)
    install(
        FILES
            ${CONAN_QT_ROOT}/lib/libQt5Network.so.5    ${CONAN_QT_ROOT}/lib/libQt5Network.so.5.15.2
            ${CONAN_QT_ROOT}/lib/libQt5Xml.so.5        ${CONAN_QT_ROOT}/lib/libQt5Xml.so.5.15.2
            ${CONAN_QT_ROOT}/lib/libQt5SerialPort.so.5 ${CONAN_QT_ROOT}/lib/libQt5SerialPort.so.5.15.2
            ${CONAN_QT_ROOT}/lib/libQt5Core.so.5       ${CONAN_QT_ROOT}/lib/libQt5Core.so.5.15.2
        DESTINATION lib
    )
elseif(MSVC)
    install(
        FILES
            ${CONAN_QT_ROOT}/bin/Qt5Network.dll
            ${CONAN_QT_ROOT}/bin/Qt5Xml.dll
            ${CONAN_QT_ROOT}/bin/Qt5SerialPort.dll
            ${CONAN_QT_ROOT}/bin/Qt5Core.dll
        DESTINATION bin
    )
endif()