cmake_minimum_required(VERSION 3.16)

project(Lab4QtTimerStopwatch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(Lab4QtTimerStopwatch
    src/main.cpp
    src/MainWindow.cpp
    include/MainWindow.h
)

target_include_directories(Lab4QtTimerStopwatch PRIVATE include)
target_link_libraries(Lab4QtTimerStopwatch PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# if(MSVC)
#     target_compile_options(Lab4QtTimerStopwatch PRIVATE /Zc:__cplusplus /permissive-)
# endif()
