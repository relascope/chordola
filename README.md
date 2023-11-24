# chordola

- Realtime musical chord recognition
- Chord notes output per MIDI
- Music can come from any client supporting JACK Audio Connection Kit


Build
=====
Get the Source
--------------
! This codebase uses git rsubmodule(s)

    git clone --recursive https://github.com/relascope/chord-rec.git

or

    git clone https://github.com/relascope/chord-rec.git
    git submodule update --init --recursive


Prerequisites
-------------
    apt install cmake make libjack-jackd2-dev libfftw3-dev -y

Build
-----

    cd console
    cmake .
    make

Usage
=====
Start the Jack Audio Engine (e.g. with QJackCtl)

    ./chord-recogniser

Connect a JACK audio source (Graph of QJackCtl)

DOYJOY!

If it is boring to connect every day, try [aj-snapshot](https://sourceforge.net/projects/aj-snapshot/).

# Roadmap - DOJOY

- more backends
- MIDI output so that you kind of have a chord (maybe arp) which you can connect to your DAW or a synthesiser 
- mobile app
- write chords to file
- Graphical user interface

# Credits

Adam Stark developed the math behind in a beautiful library [Chord-Detector-and-Chromagram](https://github.com/adamstark/Chord-Detector-and-Chromagram)

