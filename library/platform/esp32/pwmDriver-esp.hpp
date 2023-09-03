#pragma once

#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "log.hpp"
#include "pwmDriver.hpp"
#include "strings.h"
#include <algorithm>
#include <cstring>
#include <inttypes.h>

#define I2C_MASTER_SCL_IO 16        /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO 13        /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_FREQ_HZ 400000  /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

const i2c_port_t I2C_MASTER_NUM = I2C_NUM_0; /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */

#define PCA9685_I2C_ADDRESS 0x40 /**< Default PCA9685 I2C Slave Address */
#define PCA9685_MODE1 0x00       /**< Mode Register 1 */
#define PCA9685_MODE2 0x01       /**< Mode Register 2 */
#define MODE1_RESTART 0x80       /**< Restart enabled */
#define PCA9685_LED0_ON_L 0x06   /**< LED0 on tick, low byte*/
#define PCA9685_PRESCALE_MIN 3   /**< minimum prescale value */
#define PCA9685_PRESCALE_MAX 255 /**< maximum prescale value */
#define MODE1_SLEEP 0x10         /**< Low power mode. Oscillator off */
#define MODE1_AI 0x20            /**< Auto-Increment enabled */
#define MODE1_EXTCLK 0x40        /**< Use EXTCLK pin clock */
#define MODE1_RESTART 0x80       /**< Restart enabled */
#define PCA9685_PRESCALE 0xFE    /**< Prescaler for PWM output frequency */

const uint8_t PWM_LED_ORDER[] = {5,  4, 3,10, 9, 8, 7, 6, 2, 0, 1, 11}; //numbering on backside, pcb has a different numbering

class PWMDriver_ESP : public PWMDriver
{
private:
    static const int maxChannels = 16;
    const char *TAG = "PWM";
    
    bool initialized = false;
    volatile bool busy = false;
    SemaphoreHandle_t dirtySemaphore = xSemaphoreCreateBinary();
    uint16_t values[16];
    uint16_t valuesBuf[16];

public:
    PWMDriver_ESP()
    {
        initialize();
    }

    ~PWMDriver_ESP() = default;

    void initialize()
    {
        if (initialized)
            return;

        Log::info(TAG, "initializing pwm");

        i2c_config_t conf = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = I2C_MASTER_SDA_IO,
            .scl_io_num = I2C_MASTER_SCL_IO,
            .sda_pullup_en = GPIO_PULLUP_DISABLE,
            .scl_pullup_en = GPIO_PULLUP_DISABLE,
            .master = {
                .clk_speed = I2C_MASTER_FREQ_HZ,
            },
            .clk_flags = 0
        };

        ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));

        ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, ESP_INTR_FLAG_IRAM));

        reset();

        xTaskCreatePinnedToCore(PCA9685Task, "pwm_task", 2048, this, 6, NULL, 1);

        initialized = true;
        setFrequency(1500);
    }

private:
    void reset()
    {
        write8(PCA9685_MODE1, MODE1_RESTART);
        vTaskDelay(10);
    }

    void write8(uint8_t addr, uint8_t d)
    {
        uint8_t write_buf[2] = {addr, d};
        ESP_ERROR_CHECK(i2c_master_write_to_device(I2C_MASTER_NUM, PCA9685_I2C_ADDRESS, write_buf, sizeof(write_buf), I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
    }

    uint8_t read8(uint8_t addr)
    {
        uint8_t data;
        ESP_ERROR_CHECK(i2c_master_write_read_device(I2C_MASTER_NUM, PCA9685_I2C_ADDRESS, &addr, 1, &data, 1, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS));
        return data;
    }

    bool writePWMData(uint8_t *data, int length, int maximumWait)
    {
        if (!initialized)
            return false;

        // todo sync with display, avoid collisions
        // if (!xSemaphoreTake(i2cMutex, (TickType_t)maximumWait))
        //     return false;

        //_i2c->setClock(1000000);
        i2c_master_write_to_device(I2C_MASTER_NUM, PCA9685_I2C_ADDRESS, data, length, maximumWait / portTICK_PERIOD_MS);

        // xSemaphoreGive(i2cMutex);
        return true;
    }

public:
    void setFrequency(int freq) override
    {
        if (!initialized)
            return;

        // Range output modulation frequency is dependent on oscillator
        if (freq < 1)
            freq = 1;
        if (freq > 3500)
            freq = 3500; // Datasheet limit is 3052=50MHz/(4*4096)

        float prescaleval = ((25000000 / (freq * 4096.0)) + 0.5) - 1;
        if (prescaleval < PCA9685_PRESCALE_MIN)
            prescaleval = PCA9685_PRESCALE_MIN;
        if (prescaleval > PCA9685_PRESCALE_MAX)
            prescaleval = PCA9685_PRESCALE_MAX;
        uint8_t prescale = (uint8_t)prescaleval;

        uint8_t oldmode = read8(PCA9685_MODE1);

        uint8_t newmode = (oldmode & ~MODE1_RESTART) | MODE1_SLEEP; // sleep
        write8(PCA9685_MODE1, newmode);                             // go to sleep
        write8(PCA9685_PRESCALE, prescale);                         // set the prescaler
        write8(PCA9685_MODE1, oldmode);
        vTaskDelay(5);
        // This sets the MODE1 register to turn on auto increment.
        write8(PCA9685_MODE1, oldmode | MODE1_RESTART | MODE1_AI);
    }

    bool ready() override
    {
        return !busy;
    }

    void write(uint8_t index, uint16_t value, bool invert) override
    {
        if (index >= maxChannels)
            return;

        if (value > 4096)
            value = 4096;

        if (invert)
            value = 4096 - value;

        values[PWM_LED_ORDER[index]] = value;
    }

    void show() override
    {
        busy = true;
        xSemaphoreGive(dirtySemaphore);
    }

private:
    static void PCA9685Task(void *param)
    {
        PWMDriver_ESP *instance = (PWMDriver_ESP *)param;
        uint8_t buffer[16 * 4 + 1];
        buffer[0] = PCA9685_LED0_ON_L;

        while (true)
        {
            if (xSemaphoreTake(instance->dirtySemaphore, portMAX_DELAY)) // wait for show() to be called
            {
                vTaskDelay(1); // i dont know why, but this makes it more stable.
                instance->busy = true;
                memcpy(instance->valuesBuf, instance->values, sizeof(instance->valuesBuf));
                int edgepos = 0;

                // this is an attempt to stagger the on times of the leds a bit, so the
                // overall power consumption does not change too abruptly.
                // it results in flicker during the frame change though, so its still turned off
                // todo make this settings
                bool cascadedPWm = false;
                bool pcaCascaded = false;

                unsigned int on, off;

                for (int i = 0; i < 16; i++)
                {
                    int value = instance->valuesBuf[i];
                    if (value == 0)
                    {
                        // this led is fully off, no pwm
                        on = 0;
                        off = 1 << 12;
                    }
                    else if (value >= 4096)
                    {
                        // this led is fully on, no pwm
                        on = 1 << 12;
                        off = 0;
                    }
                    else if (cascadedPWm)
                    {
                        // we start at the off-end of the previous led, to flatten out current
                        on = edgepos;
                        off = (edgepos + value) % 4096;
                        edgepos = off;
                    }
                    else if (pcaCascaded)
                    {
                        on = edgepos;
                        off = (on + value) % 4096;
                        edgepos += 40;
                    }
                    else
                    {
                        // we always start at the start of each pwm cycle, so we are sure to update the value when the led is off
                        on = 0;
                        off = value;
                    }
                    buffer[i * 4 + 1] = (on & 0xFF);
                    buffer[i * 4 + 2] = (on >> 8);
                    buffer[i * 4 + 3] = (off & 0xFF);
                    buffer[i * 4 + 4] = (off >> 8);
                }

                instance->writePWMData(buffer, sizeof(buffer), 40);
                vTaskDelay(1);
                instance->busy = false;
            }
        }
    }
};