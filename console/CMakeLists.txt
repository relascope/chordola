cmake_minimum_required(VERSION 3.5)

project(chord-recogniser-console LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chord-recogniser-console main.cpp ../lib/jack-backend/audiobackend.cpp ../lib/jack-backend/audiobackend.h ../lib/jack-backend/chord.h ../lib/Chord-Detector-and-Chromagram/src/Chromagram.cpp ../lib/Chord-Detector-and-Chromagram/src/Chromagram.h ../lib/Chord-Detector-and-Chromagram/src/ChordDetector.h ../lib/Chord-Detector-and-Chromagram/src/ChordDetector.cpp
        ../lib/jack-backend/chord.cpp
        ../lib/jack-backend/JackMidiGateway.cpp
        ../lib/jack-backend/JackMidiGateway.h
        ../lib/jack-backend/chord_listener.cpp
        ../lib/jack-backend/chord_listener.h)

target_link_libraries(chord-recogniser-console -ljack)
target_link_libraries(chord-recogniser-console -lfftw3)
target_link_libraries(chord-recogniser-console -latomic)

add_compile_definitions(USE_FFTW)
