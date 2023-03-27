#include "core/distribution/pipes/pipe.hpp"
#include "core/generation/controllers/midiController.hpp"
#include "core/generation/controllers/midiControllerFactory.hpp"
#include "core/generation/patterns/helpers/tempo/tapTempo.h"
#include "core/generation/patterns/helpers/tempo/tempo.h"
#include "platform/includes/ethernet.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/midi.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"
#include <memory>
#include <vector>

class Hyperion
{
public:
    virtual void start()
    {
        Log::info("Hyperion", "start");

        check_safe_mode();

        setup_network();
        setup_rotary();
        setup_display();
        setup_tempo();
        setup_midi();

        Log::info("HYP", "starting outputs");
        for (Pipe *pipe : pipes)
            pipe->out->begin();

        clearAll();

        Log::info("Hyperion", "starting inputs");
        for (Pipe *pipe : pipes)
            pipe->in->begin();

        //Thread::create(UpdateDisplayTask, "UpdateDisplay", Thread::Purpose::control, 3000, this, 4);
        Thread::create(runTask, "run", Thread::Purpose::distribution, 30000, this, 1);
    }

    virtual void addPipe(Pipe *pipe)
    {
        pipes.push_back(pipe);
    }

    virtual void clearAll()
    {
        for (Pipe *pipe : pipes)
        {
            pipe->out->clear();
            pipe->out->show();
        }
    }

    ControlHub hub;
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
        // add tempo sources in order of importance. first has highest priority
        //  Tempo::AddSource(ProDJLinkTempo::getInstance());
        //  Tempo::AddSource(MidiClockTempo::getInstance());
        //  Tempo::AddSource(TapTempo::getInstance());
        //  Tempo::AddSource(UdpTempo::getInstance());

        // Midi::Initialize();
        // Midi::onNoteOn([](uint8_t ch, uint8_t note, uint8_t velocity) {
        //     if (note == Configuration.tapMidiNote) TapTempo::getInstance()->Tap();
        //     if (note == Configuration.tapStopMidiNote) TapTempo::getInstance()->Stop();
        //     if (note == Configuration.tapAlignMidiNote) TapTempo::getInstance()->Align();
        //     if (note == Configuration.tapBarAlignMidiNote) Tempo::AlignPhrase();
        // });

        // Rotary::onPress([]() { TapTempo::getInstance()->Tap(); });
        // Rotary::onLongPress([]() { TapTempo::getInstance()->Stop(); });
    }

    virtual void setup_midi()
    {
        Midi::initialize();
        Midi::onDeviceCreatedDestroyed(
            [](MidiDevice *device, std::string name, void *userData)
            {
                auto hyp = (Hyperion *)userData;
                auto controller = MidiControllerFactory::create(device, name, &hyp->hub);
                hyp->midiControllers.insert({device, std::move(controller)});
            },
            [](MidiDevice *device, std::string name, void *userData)
            {
                auto hyp = (Hyperion *)userData;
                //because midiControllers contains a smart pointer to the midiDevice
                //it will automatically delete the device
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

        // Thread::sleep(10);
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
    // MidiControllerFactory *midiControllerFactory;
};
