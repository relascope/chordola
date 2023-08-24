#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#include "VERSION.h"
#include "audiobackend.h"
#include "chord.h"

static std::atomic<Chord> actChord;

void consumeThread() {
  while (true) {
    static Chord oldChord{0, 0, 0};
    if (oldChord != actChord) {
      oldChord = actChord;
      std::cout << actChord;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }
}

void ChordArrived(Chord newChord) { actChord = newChord; }

int main(int argc, char *argv[]) {
  std::cout << "chord-rec console: Realtime Chord Recognition\nVersion: "
            << CHORD_REC_CONSOLE_VERSION << '\n';

  std::thread consume(consumeThread);

  registerChordListener(&ChordArrived);

  std::string clientName(argv[0]);
  connectAudioBackend(clientName);

  return 0;
}
