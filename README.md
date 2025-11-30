# ChordOla!

- almost Realtime musical chord recognition
- Music can come from any client supporting JACK Audio Connection Kit

- Linux-version
- MacOS in progress

Why?
====
- [Ultimate Guitar](https://www.ultimate-guitar.com/) if somebody already wrote the chords down
- [Chordify](https://chordify.net/) for any song, but not realtime
- [Sonic Visualiser](https://www.sonicvisualiser.org/) with [Chordino and NNLS Chroma Plugin](https://www.matthiasmauch.net/) for any song, but not realtime. Complexer chords detected, loop sections. 
- [Chord ai](https://chordai.net/) on smartphone has instant chord recognition, but you cannot select the audio source for the chord recognition. Using microphone with unknown songs works well until you do not try to play along with accoustic instruments. With some configuration, the MacOS software can recognize chords from a different source - e.g. your browser - with Blackhole. This can isolate the chord recognition and it is possible to play along. Free version only detects basic chords.


ChordOla! can detect chords from any source almost realtime. So she is the optimal companion to just practice along your running playlist or movies - without interference. 


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
Make sure to have the following packages installed: build-essential cmake make libjack-jackd2-dev libfftw3-dev

You can do so on Debian-based systems with: 

    sudo apt install build-essential cmake make libjack-jackd2-dev libfftw3-dev -y

Build
-----
    mkdir build
    cd build
    cmake ..
    make

Usage
=====

ChordOla! uses JACK as a sound server. 

Easiest way to start JACK is to use QJackCtl. You can install it with: 

    sudo apt install qjackctl

Start the Jack Audio Engine (e.g. with QJackCtl)

Run ChordOla! 

    ./chordola
    

Connect a JACK audio source (Graph of QJackCtl or with qpwgraph)


MacOS
=====
Download Jack (include QJackCtl)

Install Blackhole

Set sound device to Blackhole in QJackCtl and start Jack

Create Multi-Output-Device with Blackhole and Speakers

Set sound output to Multi-Output-Device (e.g. with AudioPick for browser or system-wide)

    brew install cmake jack fftw
    
    
ToDo
-----
CMakeLists.txt points directly to homebrew include in opt



DoJoy!

If it is boring to connect every day, try [aj-snapshot](https://sourceforge.net/projects/aj-snapshot/).


# Credits

Adam Stark developed the math behind in a beautiful library [Chord-Detector-and-Chromagram](https://github.com/adamstark/Chord-Detector-and-Chromagram)

