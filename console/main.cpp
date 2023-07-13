#include <iostream>

#include "audiobackend.h"
#include "VERSION.h"

int main(int argc, char *argv[])
{
    std::cout << "chord-rec console: Realtime Chord Recognition\nVersion: " << CHORD_REC_CONSOLE_VERSION << '\n';

    connectAudioBackend();

//    startChordRecognition();


    return 0;
}
