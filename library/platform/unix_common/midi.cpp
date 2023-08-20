#include "midi.hpp"
#include "midi-unix.hpp"

Midi *Midi::createInstance()
{
    return new MidiUnix();
}

Midi *Midi::instance = nullptr;