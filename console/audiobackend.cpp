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

void jack_shutdown(void *arg) {
  std::cerr << "jack shutting down, so we do too! DOJOY something else!\n";
}

int jack_process_callback(jack_nframes_t nframes, void *arg) {
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
void connectAudioBackend() {

  jack_client_t *client;

  if ((client = jack_client_new("Chord Recognizer DOJOY")) == 0) {
    std::cerr << "jack not running?\n";
    throw AudioBackendException("Could not register a new Jack Client");
  }

  jack_set_process_callback(client, jack_process_callback, 0);
  jack_on_shutdown(client, jack_shutdown, 0);

  std::cout << "engine sample rate: %" << PRIu32 << "\n"
            << jack_get_sample_rate(client);

  input_port = jack_port_register(client, "input", JACK_DEFAULT_AUDIO_TYPE,
                                  JackPortIsInput, 0);
  if (jack_activate(client)) {
    std::cerr << "cannot activate client\n";
    throw AudioBackendException("Jack Client cannot be activated");
  }

  chromagram.setSamplingFrequency(jack_get_sample_rate(client));
  chromagram.setInputAudioFrameSize(jack_get_buffer_size(client));

  sleep(1000);
  jack_client_close(client);
  exit(0);
}

#endif // AUDIOBACKEND_CPP
