#ifndef AUDIOBACKEND_H
#define AUDIOBACKEND_H

#include "chord.h"

namespace jack_backend {

void connectAudioBackend(const std::string &clientName);

}
#endif // AUDIOBACKEND_H
