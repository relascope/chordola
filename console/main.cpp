#include <iostream>

#include "VERSION.h"
#include "audiobackend.h"
#include "chord.h"

void ChordArrived(Chord newChord) {
  static Chord oldChord{0, 0, 0};
  if (oldChord != newChord) {
    oldChord = newChord;
    std::cout << newChord;
  }
}

int main(int argc, char *argv[]) {
  std::cout << "chord-rec console: Realtime Chord Recognition\nVersion: "
            << CHORD_REC_CONSOLE_VERSION << '\n';

  registerChordListener(&ChordArrived);
  connectAudioBackend(argv[0]);

  return 0;
}
