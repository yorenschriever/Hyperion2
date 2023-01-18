//#include "core/pipe.hpp"
#include "platform/includes/log.hpp"
#include "platform/includes/ethernet.hpp"

class Hyperion
{
public: 
    virtual void setup()
    {
        Log::info("Hyperion","setup");

        check_safe_mode();

        setup_network();
        setup_rotary();
        setup_display();
        setup_tempo();
        setup_midi();

        // for(output: outputs){
        //     output->initialize();
        // }
    }

    virtual void run()
    {
        Log::info("Hyperion","run 123");
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
        Ethernet::Initialize("hyperion");
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
};