#include "midi.hpp"
#include "log.hpp"

Midi *Midi::createInstance()
{
    Log::error("MIDI", "Midi is not supported on this platform.");
    return nullptr;
}

Midi *Midi::instance = nullptr;