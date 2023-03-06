#include "rmt.hpp"
#include "log.hpp"

#include "esp_attr.h"
#include "driver/rmt_tx.h"
#include "misc/led_strip_encoder.h"
#include "misc/pinMapping.hpp"

#define RMT_LED_STRIP_RESOLUTION_HZ 10000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)

static const char *TAG = "RMT";

void RMT::begin(unsigned int port)
{
    if (port ==0 || port > sizeof(pinMapping)){
        Log::error(TAG,"port must be between 1-%d (inclusive), but was %d", sizeof(pinMapping), port);
        return;
    }

    Log::info(TAG, "Create RMT TX channel");
    rmt_tx_channel_config_t tx_chan_config = {
        .gpio_num = pinMapping[port-1],
        .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
        .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
        .mem_block_symbols = 64, // increase the block size can make the LED less flickering
        .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
        
        .flags = {
            invert_out: 0,
            with_dma: 0,
            io_loop_back: 0,
            io_od_mode: 0 
        },

    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_new_tx_channel(&tx_chan_config, &led_chan));

    Log::info(TAG, "Install led strip encoder");
    led_strip_encoder_config_t encoder_config = {
        .resolution = RMT_LED_STRIP_RESOLUTION_HZ,
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_new_led_strip_encoder(&encoder_config, &led_encoder));


    Log::info(TAG, "Enable RMT TX channel");
    ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_enable(led_chan));

    //Log::info(TAG, "Start LED rainbow chase");
    tx_config = {
        .loop_count = 0, // no transfer loop
        .flags {
            .eot_level = 0 //end of transmission level
        }
    };

    rmt_tx_event_callbacks_t rmt_tx_event_callbacks = {
        .on_trans_done = ready_cb
    };
    rmt_tx_register_event_callbacks(led_chan, &rmt_tx_event_callbacks, (void*)&ready_);
}

void RMT::send(uint8_t *buffer, int length)
{
    ready_ = false;
    ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_transmit(led_chan, led_encoder, buffer, length, &tx_config));
}

bool RMT::ready()
{
    return ready_;
}

bool IRAM_ATTR RMT::ready_cb(rmt_channel_handle_t channel, const rmt_tx_done_event_data_t *edata, void *user_data)
{
    bool *ready = (bool *)user_data;
    *(ready) = true;
    return false;
}
