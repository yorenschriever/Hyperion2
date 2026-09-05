#include "dmx.hpp"
#include "log.hpp"

#include "dmx-unix.hpp"

// const int max_port = 1;
// DMX *instances[max_port] = {nullptr};

// const int uarts[max_port] = {UART_NUM_1};
// const int input_pins[max_port] = {36};
// const int output_pins[max_port] = {32};

DMXUnix *unixInstance = nullptr;

DMX *DMX::getInstance(unsigned int port)
{
    // if (port >= max_port)
    // {
    //     Log::error("dmx", "dmx instance on port %d is not available on this platform", port);
    //     return nullptr;
    // }

    // if (instances[port] == nullptr)
    //     instances[port] = new DMX_ESP(uarts[port], output_pins[port], input_pins[port]);
    // return instances[port];

    if (port != 0)
    {
        Log::error("dmx", "dmx instance on port %d is not available on this platform", port);
        return nullptr;
    }

    if (unixInstance == nullptr)
        unixInstance = new DMXUnix();

    return unixInstance;
}