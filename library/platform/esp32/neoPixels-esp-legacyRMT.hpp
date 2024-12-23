#pragma once
#include "neoPixels.hpp"
#include "log.hpp"
#include "esp_attr.h"
#include "pinMapping.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/rmt.h"
#include "soc/rmt_struct.h"
#include <algorithm>

/*
 * This class uses the legacy RMT driver. it is already deprecated, and
 * not fully accessible anymore, but i still managed to get it to work.
 * The reason i use the legacy driver is that:
 * - the new driver contains more layers of abstraction, making the interrupt code slower. 
 *   it is made impossible to skip these layers and, for example, write your own byte encoder
 *   that is more efficient
 * - the new driver lets you only access the channel that triggered the interrupt. This means that
 *   we would have 8X the number of interrupts and the corresponding overhead.
 * I set and unset a gpio pin at the start and the end of the interrupt. 
 * When look at on an oscilloscope, i saw that the pin was high more than half the time. 
 * I was very likely had to wait too long, and the rmt buffer would underrun.
 * Anyway, the new driver showed a lot of flicker, thats why i went with the old driver.
 * Newer microcontrollers, like the esp32-s3 will have dma to load the rmt data, so hopefully
 * the new driver will will without glitches there.

*/

/*
 * RMTMEM is not defined in the new driver driver anymore, so declare the structure here.
 * The linker will point it to the correct memory address.
*/
#define ETS_RMT_INTR_SOURCE                     47

typedef union {
    struct {
        uint16_t duration0 : 15; /*!< Duration of level0 */
        uint16_t level0 : 1;     /*!< Level of the first part */
        uint16_t duration1 : 15; /*!< Duration of level1 */
        uint16_t level1 : 1;     /*!< Level of the second part */
    };
    uint32_t val; /*!< Equivalent unsigned value for the RMT symbol */
} rmt_symbol_word_t;

typedef struct {
    struct {
        rmt_symbol_word_t data32[SOC_RMT_MEM_WORDS_PER_CHANNEL];
    } chan[SOC_RMT_CHANNELS_PER_GROUP];
} rmt_block_mem_t;
// RMTMEM address is declared in <target>.peripherals.ld
extern rmt_block_mem_t RMTMEM;


#define RMT_MAX_CHANNELS 8
#define MAX_PULSES 64      /* A channel has a 64 "pulse" buffer */
#define PULSES_PER_FILL 24 /* One pixel's worth of pulses */

struct TransmissionContext
{
    bool active = false;
    uint8_t *buffer = NULL;
    int length = 0;
    volatile int bufferposition = 0;
    volatile uint32_t *mRMT_mem_ptr = NULL;
    uint16_t mCurPulse = 0;
    uint32_t val1;
    uint32_t val0;
    uint32_t RmtDurationReset = 0;
};


class NeoPixelsEspLegacyRMT : public NeoPixels {

private:
    bool initialized = false;
    const char *TAG = "RMT";
    int port;
    static SemaphoreHandle_t xMutex;
    rmt_channel_t rmtchannel;
    int length;
    static intr_handle_t gRMT_intr_handle;
    static TransmissionContext transmissionContext[RMT_MAX_CHANNELS];
    NeoPixels::Timing timing;

    //I decided not to double buffer, since the pipes check whether the driver is still busy sending.
    //This saves a bit of memory. If this class is being used by anything else than a pipe/output,
    //i could be worth to prefer stability over memory, and enabling the double buffer.
    //uint8_t *sendbuffer = NULL;

public:
    NeoPixelsEspLegacyRMT() {}
    ~NeoPixelsEspLegacyRMT() = default;

    void begin(unsigned int port, NeoPixels::Timing timing)
    {
        this->port = port;
        this->timing = timing;
       
        //initialize of the driver on the correct core (core1), to the interrupt will take place 
        //on the least busy core.
        xTaskCreatePinnedToCore(beginTask,"rmt",4096,(void*) this,3,NULL,1);
    }

    static void beginTask(void* param)
    {
        auto instance = (NeoPixelsEspLegacyRMT*) param;
        instance->begin_();
        vTaskDelete(NULL);
    }

    void begin_();
    void send(uint8_t *buffer, int length);
    bool ready();

    inline constexpr static IRAM_ATTR uint32_t Item32Val(uint16_t nsHigh, uint16_t nsLow)
    {
        return (FromNs(nsLow) << 16) | (1 << 15) | (FromNs(nsHigh));
    }

    inline constexpr static IRAM_ATTR uint32_t FromNs(uint32_t ns)
    {
        return ns / NsPerRmtTick;
    }

private:

    static void IRAM_ATTR interruptHandler(void *arg);
    static void IRAM_ATTR fillNext(uint8_t channel);

    const static DRAM_ATTR uint8_t RmtClockDivider = 2;
    const static DRAM_ATTR uint32_t RmtCpu = 80000000L; // 80 mhz RMT clock
    const static DRAM_ATTR uint32_t NsPerSecond = 1000000000L;
    const static DRAM_ATTR uint32_t RmtTicksPerSecond = (RmtCpu / RmtClockDivider);
    const static DRAM_ATTR uint32_t NsPerRmtTick = (NsPerSecond / RmtTicksPerSecond); // about 25

};

