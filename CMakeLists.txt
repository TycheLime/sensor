cmake_minimum_required(VERSION 3.16)

project(sensor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加SerialPort到find_package
find_package(Qt6 REQUIRED COMPONENTS Quick SerialPort)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appsensor
    main.cpp
)

qt_add_qml_module(appsensor
    URI sensor
    VERSION 1.0
    QML_FILES rec.qml
    SOURCES
    SOURCES depth.h depth.cpp
    SOURCES compass.h compass.cpp
    SOURCES manager.h manager.cpp
    QML_FILES
    QML_FILES
    QML_FILES
    QML_FILES
    SOURCES device.h
    SOURCES device.cpp
    RESOURCES config.ini
    SOURCES posture.h posture.cpp
    QML_FILES
    QML_FILES sensor.qml
    QML_FILES
    QML_FILES
    SOURCES
    SOURCES
    SOURCES sensorfactory.h sensorfactory.cpp
    RESOURCES sleep.svg
    QML_FILES back.qml
    QML_FILES SensorInfo.qml
    SOURCES sensorreleasetask.h sensorreleasetask.cpp
    QML_FILES Main.qml
    SOURCES
)

set_target_properties(appsensor PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 链接Qt6::SerialPort库
target_link_libraries(appsensor
    PRIVATE Qt6::Quick Qt6::SerialPort
)

include(GNUInstallDirs)
install(TARGETS appsensor
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
