#ifndef CHORD_RECOGNISER_CONSOLE_AUDIOBACKENDEXCEPTION_H

#include <string>
#include <stdexcept>

namespace jack_backend {

class AudioBackendException final : public std::runtime_error {
 public:
  AudioBackendException(const std::string &msg) : runtime_error{msg} {}
};

}
#endif //CHORD_RECOGNISER_CONSOLE_AUDIOBACKENDEXCEPTION_H
