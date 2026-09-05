#include "dmx.hpp"
#include "log.hpp"
#include "../unix_common/dmx-unix.hpp"

DMXUnix *unixInstance = nullptr;

DMX *DMX::getInstance(unsigned int port)
{
    if (port != 0)
    {
        Log::error("dmx", "dmx instance on port %d is not available on this platform", port);
        return nullptr;
    }

    if (unixInstance == nullptr)
        unixInstance = new DMXUnix("/dev/cu.usbserial-00000000");

    return unixInstance;
}