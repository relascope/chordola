cmake_minimum_required(VERSION 3.26)
project(ui_qt)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(ui_qt main.cpp)
target_link_libraries(ui_qt
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)

