#include "midi.hpp"
#include "log.hpp"
#include "midi-esp.hpp"

Midi *Midi::createInstance()
{
    if (!instance)
        instance = new MidiEsp();
    return instance;
}

Midi *Midi::instance = nullptr;