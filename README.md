# chordola

- Realtime musical chord recognition
- Music can come from any client supporting JACK Audio Connection Kit

- Linux-version
- MacOS in progress

Build
=====
Get the Source
--------------
! This codebase uses git rsubmodule(s)

    git clone --recursive https://github.com/relascope/chordola.git

or

    git clone https://github.com/relascope/chordola.git
    git submodule update --init --recursive


Prerequisites
-------------
    apt install cmake make libjack-jackd2-dev libfftw3-dev -y

Build
-----

    mkdir build
    cd build
    cmake ..
    make

Usage
=====
PipeWire
--------
connect with qpwgraph

Jack
----
Start the Jack Audio Engine (e.g. with QJackCtl)

    ./chordola

Connect a JACK audio source (Graph of QJackCtl)



DoJoy!

If it is boring to connect every day, try [aj-snapshot](https://sourceforge.net/projects/aj-snapshot/).


# Credits

Adam Stark developed the math behind in a beautiful library [Chord-Detector-and-Chromagram](https://github.com/adamstark/Chord-Detector-and-Chromagram)

