#ifndef AUDIOBACKEND_H
#define AUDIOBACKEND_H

#include <string>

namespace jack_backend {

void connect_audio_backend(const std::string& client_name);

}
#endif // AUDIOBACKEND_H
