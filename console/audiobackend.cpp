#ifndef AUDIOBACKEND_CPP
#define AUDIOBACKEND_CPP

#include "audiobackend.h"
#include "chord.h"

#include "../lib/Chord-Detector-and-Chromagram/src/ChordDetector.h"
#include "../lib/Chord-Detector-and-Chromagram/src/Chromagram.h"

#include <jack/jack.h>

#include <errno.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <thread>
#include <unistd.h>

int frameSize = 512;
int sampleRate = 44100;

Chromagram chromagram(frameSize, sampleRate);

std::string notes[] = {"c",   "cis", "d",   "dis", "e",   "f",
                       "fis", "g",   "gis", "a",   "ais", "h"};

std::string chordQualityNames[] = {"Minor",    "Major",       "Suspended",
                                   "Dominant", "Dimished5th", "Augmented5th"};

void printChroma(std::vector<double> c) {
  std::cout << "______" << std::endl;
  for (int i = 0; i < 12; i++)
    std::cout << c[i] << std::endl;
}

jack_port_t *input_port;

void jack_shutdown(void *) {
  std::cerr << "jack shutting down, so we do too! DOJOY something else!\n";
}

int jack_process_callback(jack_nframes_t nframes, void *) {
  jack_default_audio_sample_t *inputFrames =
      (jack_default_audio_sample_t *)jack_port_get_buffer(input_port, nframes);

  double *frames = new double[nframes];
  for (unsigned int i = 0; i < nframes; ++i) {
    frames[i] = inputFrames[i];
  }

  chromagram.processAudioFrame(frames);

  if (chromagram.isReady()) {
    std::vector<double> chroma = chromagram.getChromagram();
    static ChordDetector chordDetector;
    chordDetector.detectChord(chroma);

    // TODOJOY decide if we want to print everything or only if a new chord is
    // detected - configurable?
    std::cout << notes[chordDetector.rootNote] << " "
              << chordQualityNames[chordDetector.quality] << " "
              << chordDetector.intervals << std::endl;
  }

  return 0;
}


jack_client_t *jackClient;




void connect_ports(const char* src, const char* destination) {
    const char* srcType = jack_port_type(jack_port_by_name(jackClient, src));
    const char* destType = jack_port_type(jack_port_by_name(jackClient, destination));

    if (jack_port_connected_to(jack_port_by_name(jackClient, src), destination)) {
        return;
    }

    if (std::string(srcType)   != std::string(destType))
        return;

    if (jack_connect(jackClient, src, destination) != 0) {
        std::stringstream ss;
        ss << "Port connection failed. " << src << " to " << destination << " \n";
        throw AudioBackendException(ss.str());
    }
}

void connectSystemCapture() {
    const char **outports;
    if ((outports = jack_get_ports(jackClient, "system", NULL, JackPortIsOutput)) == NULL) {
        throw AudioBackendException("Error getting system capture ports");
    }

    const char* myPort = jack_port_name(input_port);
    size_t i = 0;
    while (outports != nullptr  && outports[i] != nullptr) {
        connect_ports(outports[i], myPort);
        ++i;
    }

    jack_free(outports);
}

void connectPortsConnectingSystemPlayback() {
    const char **systemPlaybackPorts;
    if ((systemPlaybackPorts = jack_get_ports(jackClient, "system", nullptr, JackPortIsInput)) == nullptr) {
        throw AudioBackendException("Error getting system playback ports");
    }

    const char* myPort = jack_port_name(input_port);
    size_t i = 0;
    while (systemPlaybackPorts != nullptr && systemPlaybackPorts[i] != nullptr) {
        const char** portsConnectedToPlayback = jack_port_get_all_connections(jackClient, jack_port_by_name(jackClient, systemPlaybackPorts[i]));

        size_t j = 0;
        while (portsConnectedToPlayback != nullptr && portsConnectedToPlayback[j] != nullptr) {
            connect_ports(portsConnectedToPlayback[j], myPort);
            ++j;
        }
        jack_free(portsConnectedToPlayback);
        ++i;
    }

    jack_free(systemPlaybackPorts);
}

void autoConnectPorts() {
    while (true) {
    connectSystemCapture();
    connectPortsConnectingSystemPlayback();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void connectAudioBackend() {
  if ((jackClient = jack_client_new("Chord Recognizer DOJOY")) == 0) {
    std::cerr << "jack not running?\n";
    throw AudioBackendException("Could not register a new Jack Client");
  }

  jack_set_process_callback(jackClient, jack_process_callback, 0);
  jack_on_shutdown(jackClient, jack_shutdown, 0);

  std::cout << "engine sample rate: %" << PRIu32 << "\n"
            << jack_get_sample_rate(jackClient);

  input_port = jack_port_register(jackClient, "input", JACK_DEFAULT_AUDIO_TYPE,
                                  JackPortIsInput, 0);

  if (jack_activate(jackClient)) {
    std::cerr << "cannot activate client\n";
    throw AudioBackendException("Jack Client cannot be activated");
  }

  std::thread autoConnect(autoConnectPorts);

  chromagram.setSamplingFrequency(jack_get_sample_rate(jackClient));
  chromagram.setInputAudioFrameSize(jack_get_buffer_size(jackClient));

  sleep(1000);
  jack_client_close(jackClient);
  exit(0);
}

#endif // AUDIOBACKEND_CPP
