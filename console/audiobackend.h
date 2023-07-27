#ifndef AUDIOBACKEND_H
#define AUDIOBACKEND_H

#include "chord.h"

typedef void (*ChordListener)(Chord);

void connectAudioBackend();

void registerChordListener(ChordListener);

#endif // AUDIOBACKEND_H
