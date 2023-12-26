cmake_minimum_required(VERSION 3.5)

project(chordola LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(CMAKE_CXX_FLAGS -Wall)
set(CMAKE_CXX_FLAGS -Wextra)

add_executable(chordola console/main.cpp lib/jack-backend/audiobackend.cpp lib/jack-backend/audiobackend.h lib/jack-backend/chord.h
        lib/Chord-Detector-and-Chromagram/src/Chromagram.cpp
        lib/Chord-Detector-and-Chromagram/src/Chromagram.h
        lib/Chord-Detector-and-Chromagram/src/ChordDetector.h
        lib/Chord-Detector-and-Chromagram/src/ChordDetector.cpp
        lib/jack-backend/chord.cpp
        lib/jack-backend/chord_listener.cpp
        lib/jack-backend/chord_listener.h)

target_link_libraries(chordola -ljack)
target_link_libraries(chordola -lfftw3)
target_link_libraries(chordola -latomic)

find_package (Threads REQUIRED)

target_link_libraries(chordola
    ${CMAKE_THREAD_LIBS_INIT}
)

add_compile_definitions(USE_FFTW)
