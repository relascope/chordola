#ifndef AUDIOBACKEND_H
#define AUDIOBACKEND_H

#include <stdexcept>
#include <string>

class AudioBackendException final : public std::runtime_error {
public:
    AudioBackendException(const std::string& msg) : std::runtime_error{msg} {}
};

void connectAudioBackend();

#endif // AUDIOBACKEND_H
