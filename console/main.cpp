#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#include "../lib/jack-backend/audiobackend.h"
#include "../lib/jack-backend/chord.h"
#include "../lib/jack-backend/chord_listener.h"
#include "VERSION.h"

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

  chord_recogniser::registerChordListener(&ChordArrived);

  std::string clientName(argv[0]);
  jack_backend::connectAudioBackend(clientName);

  consume.join();

  return 0;
}
