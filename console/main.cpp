#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#include "../lib/jack-backend/audiobackend.h"
#include "../lib/jack-backend/chord.h"
#include "../lib/jack-backend/chord_listener.h"
#include "VERSION.h"

static std::atomic<chord_recogniser::Chord> act_chord;

void consumeThread()
{
    while (true) {
        static chord_recogniser::Chord old_chord{0, 0, 0};
        if (old_chord!=act_chord) {
            old_chord = act_chord;
            std::cout << act_chord;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

void ChordArrived(chord_recogniser::Chord new_chord) { act_chord = new_chord; }

int main(int argc, char* argv[])
{
    std::cout << "chord-rec console: Realtime Chord Recognition\nVersion: "
              << CHORD_REC_CONSOLE_VERSION << '\n';

    std::thread consume(consumeThread);

    chord_recogniser::register_chord_listener(&ChordArrived);

    std::string client_name(argv[0]);
    jack_backend::connect_audio_backend(client_name);

    consume.join();

    return 0;
}
