#ifndef CHORD_H
#define CHORD_H

#include <ostream>
#include <string>

/* Chord to be compatible with the actual chosen Chord Recogniser */
typedef struct Chord {
  int rootNote;
  int quality;
  int intervals;

  bool operator<(const Chord &other) const {
    if (rootNote < other.rootNote)
      return true;
    if (quality > other.quality)
      return false;
    return intervals < other.intervals;
  }

  bool operator==(const Chord &other) const;

  bool operator!=(const Chord &other) const { return !(*this == other); }

  friend std::ostream &operator<<(std::ostream &os, Chord const &chord);

} Chord;

#endif // CHORD_H
