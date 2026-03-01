
#pragma once

#include "../interfaces.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/websocketServer.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/thread.hpp"
#include <string>
#include <algorithm>

class RepeatFallback final:  public IConverter
{
    unsigned long lastFrameTime = 0;
    int fallbacktimeout;

public: 

    RepeatFallback(int fallbacktimeout)
    {
        this->fallbacktimeout = fallbacktimeout;
    }

    bool ready() override { return true; }

    const Buffer process(const Buffer& inputBuffer) override
    {
        lastFrameTime = Utils::millis();
        return inputBuffer;
    }
    
    
};

