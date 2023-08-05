#include <iostream>

#include "VERSION.h"
#include "audiobackend.h"

int main(int argc, char *argv[]) {
  std::cout << "chord-rec console: Realtime Chord Recognition\nVersion: "
            << CHORD_REC_CONSOLE_VERSION << '\n';

    registerChordListener([] (Chord c) { std::cout << c; });
    connectAudioBackend(argv[0]);

  return 0;
}
