#include "chord.h"

std::string noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

std::string chordQualityNames[] = {"Minor", "Major", "Suspended", "Dominant", "Dimished5th", "Augmented5th"};

bool Chord::operator==(const Chord &other) const {
    return rootNote == other.rootNote && quality == other.quality && intervals == other.intervals;
}

std::ostream &operator<<(std::ostream &os, const Chord &chord) {
    os << noteNames[chord.rootNote];

    if (chord.quality == 2)
        os << "sus";
    else if (chord.quality == 4)
        os << "°";
    else if (chord.quality == 5)
        os << "+";

    if (chord.intervals != 0)
        os << chord.intervals;

    os << '\n';

    return os;
}
