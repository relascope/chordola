//
// Created by guru on 11.09.23.
//

#ifndef CHORD_RECOGNISER_CONSOLE_LIB_JACK_BACKEND_CHORD_LISTENER_H_
#define CHORD_RECOGNISER_CONSOLE_LIB_JACK_BACKEND_CHORD_LISTENER_H_

#include "chord.h"

namespace chord_recogniser {
typedef void (*ChordListener)(Chord);
void registerChordListener(const ChordListener &);
void notifyListener(Chord chord);
}

#endif //CHORD_RECOGNISER_CONSOLE_LIB_JACK_BACKEND_CHORD_LISTENER_H_
