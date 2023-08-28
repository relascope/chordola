cmake_minimum_required(VERSION 3.26)
project(chord-recogniser-qt-DoJoy)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(chord-recogniser-qt-DoJoy main.cpp ../lib/jack-backend/audiobackend.cpp ../lib/jack-backend/audiobackend.h ../lib/jack-backend/chord.h ../lib/Chord-Detector-and-Chromagram/src/Chromagram.cpp ../lib/Chord-Detector-and-Chromagram/src/Chromagram.h ../lib/Chord-Detector-and-Chromagram/src/ChordDetector.h ../lib/Chord-Detector-and-Chromagram/src/ChordDetector.cpp
        ../lib/jack-backend/chord.cpp)

target_link_libraries(chord-recogniser-qt-DoJoy -lfftw3)
target_link_libraries(chord-recogniser-qt-DoJoy -ljack)
target_link_libraries(chord-recogniser-qt-DoJoy -latomic)

target_link_libraries(chord-recogniser-qt-DoJoy
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)