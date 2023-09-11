#ifndef AUDIOBACKEND_CPP
#define AUDIOBACKEND_CPP

#include "audiobackend.h"

#include "../Chord-Detector-and-Chromagram/src/ChordDetector.h"
#include "../Chord-Detector-and-Chromagram/src/Chromagram.h"
#include "audiobackendexception.h"
#include "chord_listener.h"

#include <jack/jack.h>
#include <jack/midiport.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

namespace jack_backend {

jack_client_t* jackClient;
jack_port_t* audio_input_port;
jack_port_t* midi_output_port_;

int frameSize = 512;
int sampleRate = 44100;

Chromagram chromagram(frameSize, sampleRate);

void printChroma(std::vector<double> c)
{
    std::cout << "______" << std::endl;
    for (int i = 0; i<12; i++)
        std::cout << c[i] << std::endl;
}

std::vector<unsigned char> get_most_significant_notes(const std::vector<double>& chromagram)
{
    // Create a vector of pairs with (value, index)
    std::vector<std::pair<double, unsigned char>> value_index_pairs;
    for (size_t i = 0; i<chromagram.size(); ++i) {
        value_index_pairs.emplace_back(chromagram[i], static_cast<unsigned char>(i));
    }

    // Sort the vector of pairs in descending order by value
    std::sort(value_index_pairs.rbegin(), value_index_pairs.rend());

    std::vector<unsigned char> notes_ordered;
    for (const auto& pair : value_index_pairs) {
        notes_ordered.push_back(pair.second);
    }

    return notes_ordered;
}

template<typename T>
void shrink_vector(std::vector<T>& vector, size_t max_size)
{
    if (vector.size()>max_size) {
        vector.erase(vector.begin()+max_size, vector.end());
    }
}

void write_midi_chord(const std::vector<double>& chromagram_vector, jack_nframes_t number_of_frames)
{
    constexpr const size_t k_number_of_notes_for_chord = 3;

    std::vector<unsigned char> new_notes_ordered = get_most_significant_notes(chromagram_vector);
    shrink_vector(new_notes_ordered, k_number_of_notes_for_chord);

    static std::vector<unsigned char> old_notes_ordered;

    void* midi_out_buffer = jack_port_get_buffer(midi_output_port_, number_of_frames);
    jack_midi_clear_buffer(midi_out_buffer);

    for (unsigned int i : old_notes_ordered) {
        unsigned char* buffer;
        buffer = jack_midi_event_reserve(midi_out_buffer, 0, 3);
        buffer[2] = 108;// velocity
        buffer[1] = 60+i; // note starting with C4
        buffer[0] = 0x80U; // note off
    }

    for (unsigned int i : new_notes_ordered) {
        unsigned char* buffer;
        buffer = jack_midi_event_reserve(midi_out_buffer, 0, 3);
        buffer[2] = 108;// velocity
        buffer[1] = 60+i; // note starting with C4
        buffer[0] = 0x90U; // note on (note off is 0x80)
    }

    old_notes_ordered = new_notes_ordered;
}

void processFrames(double* frames, jack_nframes_t number_of_frames)
{
    chromagram.processAudioFrame(frames);

    if (chromagram.isReady()) {
        std::vector<double> chroma = chromagram.getChromagram();

        write_midi_chord(chroma, number_of_frames);

        static ChordDetector chord_detector;
        chord_detector.detectChord(chroma);

        Chord chord{chord_detector.rootNote, chord_detector.quality,
                    chord_detector.intervals};
        chord_recogniser::notify_listener(chord);
    }
}

void jack_shutdown(void*)
{
    std::cerr << "jack shutting down, so we do too! DoJoy something else!\n";
}

int jack_process_callback(jack_nframes_t number_of_frames, void*)
{
    auto input_frames =
            static_cast<jack_default_audio_sample_t*> ( jack_port_get_buffer(audio_input_port, number_of_frames));

    auto* frames = new double[number_of_frames];
    for (unsigned int i = 0; i<number_of_frames; ++i) {
        frames[i] = input_frames[i];
    }

    processFrames(frames, number_of_frames);

    delete[] frames;

    return 0;
}

void connect_audio_backend(const std::string& client_name)
{
    std::unique_ptr<jack_status_t> status;

    jackClient = jack_client_open(client_name.c_str(),
            JackOptions::JackNullOption, status.get());
    if (jackClient==nullptr) {
        std::cerr << "jack not running?\n";
        throw AudioBackendException("Could not register a new Jack Client");
    }

    (void) jack_set_process_callback(jackClient, jack_process_callback, 0);
    jack_on_shutdown(jackClient, jack_shutdown, 0);

    std::cout << "engine sample rate: %" << PRIu32 << "\n"
              << jack_get_sample_rate(jackClient);

    audio_input_port = jack_port_register(jackClient, "audio-input", JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsInput, 0);

    midi_output_port_ =
            jack_port_register(jackClient, "midi-chords-out", JACK_DEFAULT_MIDI_TYPE,
                    JackPortIsOutput, 0);

    if (jack_activate(jackClient)) {
        std::cerr << "cannot activate client\n";
        throw AudioBackendException("Jack Client cannot be activated");
    }

    chromagram.setSamplingFrequency(jack_get_sample_rate(jackClient));
    chromagram.setInputAudioFrameSize(jack_get_buffer_size(jackClient));

    while (true) {
        sleep(10000);
    }

    jack_client_close(jackClient);
    exit(0);
}
}
#endif // AUDIOBACKEND_CPP
