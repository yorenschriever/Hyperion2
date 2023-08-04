#pragma once

#include "spi.hpp"
#include <inttypes.h>
#include "driver/spi_master.h"
#include "esp_system.h"
#include "misc/pinMapping.hpp"
#include <stdio.h>
#include <string.h>

#define SENDER_HOST HSPI_HOST

class SPI_ESP: public SPI
{
public:
    void begin() override{


        //Configuration for the SPI bus
        spi_bus_config_t buscfg={
            .mosi_io_num=pinMapping[1],
            .miso_io_num=-1,
            .sclk_io_num=pinMapping[0],
            .quadwp_io_num=-1,
            .quadhd_io_num=-1
        };

        //Configuration for the SPI device on the other side of the bus
        spi_device_interface_config_t devcfg={
            .command_bits=0,
            .address_bits=0,
            .dummy_bits=0,
            
            //.duty_cycle_pos=128,        //50% duty cycle
            .mode=0,
            .clock_speed_hz=500000,
            //.spics_io_num=-1,           //chip select pin
            //.cs_ena_posttrans=3,        //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
            .queue_size=1
        };

        ret=spi_bus_initialize(SENDER_HOST, &buscfg, SPI_DMA_CH_AUTO);
        assert(ret==ESP_OK);
        ret=spi_bus_add_device(SENDER_HOST, &devcfg, &handle);
        assert(ret==ESP_OK);
    }

    void send(uint8_t *buffer, int length) override{
        spi_transaction_t t;
        memset(&t, 0, sizeof(t));
        t.length=length*8;
        t.tx_buffer=buffer;
        t.rx_buffer=NULL;

        spi_device_transmit(handle, &t);
    }

    bool ready() override{
        //spi_transaction_t description;
        //spi_device_get_trans_result(handle, &description,0);

        //return description.;  
        return true; //TODO this spi implementation is not async. the main loop will halt until all pixel data is sent out. use the spi queue functionality.     
    }

    virtual ~SPI_ESP() {
        spi_bus_remove_device(handle);
    }

private :
            esp_err_t ret;
        spi_device_handle_t handle;
};
