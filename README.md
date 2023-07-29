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
Start the Jack Audio Engine

    ./chord-recogniser

chord-recogniser connects at startup to:
* every readable system client (system capture)
* everything that has a connection to system playback

So, any microphone or guitar will be connected as well as a media player which is currently playing. 

The connections will be periodically reestablished.  

DOYJOY!

# Roadmap - DOJOY

- more backends
- MIDI output so that you kind of have a chord (maybe arp) which you can connect to your DAW or a synthesiser 
- mobile app
- write chords to file
- Graphical user interface