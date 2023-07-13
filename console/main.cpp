#include <iostream>

#include "VERSION.h"
#include "audiobackend.h"

int main(int argc, char *argv[]) {
  std::cout << "chord-rec console: Realtime Chord Recognition\nVersion: "
            << CHORD_REC_CONSOLE_VERSION << '\n';

  connectAudioBackend();

  return 0;
}
