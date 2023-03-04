#include <vector>

#include "core/distribution/pipes/pipe.hpp"
#include "platform/includes/ethernet.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/thread.hpp"
#include "platform/includes/utils.hpp"


class Hyperion
{
public:
    virtual void setup()
    {
        Log::info("Hyperion", "setup");

        check_safe_mode();

        setup_network();
        setup_rotary();
        setup_display();
        setup_tempo();
        setup_midi();

        Log::info("HYP", "starting outputs");
        for (Pipe* pipe : pipes)
            pipe->out->begin();

        clearAll();

        Log::info("Hyperion", "starting inputs");
        for (Pipe* pipe : pipes)
            pipe->in->begin();

        Thread::create(UpdateDisplayTask,"UpdateDisplay",Thread::Purpose::distribution,3000,this,0);
    }

    virtual void run()
    {
        //Log::info("Hyperion", "run 123");

        for (Pipe* pipe : pipes)
            pipe->process();

        for (Pipe* pipe : pipes)
            pipe->out->postProcess();
    }

    virtual void addPipe(Pipe* pipe) {
        pipes.push_back(pipe);
    }

    virtual void clearAll()
    {
        for (Pipe* pipe : pipes)
        {
            pipe->out->clear();
            pipe->out->show();
        }
    }
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
    }

    virtual void setup_midi()
    {
    }

    static void UpdateDisplayTask(void *parameter)
    {
        Hyperion* instance = (Hyperion*) parameter;
        unsigned long lastFpsUpdate = 0;
        bool firstRun=true;
        while (true)
        {

            unsigned long now = Utils::millis();
            unsigned long elapsedTime = now - lastFpsUpdate;

            int activeChannels = 0;
            int totalUsedFrames = 0;
            int totalMissedFrames = 0;
            int totalTotalFrames = 0;
            int totalLength = 0;
            for (Pipe* pipe : instance->pipes)
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
                //skip the first run, we just started measuring, the stats do not make sense yet
                Thread::sleep(500);
                firstRun = false;
                continue;
            }

            Log::info("HYP","FPS: %d of %d (%d%% miss)\t interval: %dms \t freeHeap: %d \t avg length: %d \t channels: %d \t totalLights: %d", (int)outFps, (int)inFps, (int)misses, (int)elapsedTime, Utils::get_free_heap(), avgLength, activeChannels, totalLength);
            


            //Debug.printf("IPAddress: %s\r\n", Ethernet::GetIp().toString().c_str());
            //Debug.printf("Tempo source: %s\r\n", Tempo::SourceName());

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

    std::vector<Pipe*> pipes;
};