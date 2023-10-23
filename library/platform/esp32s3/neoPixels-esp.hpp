#pragma once
#include "neoPixels.hpp"
#include "log.hpp"

#include "esp_attr.h"
#include "driver/rmt_tx.h"
#include "misc/led_strip_encoder.h"
#include "misc/pinMapping.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// #include "include/esp_pm.h"
// #include "esp32/include/esp32/pm.h"
#include "esp_pm.h"

#define RMT_LED_STRIP_RESOLUTION_HZ 40000000 // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)

class NeoPixelsEsp : public NeoPixels {

private:
    bool initialized = false;
    const char *TAG = "RMT";

    rmt_channel_handle_t led_chan;
    rmt_transmit_config_t tx_config;
    rmt_encoder_handle_t led_encoder;
    volatile bool ready_ = false;
    // volatile unsigned long readyAt = 0;
    int port;

    // static SemaphoreHandle_t xMutex;

    // uint8_t *buffer;
    // int length;
    // SemaphoreHandle_t xMutexSend = xSemaphoreCreateBinary();

public:
    NeoPixelsEsp() {}
    ~NeoPixelsEsp() = default;

    void begin(unsigned int port)
    {
        // Log::info(TAG, "begin");
    
        // Log::info(TAG, "got mutex");
        this->port = port;
        // xTaskCreatePinnedToCore(
        // beginTask,
        // "rmt",
        // 4096,
        // (void*) this,
        // 2,
        // NULL,
        // 1);
        this->begin_();
    }

    // static void beginTask(void* param)
    // {
    //     auto instance = (NeoPixelsEsp*) param;
    //     //xSemaphoreTake( xMutex, portMAX_DELAY );
    //     instance->begin_();

    //     // while(true){
    //     //     if (xSemaphoreTake(instance->xMutexSend,portMAX_DELAY)){
    //     //         // xSemaphoreTake( xMutex, portMAX_DELAY );
    //     //         instance->send_();
    //     //         // xSemaphoreGive( xMutex);
    //     //     }
    //     // }
    //     //xSemaphoreGive( xMutex );
    //     vTaskDelete(NULL);
    // }

    void begin_()
    {
        
        if (port ==0 || port > sizeof(pinMapping)){
            Log::error(TAG,"port must be between 1-%d (inclusive), but was %d", sizeof(pinMapping), port);
            return;
        }

        Log::info(TAG, "Create RMT TX channel");
        rmt_tx_channel_config_t tx_chan_config = {
            .gpio_num = (gpio_num_t) pinMapping[port-1],
            .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
            .resolution_hz = RMT_LED_STRIP_RESOLUTION_HZ,
            .mem_block_symbols = 64, // increase the block size can make the LED less flickering
            .trans_queue_depth = 1, // set the number of transactions that can be pending in the background

            .flags = {
                invert_out: 0,
                with_dma: 0,
                io_loop_back: 0,
                io_od_mode: 0 
            },
            
            .intr_priority= 3,
        };
        ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_new_tx_channel(&tx_chan_config, &led_chan));

        Log::info(TAG, "Install led strip encoder");
        led_strip_encoder_config_t encoder_config = {
            .resolution = RMT_LED_STRIP_RESOLUTION_HZ,
        };
        ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_new_led_strip_encoder(&encoder_config, &led_encoder));


        Log::info(TAG, "Enable RMT TX channel");
        ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_enable(led_chan));

        tx_config = {
            .loop_count = 0, // no transfer loop
            .flags {
                .eot_level = 0 //end of transmission level
            }
        };

        rmt_tx_event_callbacks_t rmt_tx_event_callbacks = {
            .on_trans_done = ready_cb
        };
        ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_tx_register_event_callbacks(led_chan, &rmt_tx_event_callbacks, (void*)&ready_));

        // //lock the apb clock to 80mhz
        // esp_pm_lock_handle_t apb_lock_handle;
        // esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, "rmt", &apb_lock_handle);
        // esp_pm_lock_acquire(apb_lock_handle);

        initialized = true;
        ready_ = true;
        // xSemaphoreTake( xMutexSend,0);
    }

    void send(uint8_t *buffer, int length)
    {
        if (!ready_)
            return; 
        // this->buffer = buffer;
        // this->length = length;
        // xSemaphoreGive( xMutexSend );
        // send_();

        ready_ = false;
        ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_transmit(led_chan, led_encoder, buffer, length, &tx_config));
    }

    // void send_()
    // {
    //     ready_ = false;
    //     ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_transmit(led_chan, led_encoder, buffer, length, &tx_config));
    // }

    bool ready()
    {
        return ready_;
    }

private:
    static bool IRAM_ATTR ready_cb(rmt_channel_handle_t channel, const rmt_tx_done_event_data_t *edata, void *user_data)
    {
        bool *ready = (bool *)user_data;
        *(ready) = true;
        return false;
    }
};

// SemaphoreHandle_t NeoPixelsEsp::xMutex = xSemaphoreCreateMutex();
