#pragma once
#include <stdint.h>
#include <array>
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

#include "dmx.hpp"

constexpr char devicePath[] = "/dev/cu.usbserial-00000000";
constexpr unsigned long dmxBaudRate = 250000;
constexpr useconds_t breakDurationMicroseconds = 100;
constexpr useconds_t markAfterBreakMicroseconds = 12;
constexpr std::size_t dmxChannelCount = 512;

constexpr char TAG[] = "DMXUnix";

class DMXUnix : public DMX
{
private:
    int fd = -1; // file descriptor for the DMX device
    unsigned char txBuffer[dmxChannelCount+1];

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
            std::cerr << "Unable to start DMX break on " << devicePath << ": " << std::strerror(errno) << '\n';
            return false;
        }
        if (usleep(breakDurationMicroseconds) == -1) {
            const int errorNumber = errno;
            ioctl(serialPort, TIOCCBRK);
            std::cerr << "Unable to hold DMX break on " << devicePath << ": " << std::strerror(errorNumber) << '\n';
            return false;
        }
        if (ioctl(serialPort, TIOCCBRK) == -1 || usleep(markAfterBreakMicroseconds) == -1) {
            std::cerr << "Unable to finish DMX break on " << devicePath << ": " << std::strerror(errno) << '\n';
            return false;
        }

        if (!writeAll(serialPort, frame, frameSize) || tcdrain(serialPort) == -1) {
            std::cerr << "Unable to send DMX frame to " << devicePath << ": " << std::strerror(errno) << '\n';
            return false;
        }

        return true;
    }

public:
    DMXUnix() {
        initialize();
        txBuffer[0] = 0; // initialize the first byte of the DMX buffer to 0
    }

    //read is not implemented
    uint8_t read(uint16_t channel) override { return 0; } 
    bool isHealthy() override { return false; }  // returns true when a valid DMX signal has been received within the last 500ms
    int getFrameNumber() override { return 0; }           // get the number of frames received
    uint8_t *getDataPtr() override { return nullptr; }
    
    void initialize()
    {
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

        // int copylength = std::min(size, (startFrameSize + universeSize) - startChannel);
        int copylength = std::min(len, (int)(sizeof(txBuffer) - startChannel));
        if (copylength > 0)
            memcpy(txBuffer + startChannel, data, copylength);

        //tx_size is number of dmx bytes to transmit. so: start frame byte + dmx channel bytes
        // tx_size = std::max((int)tx_size, startChannel + copylength);

    }

    void show() override {
        sendDmxFrame(fd, txBuffer, sizeof(txBuffer));
    }                                   
    bool ready() override { return true; };
    void clearTxBuffer() override {};

    //TODO
    void sendFullFrame(bool) override {};                                
    void setUniverseSize(int minsize, int trailingchannels) override {}; 

    ~DMXUnix() override {
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
    }; 
};