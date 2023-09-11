//
// Created by guru on 11.09.23.
//

#include "chord_listener.h"
#include <vector>
namespace chord_recogniser {
std::vector<ChordListener> chord_listeners;

void registerChordListener(const ChordListener &chord_listener) {
  chord_listeners.push_back(chord_listener);
}

void notifyListener(Chord chord) {
  for (const auto &listener : chord_listeners) {
    listener(chord);
  }
}

}
