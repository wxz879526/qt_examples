cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Qt_examples VERSION 0.0.1 LANGUAGES CXX)

option(USE_WERROR "Error on compile warning" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_DIR "D:/DVP/qt65/6.5.0/msvc2019_64" CACHE PATH "QT6 Path")
set(CMAKE_PREFIX_PATH ${QT_DIR})
find_package(QT NAMES Qt6 Qt5 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(mediaplayer)
add_subdirectory(net-chat)