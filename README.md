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


Build
-----

The easiest way is to use Qt Creator and load ./console/console.pro

The fastest way is 

    cd console
    qmake .
    make


Usage
=====
Start the Jack Audio Engine, e.g. with QjackCtl

    ./chord-recogniser

Open the Graph in QjackCtl, where you will see a jack client named "Chord Recognizer DOJOY". Connect somethink to the input port. 

DOYJOY!

# Roadmap - DOJOY

- auto-connect to System::capture
- more backends
- MIDI output so that you kind of have a chord (maybe arp) which you can connect to your DAW or a synthesiser 
- mobile app
- write chords to file
- Graphical user interface