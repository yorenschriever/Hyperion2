#include "pixelMonitor.hpp"
#include "platform/includes/log.hpp"
#include "thread.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

static const char *TAG = "PIXEL_MONITOR";

unsigned int PixelMonitor::addOutput(PixelMap map)
{
    if (begun)
    {
        Log::error(TAG, "you must call PixelMonitor::addOutput before starting the monitor.");
        return 0;
    }
    outputs.push_back({.map = map, .port = ++port});
    return port;
}

void PixelMonitor::begin()
{
    if (begun)
        return;
    begun = true;

    char tempFileName[] = "/tmp/pixelMap_XXXXXX";
    // this replaces tempFileName placeholder variable with a unique filename
    int temp_fd = mkstemp(tempFileName);

    Log::info(TAG, "monitor map file name: %s", tempFileName);

    FILE *fp;
    fp = fopen(tempFileName, "w");
    fprintf(fp, "[\n");
    int outputIndex = 0;
    for (auto output : outputs)
    {
        fprintf(fp, "  {\n");
        fprintf(fp, "    \"port\": %d,\n", output.port);
        fprintf(fp, "    \"positions\": [\n");
        int index = 0;
        for (auto pixel : output.map)
            fprintf(fp, "      {\"x\" : %f, \"y\" : %f}%s \n", pixel.x, pixel.y, index++ == output.map.size() - 1 ? "" : ",");
        fprintf(fp, "    ]\n");
        fprintf(fp, "  }%s\n", outputIndex++ == outputs.size() - 1 ? "" : ",");
    }
    fprintf(fp, "]\n");
    fclose(fp);

    Log::info(TAG, "monitor map written");

    char *command = new char[150];
    snprintf(command, 150, "python3 $HYPERION_LIB_DIR/scripts/pixelMonitor.py %s", tempFileName);

    Log::info(TAG, "starting monitor task");
    Thread::create(monitorTask, "monitorTask", Thread::Purpose::distribution, 3000, (void *)command, 0);
}

void PixelMonitor::monitorTask(void *pvParameters)
{
    const char *command = static_cast<const char *>(pvParameters);

    Log::info(TAG, "command: %s", command);

    std::array<char, 128> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        Log::info(TAG, buffer.data());
    }

    Thread::destroy();
}

std::vector<PixelMonitor::PixelMonitorOutput> PixelMonitor::outputs;
bool PixelMonitor::begun = false;
unsigned int PixelMonitor::port = 9700;