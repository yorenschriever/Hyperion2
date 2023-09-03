#pragma once

#include "dmx.hpp"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "log.hpp"
#include "strings.h"
#include <algorithm>
#include <cstring>
#include <inttypes.h>

#define HEALTHY_TIME 500 // timeout in ms
#define BUF_SIZE 1024 //  buffer size for rx events

#define DMX_IDLE 0
#define DMX_BREAK 1
#define DMX_DATA 2

class DMX_ESP : public DMX
{
private:
    static const int universeSize = 512;
    static const int startFrameSize = 1;
    const char * TAG = "DMX";

    uart_port_t dmx_uart_num;

    QueueHandle_t dmx_rx_queue;   // queue for uart rx events
    SemaphoreHandle_t sync_dmx;   // semaphore for synchronizing access to dmx array
    uint8_t dmx_state = DMX_IDLE; // status, in which received state we are
    uint16_t current_rx_addr = 0; // last received dmx channel
    long last_dmx_packet = 0;     // timestamp for the last received packet
    uint8_t dmx_data[startFrameSize + universeSize];        // stores the received dmx data
    volatile unsigned int frameCount = 0;
    //dmx_rx_buffer always contains one whole valid dmx frame, and is always ready to be read.
    //as opposed to dmx_data, which can be in the middle of being filled as dmx data comes in.
    //the contents of dmx_data will be copied into dmx_rx_buffer once 512 channel have been received or
    //when the break of the next frame is received.
    //This means that if we dont receive a full universe, we will always be 1 frame behind, because we 
    //dont know if additional channel will come in, until the next break. 
    uint8_t dmx_rx_buffer[startFrameSize + universeSize];

    SemaphoreHandle_t tx_dirtySemaphore = xSemaphoreCreateBinary();
    uint8_t dmx_tx_buffer[startFrameSize + universeSize];
    uint8_t dmx_tx_frontbuffer[startFrameSize + universeSize];
    volatile bool tx_busy = false;
    volatile int tx_size = 0;
    bool fullframe = true;
    int minchannels = 0;
    int trailingchannels = 0;

    bool initialized = false;

public:
    DMX_ESP(uart_port_t uart, int output_pin, int input_pin)
    {
        initialize(uart, output_pin, input_pin);
    }

    ~DMX_ESP() = default;

private:
    // receiving

    inline void copy_fully_filled_buffer_to_rx_buffer()
    {
        memcpy(dmx_rx_buffer, dmx_data, sizeof(dmx_data));
    }

    static void uart_event_task(void *pvParameters)
    {
        DMX_ESP *instance = (DMX_ESP *)pvParameters;
        uart_event_t event;
        uint8_t *dtmp = (uint8_t *)malloc(BUF_SIZE);
        for (;;)
        {
            // wait for data in the dmx_queue
            if (xQueueReceive(instance->dmx_rx_queue, (void *)&event, portMAX_DELAY))
            {
                bzero(dtmp, BUF_SIZE);
                switch (event.type)
                {
                case UART_DATA:
                    // read the received data
                    uart_read_bytes(instance->dmx_uart_num, dtmp, event.size, portMAX_DELAY);
                    // check if break detected
                    if (instance->dmx_state == DMX_BREAK)
                    {
                        // if not 0, then RDM or custom protocol
                        if (dtmp[0] == 0)
                        {
                            instance->dmx_state = DMX_DATA;
                            instance->current_rx_addr = 0;
                            instance->last_dmx_packet = xTaskGetTickCount();
                            instance->frameCount++;
                        }
                    }
                    // check if in data receive mode
                    if (instance->dmx_state == DMX_DATA)
                    {
                        // copy received bytes to dmx data array
                        for (int i = 0; i < event.size; i++)
                        {
                            if (instance->current_rx_addr < 513)
                            {
                                instance->dmx_data[instance->current_rx_addr++] = dtmp[i];
                            }
                        }

                        if (instance->current_rx_addr == 513)
                            instance->copy_fully_filled_buffer_to_rx_buffer();
                    }
                    break;
                case UART_BREAK:
                    // break detected
                    // clear queue und flush received bytes
                    uart_flush_input(instance->dmx_uart_num);
                    xQueueReset(instance->dmx_rx_queue);
                    instance->dmx_state = DMX_BREAK;
                    instance->copy_fully_filled_buffer_to_rx_buffer();
                    break;
                case UART_FRAME_ERR:
                case UART_PARITY_ERR:
                case UART_BUFFER_FULL:
                case UART_FIFO_OVF:
                default:
                    // error received, going to idle mode
                    uart_flush_input(instance->dmx_uart_num);
                    xQueueReset(instance->dmx_rx_queue);
                    instance->dmx_state = DMX_IDLE;
                    break;
                }
            }
        }
    }

public:
    uint8_t read(uint16_t channel) override
    {
        // restrict access to dmx array to valid values
        if (channel < 1 || channel > 512)
            return 0;

        return dmx_rx_buffer[channel];
    }

    bool isHealthy() override
    {
        if (!initialized)
            return false;

        return xTaskGetTickCount() - last_dmx_packet < HEALTHY_TIME;
    }

    int getFrameNumber()
    {
        return frameCount;
    }

    uint8_t *getDataPtr()
    {
        return dmx_rx_buffer;
    }

    //================= sending

    void write(uint8_t *data, int size, int startChannel) override
    {
        if (startChannel < 1)
        {
            Log::error(TAG,"dmx channel minimum value is 1");
            return;
        }

        int copylength = std::min(size, (startFrameSize + universeSize) - startChannel);
        if (copylength > 0)
            memcpy(dmx_tx_buffer + startChannel, data, copylength);

        //tx_size is number of dmx bytes to transmit. so: start frame byte + dmx channel bytes
        tx_size = std::max((int)tx_size, startChannel + copylength);

        //Log::info(TAG,"write ch %d, size %d, tx_size %d",startChannel, size, tx_size);
    }

private:
    void sendBuffer(uint8_t *buf, int size)
    {
        uart_wait_tx_done(dmx_uart_num, 500);

        //the esp cant send a break at the start of a message, so we send an additional 0 byte with a break at the end
        static const DRAM_ATTR char nullBuf = 0;
        uart_write_bytes_with_break(dmx_uart_num, &nullBuf, 1, 44); // break time is in bit length

        //we need to wait for the byte+break to be sent, otherwise the break will moved until after the dmx frame
        uart_wait_tx_done(dmx_uart_num, 500);

        uart_write_bytes(dmx_uart_num, buf, size);

        vTaskDelay(2 / portTICK_PERIOD_MS);   // wait a little so the tx has time to start https://www.esp32.com/viewtopic.php?t=10469
        uart_wait_tx_done(dmx_uart_num, 500); // wait till completed. at most 100 RTOS ticks (=100ms?)
        vTaskDelay(2 / portTICK_PERIOD_MS);   // wait an additional inter frame period

    }

    static void sendDMXAsync(void *pvParameters)
    {
        DMX_ESP *instance = (DMX_ESP *)pvParameters;
        while (true)
        {
            if (!xSemaphoreTake(instance->tx_dirtySemaphore, portMAX_DELAY))
                continue;

            instance->tx_busy = true;

            int frontBufferLen = instance->startFrameSize + instance->universeSize;
            if (!instance->fullframe)
            {
                frontBufferLen = std::min(std::max(instance->minchannels + instance->startFrameSize, instance->tx_size + instance->trailingchannels), instance->universeSize + instance->startFrameSize);
            }
            //Log::info(instance->TAG,"frontBufferLen %d, tx_size %d",frontBufferLen, instance->tx_size);
            memcpy(instance->dmx_tx_frontbuffer, instance->dmx_tx_buffer, frontBufferLen);
            instance->tx_size = 0;

            instance->sendBuffer(instance->dmx_tx_frontbuffer, frontBufferLen);
        
            instance->tx_busy = false;
        }
    }

    void initialize(uart_port_t uart, int output_pin, int input_pin)
    {
        if (initialized)
            return;

        Log::info(TAG, "initializing dmx");

        dmx_uart_num = uart;

        // configure UART for DMX
        uart_config_t uart_config =
            {
                .baud_rate = 250000,
                .data_bits = UART_DATA_8_BITS,
                .parity = UART_PARITY_DISABLE,
                .stop_bits = UART_STOP_BITS_2,
                .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,

                .rx_flow_ctrl_thresh = 122, //not used
                .source_clk = UART_SCLK_DEFAULT //not used
            };

        // install queue
        ESP_ERROR_CHECK(uart_driver_install(dmx_uart_num, BUF_SIZE * 2, BUF_SIZE * 2, 20, &dmx_rx_queue, ESP_INTR_FLAG_IRAM));

        ESP_ERROR_CHECK(uart_param_config(dmx_uart_num, &uart_config));

        // Set pins for UART
        uart_set_pin(dmx_uart_num, output_pin, input_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

        // create receive task
        xTaskCreate(uart_event_task, "receive_dmx", 2048, this, 12, NULL);

        // related to sending
        xTaskCreatePinnedToCore(sendDMXAsync, "send_dmx", 1024, this, 12, NULL, 1);

        //set the start frames to 0
        dmx_tx_buffer[0]=0;
        dmx_tx_frontbuffer[0]=0;

        clearTxBuffer();
        memset(dmx_data, 0, sizeof(dmx_data));

        initialized = true;
    }

public:
    void show() override
    {
        xSemaphoreGive(tx_dirtySemaphore);
    }

    bool txBusy() override
    {
        return tx_busy;
    }

    void clearTxBuffer() override
    {
        memset(dmx_tx_buffer, 0, sizeof(dmx_tx_buffer));
    }

    void sendFullFrame(bool fullframe) override
    {
        this->fullframe = fullframe;
    }

    void setUniverseSize(int minchannels, int trailingchannels) override
    {
        this->minchannels = minchannels;
        this->trailingchannels = trailingchannels;
    }
};