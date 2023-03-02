#include <vector>

#include "core/distribution/pipes/pipe.hpp"
#include "platform/includes/ethernet.hpp"
#include "platform/includes/log.hpp"

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
        }

    virtual void run()
    {
        Log::info("Hyperion", "run 123");

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

    std::vector<Pipe*> pipes;
};