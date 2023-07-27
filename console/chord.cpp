
#include "chord.h"

std::string noteNames[] = {"c", "cis", "d", "dis", "e", "f",
                           "fis", "g", "gis", "a", "ais", "h"};

std::string chordQualityNames[] = {"Minor",    "Major",       "Suspended",
                                   "Dominant", "Dimished5th", "Augmented5th"};

bool Chord::operator==(const Chord &other) const {
    return rootNote == other.rootNote && quality == other.quality &&
           intervals == other.intervals;
}

std::ostream &operator<<(std::ostream &os, const Chord &chord) {
    return os << noteNames[chord.rootNote] << " " << chordQualityNames[chord.quality] << " " << chord.intervals << "\n";
}
