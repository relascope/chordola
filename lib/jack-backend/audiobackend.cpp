#ifndef AUDIOBACKEND_CPP
#define AUDIOBACKEND_CPP

#include "audiobackend.h"
#include "chord.h"

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
#include <algorithm>

namespace jack_backend {

jack_client_t* jackClient;
jack_port_t* audio_input_port;
jack_port_t* midi_chords_output_port_;
jack_port_t* bass_note_output_port_;

int frameSize = 512;
int sampleRate = 44100;

Chromagram chromagram(frameSize, sampleRate);

void printChroma(std::vector<double> c)
{
    std::cout << "______" << std::endl;
    for (int i = 0; i<12; i++)
        std::cout << c[i] << std::endl;
}
template<typename T>
void shrink_vector(std::vector<T>& vector, size_t max_size)
{
    if (vector.size()>max_size) {
        vector.erase(vector.begin()+max_size, vector.end());
    }
}

std::vector<unsigned char> get_most_significant_notes(const std::vector<double>& chromagram,
        chord_recogniser::Chord chord)
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

    // the most significant is most likely the melody (voice), which we do not want to use
    // TODOJOY we may try to remove the first note for Chord-Detection too
    notes_ordered.erase(notes_ordered.begin());

    shrink_vector(notes_ordered, 2);

    return notes_ordered;
}

void send_midi_note(void* midi_out_buffer, unsigned int midi_number_in_octave, bool note_on = true);
void send_all_notes_off(void* midi_out_buffer);
void write_midi_chord(const std::vector<double>& chromagram_vector, chord_recogniser::Chord chord, jack_nframes_t
number_of_frames
)
{
    std::vector<unsigned char> new_notes_ordered = get_most_significant_notes(chromagram_vector, chord);

    static std::vector<unsigned char> old_notes_ordered;

    void* midi_out_buffer = jack_port_get_buffer(midi_chords_output_port_, number_of_frames);
    jack_midi_clear_buffer(midi_out_buffer);

    constexpr unsigned char midi_note_c4 = 60;

    static unsigned char old_bass_note = 0;
    unsigned char new_bass_note = 48+chord.rootNote;

    send_all_notes_off(midi_out_buffer);

    send_midi_note(midi_out_buffer, old_bass_note, false);

    for (unsigned int note_number_in_octave : old_notes_ordered) {
        send_midi_note(midi_out_buffer, midi_note_c4+note_number_in_octave, false);
    }

    for (unsigned int note_number_in_octave : new_notes_ordered) {
        send_midi_note(midi_out_buffer, midi_note_c4+note_number_in_octave);
    }

    send_midi_note(midi_out_buffer, new_bass_note);

    void* bass_out_buffer = jack_port_get_buffer(bass_note_output_port_, number_of_frames);
    jack_midi_clear_buffer(bass_out_buffer);
    send_all_notes_off(bass_out_buffer);
    send_midi_note(bass_out_buffer, old_bass_note, false);
    send_midi_note(bass_out_buffer, new_bass_note);

    old_bass_note = new_bass_note;

    old_notes_ordered = new_notes_ordered;
}

void send_all_notes_off(void* midi_out_buffer)
{
    // @see https://forum.arduino.cc/t/midi-h-library-help-all-notes-off/193357/7
    for (unsigned int i = 0; i<17; i++) {
        unsigned char* buffer;
        buffer = jack_midi_event_reserve(midi_out_buffer, 0, 3);

        buffer[0] = 123;
        buffer[1] = 0;
        buffer[2] = i;
    }
}

void send_midi_note(void* midi_out_buffer, unsigned int midi_number_in_octave, bool note_on)
{
    unsigned char* buffer;
    buffer = jack_midi_event_reserve(midi_out_buffer, 0, 3);
    buffer[0] = note_on ? 0x90 : 0x80; // note on (note off is 0x80)
    buffer[1] = midi_number_in_octave;
    buffer[2] = 108;// velocity
}

void processFrames(double* frames, jack_nframes_t number_of_frames)
{
    chromagram.processAudioFrame(frames);

    if (chromagram.isReady()) {
        std::vector<double> chroma = chromagram.getChromagram();

        static ChordDetector chord_detector;
        chord_detector.detectChord(chroma);

        chord_recogniser::Chord chord{chord_detector.rootNote, chord_detector.quality,
                                      chord_detector.intervals};
        chord_recogniser::notify_listener(chord);

        write_midi_chord(chroma, chord, number_of_frames);
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

    midi_chords_output_port_ =
            jack_port_register(jackClient, "midi-chords-out", JACK_DEFAULT_MIDI_TYPE,
                    JackPortIsOutput, 0);

    bass_note_output_port_ =
            jack_port_register(jackClient, "bass-chord-out", JACK_DEFAULT_MIDI_TYPE,
                    JackPortIsOutput, 0);

    if (jack_activate(jackClient)) {
        std::cerr << "cannot activate client\n";
        throw AudioBackendException("Jack Client cannot be activated");
    }

    chromagram.setSamplingFrequency(jack_get_sample_rate(jackClient));
    chromagram.setInputAudioFrameSize(jack_get_buffer_size(jackClient));
    chromagram.setChromaCalculationInterval(6);

    while (true) {
        sleep(10000);
    }

    jack_client_close(jackClient);
    exit(0);
}
}
#endif // AUDIOBACKEND_CPP
