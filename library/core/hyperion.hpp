#pragma once
#include "colours.h"
#include "core/distribution/inputs/inputSlicer.hpp"
#include "core/distribution/inputs/inputSplitter.hpp"
#include "core/distribution/inputs/udpInput.hpp"
#include "core/distribution/inputs/dmxInput.hpp"
#include "core/distribution/inputs/patternInput.hpp"
#include "core/distribution/luts/colourCorrectionLut.hpp"
#include "core/distribution/luts/gammaLut.hpp"
#include "core/distribution/luts/incandescentLut.hpp"
#include "core/distribution/luts/laserLut.hpp"
#include "core/distribution/outputs/dmxOutput.hpp"
#include "core/distribution/outputs/monitorOutput.hpp"
#include "core/distribution/outputs/monitorOutput3d.hpp"
#include "core/distribution/outputs/neopixelOutput.hpp"
#include "core/distribution/outputs/pwmOutput.hpp"
#include "core/distribution/outputs/spiOutput.hpp"
#include "core/distribution/outputs/udpOutput.hpp"
#include "core/distribution/outputs/artnetOutput.hpp"
#include "core/distribution/pipes/convertPipe.hpp"
#include "core/distribution/pipes/pipe.hpp"
#include "core/generation/controllers/dinMidiControllerFactory.hpp"
#include "core/generation/controllers/midiBridge.hpp"
#include "core/generation/controllers/midiController.hpp"
#include "core/generation/controllers/midiControllerFactory.hpp"
#include "core/generation/palettes.hpp"
#include "core/generation/patterns/helpers/lfo.h"
#include "core/generation/patterns/helpers/lfoTempo.h"
#include "core/generation/patterns/helpers/fade.h"
#include "core/generation/patterns/helpers/tempo/constantTempo.h"
#include "core/generation/patterns/helpers/tempo/midiClockTempo.h"
#include "core/generation/patterns/helpers/tempo/proDJLinkTempo.h"
#include "core/generation/patterns/helpers/tempo/tapTempo.h"
#include "core/generation/patterns/helpers/tempo/tempo.h"
#include "core/generation/patterns/helpers/tempo/websocketTempo.h"
#include "core/generation/pixelMap.hpp"
#include "core/generation/pixelMapSplitter3d.hpp"
#include "distribution/inputs/controlHubInput.hpp"
#include "distribution/inputs/patternCycleInput.hpp"
#include "core/distribution/inputs/switchableInput.hpp"
#include "distribution/outputs/neopixelOutput.hpp"
#include "generation/controlHub/paletteColumn.hpp"
#include "generation/controllers/websocketController.hpp"
#include "platform/includes/ethernet.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/midi.hpp"
#include "platform/includes/network.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/webServer.hpp"
#include "platform/includes/webServerResponseBuilder.hpp"
#include "platform/includes/websocketServer.hpp"
#include <memory>
#include <vector>

class Hyperion
{
    const char *TAG = "Hyperion";

public:
    using Config = struct
    {
        bool network;
        bool rotary;
        bool display;
        bool midi;
        bool tempo;
        bool web;
        bool midiBridge;
    };

    static const Config maximal;
    static const Config normal;
    static const Config minimal;

    bool https = false;
    uint16_t port = 80;

    virtual void start(Config config = normal)
    {
        Log::info(TAG, "Start");

        check_safe_mode();

        if (config.network)
            setup_network();
        if (config.rotary)
            setup_rotary();
        if (config.display)
            setup_display();
        if (config.midi)
            setup_midi();
        if (config.web)
            setup_web();
        if (config.midiBridge)
            setup_midi_bridge();
        if (config.tempo)
            setup_tempo();

        Log::info(TAG, "starting outputs");
        for (Pipe *pipe : pipes)
            pipe->out->begin();

        clearAll();

        Log::info(TAG, "starting inputs");
        for (Pipe *pipe : pipes)
            pipe->in->begin();

        Log::info(TAG, "Initialization complete. Starting main loop");
        Thread::create(UpdateDisplayTask, "UpdateDisplay", Thread::Purpose::control, 3000, this, 4);
        Thread::create(runTask, "run", Thread::Purpose::distribution, 30000, this, 1);
    }

    virtual void addPipe(Pipe *pipe)
    {
        pipes.push_back(pipe);
    }

    virtual std::vector<Pipe*> getPipes()
    {
        return this->pipes;
    }

    virtual void clearAll()
    {
        for (Pipe *pipe : pipes)
        {
            pipe->out->clear();
            pipe->out->show();
        }
    }

    virtual void setMidiControllerFactory(MidiControllerFactory * midiControllerFactory)
    {
        this->midiControllerFactory = midiControllerFactory;
    }

    virtual MidiControllerFactory* getMidiControllerFactory()
    {
        if (midiControllerFactory == nullptr)
            midiControllerFactory = new MidiControllerFactory();
        
        return midiControllerFactory;
    }

    // this setting if the webserver is spun up as HTTP or HTTPS
    void useHttps(bool useHttps = true)
    {
        this->port = useHttps? 443:80;
        this->https = useHttps;
    }

    ControlHub hub;
    WebServer *webServer;
    

private:
    virtual void check_safe_mode()
    {
        // if ()
        // {
        //     setup_network();
        //     setup_display();
        // }
    }

    virtual void setup_safe_mode()
    {
    }

    virtual void setup_network()
    {
        Log::info(TAG, "starting network");

        if (!Network::getHostName())
            Network::setHostName("hyperion");

        Ethernet::initialize();
    }

    virtual void setup_rotary()
    {
    }

    virtual void setup_display()
    {
    }

    virtual void setup_tempo()
    {
        Log::info(TAG, "setup tempo");

        // add tempo sources in order of importance. first has highest priority
        Tempo::AddSource(ProDJLinkTempo::getInstance());
        // Tempo::AddSource(MidiClockTempo::getInstance());
        Tempo::AddSource(TapTempo::getInstance());
        // Tempo::AddSource(UdpTempo::getInstance());

        // Midi::Initialize();
        // Midi::onNoteOn([](uint8_t ch, uint8_t note, uint8_t velocity) {
        //     if (note == Configuration.tapMidiNote) TapTempo::getInstance()->Tap();
        //     if (note == Configuration.tapStopMidiNote) TapTempo::getInstance()->Stop();
        //     if (note == Configuration.tapAlignMidiNote) TapTempo::getInstance()->Align();
        //     if (note == Configuration.tapBarAlignMidiNote) Tempo::AlignPhrase();
        // });

        // Rotary::onPress([]() { TapTempo::getInstance()->Tap(); });
        // Rotary::onLongPress([]() { TapTempo::getInstance()->Stop(); });

        auto midi = Midi::getInstance();
        if (midi)
            midi->onDeviceCreatedDestroyed(
                [](MidiDevice *device, std::string name, void *userData)
                {
                    auto hyp = (Hyperion *)userData;
                    auto midiTempo = new MidiClockTempo();
                    hyp->midiClockTempos.insert({device, midiTempo});
                    Tempo::AddSource(midiTempo);
                    device->addMidiListener(midiTempo);
                },
                [](MidiDevice *device, std::string name, void *userData)
                {
                    auto hyp = (Hyperion *)userData;
                    auto midiTempo = hyp->midiClockTempos[device];
                    Tempo::RemoveSource(midiTempo);
                    hyp->midiClockTempos.erase(device);
                    device->removeMidiListener(midiTempo);
                    delete midiTempo;
                },
                this);

        auto websocketTempo = new WebsocketTempo(this->webServer);
        Tempo::AddListener(websocketTempo);
        Tempo::AddSource(websocketTempo);
    }

    virtual void setup_midi()
    {
        Log::info(TAG, "setup midi");

        auto midi = Midi::getInstance();
        if (midi)
            midi->onDeviceCreatedDestroyed(
                [](MidiDevice *device, std::string name, void *userData)
                {
                    auto hyp = (Hyperion *)userData;
                    auto controller = hyp->getMidiControllerFactory()->create(device, name, &hyp->hub);
                    hyp->midiControllers.insert({device, std::move(controller)});
                },
                [](MidiDevice *device, std::string name, void *userData)
                {
                    auto hyp = (Hyperion *)userData;
                    // because midiController contains a smart pointer to the midiDevice
                    // this will also automatically delete the device
                    hyp->midiControllers.erase(device);
                },
                this);
    }

    virtual void setup_web()
    {
        Log::info(TAG, "setup web");
        webServer = WebServer::createInstance(this->port, this->https);

        hub.subscribe(new WebsocketController(&hub, webServer));
    }

    virtual void setup_midi_bridge()
    {
        Log::info(TAG, "setup midi bridge");
        auto midiBridge = new MidiBridge(webServer);

        midiBridge->onDeviceCreatedDestroyed(
            [](MidiDevice *device, std::string name, void *userData)
            {
                auto hyp = (Hyperion *)userData;
                auto controller = hyp->getMidiControllerFactory()->create(device, name, &hyp->hub);
                hyp->midiControllers.insert({device, std::move(controller)});
            },
            [](MidiDevice *device, std::string name, void *userData)
            {
                auto hyp = (Hyperion *)userData;
                // because midiController contains a smart pointer to the midiDevice
                // this will also automatically delete the device
                hyp->midiControllers.erase(device);
            },
            this);
    }

    static void UpdateDisplayTask(void *parameter)
    {
        Hyperion *instance = (Hyperion *)parameter;
        unsigned long lastFpsUpdate = 0;
        bool firstRun = true;
        while (true)
        {

            unsigned long now = Utils::millis();
            unsigned long elapsedTime = now - lastFpsUpdate;

            int activeChannels = 0;
            int totalUsedFrames = 0;
            int totalMissedFrames = 0;
            int totalTotalFrames = 0;
            int totalLength = 0;
            for (Pipe *pipe : instance->pipes)
            {
                if (pipe->in->getTotalFrameCount() == 0)
                    continue;

                activeChannels++;
                totalUsedFrames += pipe->in->getUsedFrameCount();
                totalMissedFrames += pipe->in->getMissedFrameCount();
                totalTotalFrames += pipe->in->getTotalFrameCount();
                pipe->in->resetFrameCount();

                totalLength += pipe->getNumPixels();
            }

            float outFps = activeChannels == 0 ? 0 : (float)1000. * totalUsedFrames / (elapsedTime) / activeChannels;
            float inFps = activeChannels == 0 ? 0 : (float)1000. * totalTotalFrames / (elapsedTime) / activeChannels;
            float misses = totalTotalFrames == 0 ? 0 : 100.0 * (totalMissedFrames) / totalTotalFrames;
            int avgLength = activeChannels == 0 ? 0 : totalLength / activeChannels;

            lastFpsUpdate = now;

            if (firstRun)
            {
                // skip the first run, we just started measuring, the stats do not make sense yet
                Thread::sleep(500);
                firstRun = false;
                continue;
            }

            Log::info("HYP", "FPS: %d of %d (%d%% miss)\t interval: %dms \t freeHeap: %d \t avg length: %d \t channels: %d \t totalLights: %d", (int)outFps, (int)inFps, (int)misses, (int)elapsedTime, Utils::get_free_heap(), avgLength, activeChannels, totalLength);

            // Debug.printf("IPAddress: %s\r\n", Ethernet::GetIp().toString().c_str());
            // Debug.printf("Tempo source: %s\r\n", Tempo::SourceName());

            // Display::setFPS(infps,outfps,misses);
            // Display::setLeds(totalLength);
            // Display::setDMX(DMX::IsHealthy());
            // Display::setMidi(Midi::isConnected(),Midi::isStarted());
            // Display::setEthernet(Ethernet::isConnected(),Ethernet::isConnecting());
            // Display::setWifi(Configuration.wifiEnabled, Wifi::isConnected(),Wifi::isConnecting());
            // Display::show();

            Thread::sleep(500);
        }
        Thread::destroy();
    }

    virtual void run()
    {
        // Log::info("Hyperion", "running");

        for (Pipe *pipe : pipes)
            pipe->process();

        for (Pipe *pipe : pipes)
            pipe->out->postProcess();

        Thread::sleep(1);
    }

    static void runTask(void *param)
    {
        auto instance = (Hyperion *)param;
        while (1)
            instance->run();
        Thread::destroy();
    }

    std::vector<Pipe *> pipes;
    std::map<MidiDevice *, std::unique_ptr<MidiController>> midiControllers;
    std::map<MidiDevice *, MidiClockTempo *> midiClockTempos;
    MidiControllerFactory *midiControllerFactory = nullptr;
};

const Hyperion::Config Hyperion::minimal = {
    .network = true,
    .rotary = false,
    .display = false,
    .midi = false,
    .tempo = false,
    .web = false,
    .midiBridge = false,
};

const Hyperion::Config Hyperion::normal = {
    .network = true,
    .rotary = false,
    .display = false,
    .midi = true,
    .tempo = true,
    .web = true,
    .midiBridge = true,
};

const Hyperion::Config Hyperion::maximal = {
    .network = true,
    .rotary = true,
    .display = true,
    .midi = true,
    .tempo = true,
    .web = true,
    .midiBridge = true,
};