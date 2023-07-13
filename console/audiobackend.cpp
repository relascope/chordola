#ifndef AUDIOBACKEND_CPP
#define AUDIOBACKEND_CPP

#include "audiobackend.h"

#include "../Chord-Detector-and-Chromagram/src/ChordDetector.h"
#include "../Chord-Detector-and-Chromagram/src/Chromagram.h"

#include <jack/jack.h>

#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int frameSize = 512;
int sampleRate = 44100;

Chromagram c(frameSize, sampleRate);

std::string notes[] = {"c",   "cis", "d",   "dis", "e",   "f",
                       "fis", "g",   "gis", "a",   "ais", "h"};

using namespace std;

void printChroma(std::vector<double> c) {
  cout << "______" << std::endl;
  for (int i = 0; i < 12; i++)
    cout << c[i] << std::endl;
}

jack_port_t *input_port;
jack_port_t *output_port;

int lastRoot = -1;

string chordQualityNames[] = {"Minor",    "Major",       "Suspended",
                              "Dominant", "Dimished5th", "Augmented5th"};

void jack_shutdown(void *arg) { fprintf(stderr, "jack shutting down"); }

typedef struct Chord{
    int rootNote;
    int quality;
    int intervals;

    bool operator<(const Chord& other) const {
        if (rootNote < other.rootNote) return true;
        if (quality > other.quality) return false;
        return intervals < other.intervals;
    }

    bool operator==(const Chord& other) const {
        return rootNote == other.rootNote && quality == other.quality && intervals == other.intervals;
    }
} Chord;

int process(jack_nframes_t nframes, void *arg) {
  jack_default_audio_sample_t *out =
     (jack_default_audio_sample_t *)jack_port_get_buffer(output_port, nframes);
  jack_default_audio_sample_t *in =
      (jack_default_audio_sample_t *)jack_port_get_buffer(input_port, nframes);

  double *frames = new double[nframes];
  for (int i = 0; i < nframes; i++) {
    frames[i] = in[i];
  }

  c.processAudioFrame(frames);

  if (c.isReady()) {
    std::vector<double> chroma = c.getChromagram();

      printChroma(chroma);

    ChordDetector chordDetector;

    chordDetector.detectChord (chroma);

    static Chord oldChord;
    Chord newChord;
    newChord.rootNote = chordDetector.rootNote;
    newChord.quality= chordDetector.quality;
    newChord.intervals = chordDetector.intervals;

//    if (newChord == oldChord)
//        return 0;

    oldChord = newChord;
    std::cout << notes[chordDetector.rootNote] << " "
              << chordQualityNames[chordDetector.quality] << " "
              << chordDetector.intervals << std::endl;
  }

  return 0;
}
void connectAudioBackend() {

  jack_client_t *client;
  const char **ports;

  if ((client = jack_client_new("Chord Recognizer DOJOY")) == 0) {
    fprintf(stderr, "jack not running?\n");
    throw AudioBackendException("Could not register a new Jack Client");
  }

  jack_set_process_callback(client, process, 0);

  jack_on_shutdown(client, jack_shutdown, 0);
  printf("engine sample rate: %" PRIu32 "\n", jack_get_sample_rate(client));

  input_port = jack_port_register(client, "input", JACK_DEFAULT_AUDIO_TYPE,
                                  JackPortIsInput, 0);
  output_port = jack_port_register(client, "output", JACK_DEFAULT_AUDIO_TYPE,
                                   JackPortIsOutput, 0);

  if (jack_activate(client)) {
    fprintf(stderr, "cannot activate client");
    throw AudioBackendException("Jack Client cannot be activated");
  }

  c.setSamplingFrequency(jack_get_sample_rate(client));
  c.setInputAudioFrameSize(jack_get_buffer_size(client));

  // delete sndFile;

  sleep(1000);
  jack_client_close(client);
  exit(0);
}


#endif // AUDIOBACKEND_CPP
