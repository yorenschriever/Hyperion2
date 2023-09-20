#include "../../../main.cpp"
#include "thread.hpp"
#include "driver/uart.h"

extern "C"
{
  void app_main(void);
}

void app_main(void)
{
//   Log::info("Log","Switching log output to front panel pin 8");
//   uart_config_t uart_config = {
//     .baud_rate = 115200,
//     .data_bits = UART_DATA_8_BITS,
//     .parity = UART_PARITY_DISABLE,
//     .stop_bits = UART_STOP_BITS_2,
//     .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//     .rx_flow_ctrl_thresh = 122,
// };
//   ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
//   ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, 33, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
  main();
  Thread::destroy();
}