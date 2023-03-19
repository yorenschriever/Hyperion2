#pragma once

#include "apcMiniController.hpp"
#include "korgNanoKontrol2.hpp"
#include "core/generation/controlHub/IHubController.hpp"
#include "midiController.hpp"
#include <memory>
#include <string>

class MidiControllerFactory
{
public:
    static std::unique_ptr<MidiController> create(MidiDevice *device, std::string name, ControlHub *hub)
    {
        if (name.compare("APC MINI")==0)
            return std::unique_ptr<ApcMiniController>(new ApcMiniController(hub, device));
        if (name.compare("nanoKONTROL2 SLIDER/KNOB")==0)
            return std::unique_ptr<KorgNanoKontrol2controller>(new KorgNanoKontrol2controller(hub,device));
        return nullptr;
    }
};