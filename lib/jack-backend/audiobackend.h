#ifndef AUDIOBACKEND_H
#define AUDIOBACKEND_H

#include "chord.h"

typedef void (*ChordListener)(Chord);

void connectAudioBackend(const std::string &clientName);

void registerChordListener(ChordListener);

#endif // AUDIOBACKEND_H
