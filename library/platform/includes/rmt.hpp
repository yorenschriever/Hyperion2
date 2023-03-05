#pragma once

#include <stdint.h>

#if ESP_PLATFORM
    #include "driver/rmt_tx.h"
#else
    typedef int rmt_channel_handle_t;
    typedef int rmt_transmit_config_t;
    typedef int rmt_encoder_handle_t; 
    typedef int rmt_tx_done_event_data_t;
#endif

class RMT
{
public:
    void begin(unsigned int port);
    void send(uint8_t *buffer, int length);
    bool ready();

private:
    rmt_channel_handle_t led_chan;
    rmt_transmit_config_t tx_config;
    rmt_encoder_handle_t led_encoder;
    volatile bool ready_;
    static bool ready_cb(rmt_channel_handle_t channel, const rmt_tx_done_event_data_t *edata, void *user_data);
};