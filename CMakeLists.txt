cmake_minimum_required(VERSION 3.24)

project(TimeIsMoney_CMake VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "/home/felix/Qt/6.5.0/gcc_64")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Sql Widgets PrintSupport SerialPort)
find_package(Qt6 REQUIRED COMPONENTS Core Sql Widgets PrintSupport SerialPort)

add_subdirectory(TimeIsMoneyDB_CMake)
add_subdirectory(TimeIsMoneyInvoice_CMake)
add_subdirectory(TimeIsMoneySerial_CMake)
add_subdirectory(TimeIsMoneyTask_CMake)

add_executable(TimeIsMoney_CMake
        main.cpp
        mainwindow.cpp
        config.cpp
        updater.cpp
)

target_link_libraries(TimeIsMoney_CMake
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::SerialPort
    TimeIsMoneyDB_CMake
    TimeIsMoneyInvoice_CMake
    TimeIsMoneySerial_CMake
    TimeIsMoneyTask_CMake
)

include(GNUInstallDirs)
install(TARGETS TimeIsMoney_CMake
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
