cmake_minimum_required(VERSION 3.21)
project(SCAPMS VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(SCAPMS
    src/main.cpp
    src/mainwindow.cpp
    src/clinicmanager.cpp
    src/patientrecord.cpp
    src/appointment.cpp
    include/scapms/mainwindow.h
    include/scapms/clinicmanager.h
    include/scapms/patientrecord.h
    include/scapms/appointment.h
)

target_include_directories(SCAPMS PRIVATE include)
target_link_libraries(SCAPMS PRIVATE Qt6::Widgets)

if(WIN32)
    set_target_properties(SCAPMS PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
