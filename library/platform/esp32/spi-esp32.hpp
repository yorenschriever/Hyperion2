#pragma once

#include "spi.hpp"
#include "log.hpp"
#include "utils.hpp"
#include <inttypes.h>
#include "driver/spi_master.h"
#include "esp_system.h"
#include "pinMapping.hpp"
#include <stdio.h>
#include <string.h>

class SPI_ESP: public SPI
{
private :
    esp_err_t ret;
    spi_device_handle_t handle;
    spi_host_device_t host;
    spi_transaction_t trans;

    volatile bool sendFinished = true;
    volatile unsigned long sendFinishedAt = 0;

   static void sendFinishedCallback(spi_transaction_t *trans);

public:

    SPI_ESP(spi_host_device_t host){
        this->host = host;
    }

    void begin(uint8_t clkPin, uint8_t dataPin, int frq) override{


        //Configuration for the SPI bus
        spi_bus_config_t buscfg={
            .mosi_io_num=PinMapping::map[dataPin],
            .miso_io_num=-1,
            .sclk_io_num=PinMapping::map[clkPin],
            .quadwp_io_num=-1,
            .quadhd_io_num=-1
        };

        //Configuration for the SPI device on the other side of the bus
        spi_device_interface_config_t devcfg={
            .command_bits=0,
            .address_bits=0,
            .dummy_bits=0,
            
            .mode=0,
            .clock_speed_hz=frq, 
            .spics_io_num=-1,           //chip select pin
            .queue_size=1,
            .post_cb = sendFinishedCallback,
        };

        ret=spi_bus_initialize(host, &buscfg, SPI_DMA_CH_AUTO);
        assert(ret==ESP_OK);
        ret=spi_bus_add_device(host, &devcfg, &handle);
        assert(ret==ESP_OK);
    }

    void send(uint8_t *buffer, int length) override{
        if (!ready())
            return;

        sendFinished = false;

        memset(&trans, 0, sizeof(trans));
        trans.length=length*8;
        trans.user = this;
        trans.tx_buffer=buffer;
        trans.rx_buffer=NULL;

        ESP_ERROR_CHECK_WITHOUT_ABORT(spi_device_queue_trans(handle, &trans, 0));
    }

    bool ready() override
    {
        return readyForUs(1000);
    }

    bool readyForUs(unsigned int offset)
    {
        if (!sendFinished)
            return false;
        return (Utils::micros() - sendFinishedAt > offset);
    }

    virtual ~SPI_ESP() {
        spi_bus_remove_device(handle);
    }


};

void IRAM_ATTR SPI_ESP::sendFinishedCallback(spi_transaction_t *trans)
{
    auto instance = (SPI_ESP *)trans->user;
    instance->sendFinished = true;
    instance->sendFinishedAt = Utils::micros();
}