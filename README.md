# chord-recogniser by DOJOY - the way of joy

Realtime musical chord recognition

Supported Audio-Backends
------------------------
jack

UI
--
console


Build
=====
Get the Source
--------------
! This codebase uses git submodule(s)

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

# Roadmap - DOJOY

- more backends
- MIDI output so that you kind of have a chord (maybe arp) which you can connect to your DAW or a synthesiser 
- mobile app
- write chords to file
- Graphical user interface