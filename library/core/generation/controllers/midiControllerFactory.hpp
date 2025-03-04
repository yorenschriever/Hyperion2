#pragma once

#include "apcMiniController.hpp"
#include "korgNanoKontrol2.hpp"
#include "midiMixController.hpp"
#include "core/generation/controlHub/IHubController.hpp"
#include "midiController.hpp"
#include <memory>
#include <string>

class MidiControllerFactory
{
public:
    virtual std::unique_ptr<MidiController> create(MidiDevice *device, std::string name, ControlHub *hub)
    {
        if (name.find("APC MINI")==0)   // Use find instead of compare since some browsers add an index to the name (Brave on windows)
            return std::unique_ptr<ApcMiniController>(new ApcMiniController(hub, device, ApcMiniController::MK1));
        if (name.find("APC mini mk2 Control")==0)
            return std::unique_ptr<ApcMiniController>(new ApcMiniController(hub, device, ApcMiniController::MK2));
        if (name.find("MIDI Mix")==0)
            return std::unique_ptr<MidiMixController>(new MidiMixController(hub,device));
        if (name.find("nanoKONTROL2 SLIDER/KNOB")==0)
            return std::unique_ptr<KorgNanoKontrol2controller>(new KorgNanoKontrol2controller(hub,device));
        Log::info("MidiControllerFactory", "Midi device not recognized: %s", name.c_str());
        return nullptr;
    }
};