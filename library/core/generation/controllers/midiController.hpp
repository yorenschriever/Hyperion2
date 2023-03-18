#pragma once
#include "midi-listener.hpp"
#include "../controlHub/IHubController.hpp"

class MidiController : public IMidiListener, public IHubController
{
public: 

    virtual ~MidiController() = default;
};