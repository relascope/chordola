#ifndef CHORD_H
#define CHORD_H

/* Chord to be compatible with the actual choosen Chord Recogniser */
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

  bool operator==(const Chord &other) const {
    return rootNote == other.rootNote && quality == other.quality &&
           intervals == other.intervals;
  }
} Chord;

#endif // CHORD_H
