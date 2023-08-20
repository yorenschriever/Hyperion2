#include "midi.hpp"
#include "midi-macos.hpp"

Midi *Midi::createInstance()
{
    return new MidiMacos();
}

Midi *Midi::instance = nullptr;