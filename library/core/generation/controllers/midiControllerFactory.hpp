#pragma once

#include "apcMiniController.hpp"
#include "core/generation/controlHub/IHubController.hpp"
#include "midiController.hpp"
#include <memory>
#include <string>

class MidiControllerFactory
{
public:
    static std::unique_ptr<MidiController> create(MidiDevice *device, std::string name, ControlHub *hub)
    {
        if (name.compare("APC Mini"))
            return std::unique_ptr<ApcMiniController>(new ApcMiniController(hub, device));
        return nullptr;
    }
};