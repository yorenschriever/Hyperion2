#pragma once
#include <stdint.h>
#include <array>
#include <atomic>
#include <chrono>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <IOKit/serial/ioss.h>
#include <iostream>
#include <sys/ioctl.h>
#include <termios.h>
#include <thread>
#include <unistd.h>
#include <utility>

#include "dmx.hpp"

constexpr unsigned long dmxBaudRate = 250000;
constexpr useconds_t breakDurationMicroseconds = 100;
constexpr useconds_t markAfterBreakMicroseconds = 12;
constexpr std::size_t dmxChannelCount = 512;

constexpr char TAG[] = "DMXUnix";

class DMXUnix : public DMX
{
private:
    const char* devicePath; 
    int fd = -1; // file descriptor for the DMX device
    unsigned char bufferA[dmxChannelCount+1];
    unsigned char bufferB[dmxChannelCount+1];
    unsigned char *backBuffer = bufferA;  // written to by write()
    unsigned char *frontBuffer = bufferB; // being (or last) transmitted
    std::atomic<bool> busy{false}; // true while frontBuffer is being transmitted on txThread
    std::thread txThread;

    volatile int tx_size = 0;
    bool fullframe = true;
    int minchannels = 0;
    int trailingchannels = 0;

    void transmit() {

        int frontBufferLen = 1 + dmxChannelCount;
        if (!fullframe)
        {
            frontBufferLen = 1+std::min(
                std::max(minchannels, tx_size + trailingchannels), 
                (int)dmxChannelCount);
        }

        sendDmxFrame(fd, frontBuffer, frontBufferLen);
        tx_size=0;
        busy.store(false, std::memory_order_release);
        
    }

    bool writeAll(int serialPort, const unsigned char* data, std::size_t size) {
        while (size > 0) {
            const ssize_t bytesWritten = ::write(serialPort, data, size); // ::write forces the POSIX call, avoiding this class's write() override
            if (bytesWritten == -1) {
                if (errno == EINTR) {
                    continue;
                }
                return false;
            }
            data += bytesWritten;
            size -= static_cast<std::size_t>(bytesWritten);
        }
        return true;
    }

    bool sendDmxFrame(int serialPort, const unsigned char* frame, std::size_t frameSize) {
        if (tcflush(serialPort, TCIOFLUSH) == -1 || ioctl(serialPort, TIOCSBRK) == -1) {
            Log::error(TAG, "Unable to start DMX break on %s: %s", devicePath, std::strerror(errno));
            return false;
        }
        if (usleep(breakDurationMicroseconds) == -1) {
            const int errorNumber = errno;
            ioctl(serialPort, TIOCCBRK);
            Log::error(TAG, "Unable to hold DMX break on %s: %s", devicePath, std::strerror(errorNumber));
            return false;
        }
        if (ioctl(serialPort, TIOCCBRK) == -1 || usleep(markAfterBreakMicroseconds) == -1) {
            Log::error(TAG, "Unable to finish DMX break on %s: %s", devicePath, std::strerror(errno));
            return false;
        }

        if (!writeAll(serialPort, frame, frameSize) || tcdrain(serialPort) == -1) {
            Log::error(TAG, "Unable to send DMX frame to %s: %s", devicePath, std::strerror(errno));
            return false;
        }

        return true;
    }

public:
    DMXUnix(const char* devicePath) {
        this->devicePath = devicePath;
        initialize();
    }

    //read is not implemented
    uint8_t read(uint16_t channel) override { return 0; } 
    bool isHealthy() override { return false; }  // returns true when a valid DMX signal has been received within the last 500ms
    int getFrameNumber() override { return 0; }           // get the number of frames received
    uint8_t *getDataPtr() override { return nullptr; }
    
    void initialize()
    {
        bufferA[0] = 0; // initialize the start code of both buffers to 0
        bufferB[0] = 0;

        const int serialPort = open(devicePath, O_RDWR | O_NOCTTY);
        if (serialPort == -1) {
            Log::error(TAG, "Unable to open %s: %s", devicePath, std::strerror(errno));
            return;
        }

        termios settings{};
        if (tcgetattr(serialPort, &settings) == -1) {
            Log::error(TAG, "Unable to read settings for %s: %s", devicePath, std::strerror(errno));
            close(serialPort);
            return;
        }

        cfmakeraw(&settings);
        cfsetispeed(&settings, B9600);
        cfsetospeed(&settings, B9600);
        settings.c_cflag &= ~(CSIZE | PARENB);
        settings.c_cflag |= CS8 | CSTOPB | CLOCAL | CREAD;

        if (tcsetattr(serialPort, TCSANOW, &settings) == -1) {
            Log::error(TAG, "Unable to configure %s: %s", devicePath, std::strerror(errno));
            close(serialPort);
            return;
        }

        unsigned long baudRate = dmxBaudRate;
        if (ioctl(serialPort, IOSSIOSPEED, &baudRate) == -1) {
            Log::error(TAG, "Unable to set 250000 baud for %s: %s", devicePath, std::strerror(errno));
            close(serialPort);
            return;
        }

        fd = serialPort;
        return;
    }

    void write(const uint8_t *data, int len, int startChannel) override {
        if (startChannel < 1)
        {
            Log::error(TAG,"dmx channel minimum value is 1");
            return;
        }

        int copylength = std::min(len, ((int)dmxChannelCount + 1 - startChannel));
        if (copylength > 0)
            memcpy(backBuffer + startChannel, data, copylength);

        //tx_size is number of dmx bytes to transmit. so: start frame byte + dmx channel bytes
        tx_size = std::max((int)tx_size, startChannel + copylength);

    }

    void show() override {
        bool expected = false;
        if (!busy.compare_exchange_strong(expected, true)) {
            return; // previous frame still transmitting
        }
        if (txThread.joinable()) {
            txThread.join(); // reap the finished worker before starting a new one
        }
        std::swap(frontBuffer, backBuffer);
        memcpy(backBuffer, frontBuffer, dmxChannelCount+1); 
        txThread = std::thread(&DMXUnix::transmit, this);
    }
    bool ready() override { return !busy.load(std::memory_order_acquire); };
    void clearTxBuffer() override {};


    void sendFullFrame(bool enable) override {
        fullframe = enable;
    };                                
    void setUniverseSize(int minsize, int trailingchannels) override {
        this->minchannels = minsize;;
        this->trailingchannels = trailingchannels;
    }; 

    ~DMXUnix() override {
        if (txThread.joinable()) {
            txThread.join();
        }
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
    }; 
};