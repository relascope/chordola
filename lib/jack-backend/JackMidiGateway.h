//
// Created by guru on 09.09.23.
//

#ifndef CHORD_RECOGNISER_CONSOLE_LIB_JACK_BACKEND_JACKMIDIGATEWAY_H_
#define CHORD_RECOGNISER_CONSOLE_LIB_JACK_BACKEND_JACKMIDIGATEWAY_H_

#include <jack/jack.h>

namespace jack_backend {

class JackMidiGateway {
 private:
  jack_port_t *midi_output_port;
};

} // jack_backend

#endif //CHORD_RECOGNISER_CONSOLE_LIB_JACK_BACKEND_JACKMIDIGATEWAY_H_
