#include "neoPixels-esp-legacyRMT.hpp"

intr_handle_t NeoPixelsEspLegacyRMT::gRMT_intr_handle = NULL;
DRAM_ATTR TransmissionContext NeoPixelsEspLegacyRMT::transmissionContext[RMT_MAX_CHANNELS];
SemaphoreHandle_t NeoPixelsEspLegacyRMT::xMutex = xSemaphoreCreateMutex();

void IRAM_ATTR NeoPixelsEspLegacyRMT::interruptHandler(void *arg)
{
    // -- The basic structure of this code is borrowed from the
    //    interrupt handler in esp-idf/components/driver/rmt.c
    uint32_t intr_st = RMT.int_st.val;
    uint8_t channel;

    for (channel = 0; channel < RMT_MAX_CHANNELS; channel++)
    {
        int tx_done_bit = channel * 3;
        int tx_next_bit = channel + 24;

        if (transmissionContext[channel].active)
        {

            // -- More to send on this channel
            if (intr_st & BIT(tx_next_bit))
            {
                RMT.int_clr.val |= BIT(tx_next_bit);
                fillNext(channel);
            }
            else
            {
                // -- Transmission is complete on this channel
                if (intr_st & BIT(tx_done_bit))
                {
                    RMT.int_clr.val |= BIT(tx_done_bit);
                    transmissionContext[channel].active = false;
                }
            }
        }
    }
}

void IRAM_ATTR NeoPixelsEspLegacyRMT::fillNext(uint8_t channel)
{
    TransmissionContext *tc = &transmissionContext[channel];
    uint32_t one_val = tc->val1;
    uint32_t zero_val = tc->val0;

    // fill 3 bytes, or less if the end of the buffer is reached
    // we have the memory to do this 4 at a time, but i copied the concept
    // from fastled, and they use 3 (to align with their pixel data)
    // i could test it with 4 someday
    int filltill = std::min(tc->bufferposition + 3, tc->length);

    while (tc->bufferposition < filltill)
    {
        // -- Use locals for speed
        volatile uint32_t *pItem = tc->mRMT_mem_ptr;
        uint16_t curPulse = tc->mCurPulse;

        uint8_t pixel = tc->buffer[tc->bufferposition++];

        // Shift bits out, MSB first, setting RMTMEM.chan[n].data32[x] to the
        // rmt_item32_t value corresponding to the buffered bit value
        for (uint32_t j = 0; j < 8; j++)
        {
            uint32_t val = (pixel & 0x80) ? one_val : zero_val;
            *pItem++ = val;
            pixel <<= 1;
        }

        curPulse += 8;
        if (curPulse == MAX_PULSES)
        {
            pItem = &(RMTMEM.chan[channel].data32[0].val);
            curPulse = 0;
        }

        // -- Store the new values back into the object
        tc->mCurPulse = curPulse;
        tc->mRMT_mem_ptr = pItem;
    }

    if (tc->bufferposition == tc->length)
    {
        // set the last bit time to the break time
        // the pulse buffer pointer can be at the start, in that case the last pulse it at the end of the buffer
        if (tc->mRMT_mem_ptr == &(RMTMEM.chan[channel].data32[0].val))
            ((rmt_item32_t *)&RMTMEM.chan[channel].data32[MAX_PULSES - 1].val)->duration1 = tc->RmtDurationReset;
        else
            ((rmt_item32_t *)transmissionContext[channel].mRMT_mem_ptr - 1)->duration1 = tc->RmtDurationReset;

        // -- No more data; signal to the RMT we are done
        // we are at least 8 pulses from the end of the buffer, otherwise we would have wrapped already
        for (uint32_t j = 0; j < 8; j++)
        {
            *tc->mRMT_mem_ptr++ = 0;
        }
    }
}

void IRAM_ATTR NeoPixelsEspLegacyRMT::begin_()
{
    xSemaphoreTake(xMutex, portMAX_DELAY);
    if (port == 0 || port > sizeof(pinMapping))
    {
        Log::error(TAG, "port must be between 1-%d (inclusive), but was %d", sizeof(pinMapping), port);
        return;
    }

    Log::info(TAG, "Create RMT TX channel");

    // there are 8 outputs and 8 rmt channels, so this mapping is trivial
    rmtchannel = static_cast<rmt_channel_t>(port - 1);

    rmt_config_t config;

    config.rmt_mode = RMT_MODE_TX;
    config.channel = rmtchannel;
    config.gpio_num = static_cast<gpio_num_t>(pinMapping[port - 1]);
    config.mem_block_num = 1;
    config.tx_config.loop_en = false;

    config.tx_config.idle_output_en = true;
    config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;

    config.tx_config.carrier_en = false;
    config.tx_config.carrier_level = RMT_CARRIER_LEVEL_LOW;

    config.clk_div = RmtClockDivider;

    ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_config(&config));
    ESP_ERROR_CHECK_WITHOUT_ABORT(rmt_set_tx_thr_intr_en(rmtchannel, true, PULSES_PER_FILL));
    if (gRMT_intr_handle == NULL)
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_intr_alloc(ETS_RMT_INTR_SOURCE, ESP_INTR_FLAG_LEVEL3, interruptHandler, 0, &gRMT_intr_handle));

    initialized = true;
    xSemaphoreGive(xMutex);
}

void IRAM_ATTR NeoPixelsEspLegacyRMT::send(uint8_t *buffer, int length)
{
    if (!ready())
        return;

    // sendbuffer = (uint8_t*)realloc(sendbuffer,length);
    // memcpy(sendbuffer,buffer,length);

    transmissionContext[rmtchannel].active = true;
    transmissionContext[rmtchannel].buffer = buffer;
    transmissionContext[rmtchannel].length = length;
    transmissionContext[rmtchannel].bufferposition = 0;
    transmissionContext[rmtchannel].mRMT_mem_ptr = &(RMTMEM.chan[rmtchannel].data32[0].val);
    transmissionContext[rmtchannel].mCurPulse = 0;
    transmissionContext[rmtchannel].val0 = Item32Val(timing.bit0_high, timing.bit0_low);
    transmissionContext[rmtchannel].val1 = Item32Val(timing.bit1_high, timing.bit1_low);
    transmissionContext[rmtchannel].RmtDurationReset = FromNs(timing.reset);

    // fill the buffer a bit. currently fillnext will add 3 bytes of pixel data to the pulse buffer.
    // the pulse buffer can hold 8 bytes of pulse data in total
    fillNext(rmtchannel);
    fillNext(rmtchannel);

    rmt_set_tx_intr_en(rmtchannel, true);
    rmt_tx_start(rmtchannel, true);
}

bool IRAM_ATTR NeoPixelsEspLegacyRMT::ready()
{
    if (!initialized)
        return false;

    return !transmissionContext[rmtchannel].active;
}
