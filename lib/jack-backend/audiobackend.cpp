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

std::vector<unsigned int> get_most_significant_notes(const std::vector<double>& chromagram)
{
    // Create a vector of pairs with (value, index)
    std::vector<std::pair<int, int>> value_index_pairs;
    for (int i = 0; i<chromagram.size(); ++i)
        value_index_pairs.emplace_back(chromagram[i], i);


    // Sort the vector of pairs in descending order by value
    std::sort(value_index_pairs.rbegin(), value_index_pairs.rend());

    std::vector<unsigned int> notes_ordered;
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

void write_midi_chord(const std::vector<double>& chromagram, jack_nframes_t nframes)
{
    constexpr const size_t kNumberOfNotesForChord = 3;
    // TODOJOY check the distance of the notes, maybe set the notes down an octave???
    // save the old notes and place note offs.
    std::vector<unsigned int> new_notes_ordered = get_most_significant_notes(chromagram);
    shrink_vector(new_notes_ordered, kNumberOfNotesForChord);

    static std::vector<unsigned int> old_notes_ordered;

    void* midi_out_buffer = jack_port_get_buffer(midi_output_port_, nframes);
    jack_midi_clear_buffer(midi_out_buffer);

    for (unsigned int i : old_notes_ordered) {
        unsigned char* buffer;
        buffer = jack_midi_event_reserve(midi_out_buffer, 0, 3);
        buffer[2] = 108;// velocity
        buffer[1] = 60+i; // note starting with C4
        buffer[0] = 0x80; // note off
    }

    for (unsigned int i : new_notes_ordered) {
        unsigned char* buffer;
        buffer = jack_midi_event_reserve(midi_out_buffer, 0, 3);
        buffer[2] = 108;// velocity
        buffer[1] = 60+i; // note starting with C4
        buffer[0] = 0x90; // note on (note off is 0x80)
    }

    old_notes_ordered = new_notes_ordered;
}

void processFrames(double* frames, jack_nframes_t nframes)
{
    chromagram.processAudioFrame(frames);

    if (chromagram.isReady()) {
        std::vector<double> chroma = chromagram.getChromagram();

        write_midi_chord(chroma, nframes);

        static ChordDetector chordDetector;
        chordDetector.detectChord(chroma);

        Chord chord{chordDetector.rootNote, chordDetector.quality,
                    chordDetector.intervals};
        chord_recogniser::notifyListener(chord);
    }
}

void jack_shutdown(void*)
{
    std::cerr << "jack shutting down, so we do too! DOJOY something else!\n";
}

int jack_process_callback(jack_nframes_t nframes, void*)
{
    auto inputFrames =
            (jack_default_audio_sample_t*) jack_port_get_buffer(audio_input_port, nframes);

    double* frames = new double[nframes];
    for (unsigned int i = 0; i<nframes; ++i) {
        frames[i] = inputFrames[i];
    }

    processFrames(frames, nframes);

    delete[] frames;

    return 0;
}

void connect_ports(const char* src, const char* destination)
{
    const char* srcType = jack_port_type(jack_port_by_name(jackClient, src));
    const char* destType =
            jack_port_type(jack_port_by_name(jackClient, destination));

    if (jack_port_connected_to(jack_port_by_name(jackClient, src), destination)) {
        return;
    }

    if (std::string(srcType)!=std::string(destType))
        return;

    if (jack_connect(jackClient, src, destination)!=0) {
        std::stringstream ss;
        ss << "Port connection failed. " << src << " to " << destination << " \n";
        throw AudioBackendException(ss.str());
    }
}

void connectSystemCapture()
{
    const char** outports;
    if ((outports = jack_get_ports(jackClient, "system", NULL,
            JackPortIsOutput))==NULL) {
        throw AudioBackendException("Error getting system capture ports");
    }

    const char* myPort = jack_port_name(audio_input_port);
    size_t i = 0;
    while (outports!=nullptr && outports[i]!=nullptr) {
        connect_ports(outports[i], myPort);
        ++i;
    }

    jack_free(outports);
}

void connectPortsConnectingSystemPlayback()
{
    const char** systemPlaybackPorts;
    if ((systemPlaybackPorts = jack_get_ports(jackClient, "system", nullptr,
            JackPortIsInput))==nullptr) {
        throw AudioBackendException("Error getting system playback ports");
    }

    const char* myPort = jack_port_name(audio_input_port);
    size_t i = 0;
    while (systemPlaybackPorts!=nullptr && systemPlaybackPorts[i]!=nullptr) {
        const char** portsConnectedToPlayback = jack_port_get_all_connections(
                jackClient, jack_port_by_name(jackClient, systemPlaybackPorts[i]));

        size_t j = 0;
        while (portsConnectedToPlayback!=nullptr &&
                portsConnectedToPlayback[j]!=nullptr) {
            connect_ports(portsConnectedToPlayback[j], myPort);
            ++j;
        }
        jack_free(portsConnectedToPlayback);
        ++i;
    }

    jack_free(systemPlaybackPorts);
}

void autoConnectPorts()
{
    while (true) {
        connectSystemCapture();
        connectPortsConnectingSystemPlayback();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void connectAudioBackend(const std::string& clientName)
{

    std::unique_ptr<jack_status_t> status;

    jackClient = jack_client_open(clientName.c_str(),
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
