cmake_minimum_required(VERSION 3.5)

project(chord-recogniser-console LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chord-recogniser-console main.cpp audiobackend.cpp audiobackend.h chord.h ../lib/Chord-Detector-and-Chromagram/src/Chromagram.cpp ../lib/Chord-Detector-and-Chromagram/src/Chromagram.h ../lib/Chord-Detector-and-Chromagram/src/ChordDetector.h ../lib/Chord-Detector-and-Chromagram/src/ChordDetector.cpp
        chord.cpp)

target_link_libraries(chord-recogniser-console -ljack)
target_link_libraries(chord-recogniser-console -lfftw3)

add_compile_definitions(USE_FFTW)
