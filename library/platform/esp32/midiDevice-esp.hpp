#pragma once

#include "log.hpp"
#include "midiDevice.hpp"
#include "midiListener.hpp"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "midiDevice.hpp"
#include "thread.hpp"
#include <algorithm>

// https://www.midi.org/specifications/item/table-1-summary-of-midi-message

// do not increase otherwise you will get a bug that the last byte cannot be read
// https://github.com/espressif/arduino-esp32/issues/1824
#define MIDI_BUF_SIZE 120

#define NUMBER_OF_CONTROLLERS 120
#define NUMBER_OF_NOTES 127

#define NOTEON 0x90
#define NOTEOFF 0x80
#define CONTROLLERCHANGE 0xB0

#define SYSEX 0xF0
#define SYSEX_NON_REALTIME 0x7E
#define SYSEX_ALL_CHANNELS 0x7f
#define SYSEX_GENERAL_INFORMATION 0x06
#define SYSEX_IDENTITY_REQUEST 0x01
#define SYSEX_IDENTITY_REPLY 0x02
#define SYSEX_END 0xF7

// the code on the atmega will insert active sense messages every 300ms
// if nothing else was sent. we use this to check if the connection is still alive
#define HEALTHY_TIME 500

struct DeviceCreatedDestroyedEntry
{
  Midi::DeviceCreatedDestroyedCallback created;
  Midi::DeviceCreatedDestroyedCallback destroyed;
  void *userData;
};

class MidiDeviceEsp : public MidiDevice
{
private:
  bool initialized = false;
  QueueHandle_t midi_rx_queue;
  uart_port_t uart;
  unsigned long lastMessageTime = 0;
  bool sendsActiveSense = false;
  const char *TAG = "MIDI";
  std::vector<DeviceCreatedDestroyedEntry> *callbacks;

public:
  const char *name = UNKNOWN_DIN_MIDI_DEVICE;
  bool connected = false;

  MidiDeviceEsp(uart_port_t uart, int rxpin, int txpin, std::vector<DeviceCreatedDestroyedEntry> *callbacks)
  {
    this->uart = uart;
    this->callbacks = callbacks;

    uart_config_t uart_config =
        {
            .baud_rate = 31250,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_2,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,

            .rx_flow_ctrl_thresh = 122, // not used
            .source_clk = UART_SCLK_DEFAULT
            // .use_ref_tick = false       // not used
        };

    ESP_ERROR_CHECK(uart_param_config(uart, &uart_config));

    // note that i use uart 2 for midi here, but connect it to pin 1 and 3, the pin where uart0 would normally
    // be. uart 0 can still be used as serial output for debug messages/core dumps by the chip of you route them somewhere else.
    initialized = uart_set_pin(uart, txpin, rxpin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) == ESP_OK;
    if (!initialized)
      return;

    // install queue
    // even though the documentation says i cannot use ESP_INTR_FLAG_IRAM here, it seems to solve the neopixelOutput flicker issue
    initialized = uart_driver_install(uart, MIDI_BUF_SIZE * 2, MIDI_BUF_SIZE * 2, 20, &midi_rx_queue, ESP_INTR_FLAG_IRAM) == ESP_OK;
    if (!initialized)
      return;

    // create receive task
    xTaskCreatePinnedToCore(uart_event_task, "Midi", 4096, this, 6, NULL, 1);
  }

  void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override
  {
    if (!initialized)
      return;
    uint8_t buffer[3];
    buffer[0] = NOTEON | channel;
    buffer[1] = note;
    buffer[2] = velocity;
    uart_write_bytes(uart, (const char *)buffer, sizeof(buffer));
  }

  void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity) override
  {
    if (!initialized)
      return;
    uint8_t buffer[3];
    buffer[0] = NOTEOFF | channel;
    buffer[1] = note;
    buffer[2] = velocity;
    uart_write_bytes(uart, (const char *)buffer, sizeof(buffer));
  }

  void sendControllerChange(uint8_t channel, uint8_t controller, uint8_t value) override
  {
    if (!initialized)
      return;
    uint8_t buffer[3];
    buffer[0] = CONTROLLERCHANGE | channel;
    buffer[1] = controller;
    buffer[2] = value;
    uart_write_bytes(uart, (const char *)buffer, sizeof(buffer));
  }

  bool waitTxDone(int timeout = 500) override
  {
    if (!initialized)
      return false;
    return uart_wait_tx_done(uart, timeout) == ESP_OK;
  };

  bool isConnected() override
  {
    if (!initialized)
      return false;

    return connected;
  }

private:
  static void uart_event_task(void *pvParameters)
  {
    auto instance = (MidiDeviceEsp *)pvParameters;
    uint8_t messageposition = 0;
    uint8_t message[4];
    while (true)
    {
      uint8_t data[1];
      int length = uart_read_bytes(instance->uart, data, 1, HEALTHY_TIME);

      if (length == -1)
      {
        Log::error(instance->TAG, "midi uart receive error");
        continue;
      }

      if (length == 0)
      {
        if (instance->sendsActiveSense)
        {
          instance->sendsActiveSense = false;
          instance->connected = false;

          Log::info(instance->TAG, "disconnect");
          for (auto cb :*(instance->callbacks))
            cb.destroyed(instance, instance->name, cb.userData);
        }
        continue;
      }

      if (!instance->connected)
      {
        Log::info(instance->TAG, "connect");
        for (auto cb :*(instance->callbacks))
            cb.created(instance, instance->name, cb.userData);
      }

      instance->lastMessageTime = xTaskGetTickCount(); // TODO remove now that we use isConnected;
      instance->connected = true;

      if (data[0] >= 0xF8)
      {
        if (data[0] == 0xFE)
          instance->sendsActiveSense = true;

        // this is a system realtime message, it is always 1 byte long, and come in between
        // other messages, so they should not reset the messageposition.
        for (auto listener : instance->listeners)
          listener->onSystemRealtime(data[0]);
        continue;
      }

      if (data[0] >= 0x80)
      {
        // detect start of a message, start filling the buffer
        message[0] = data[0];
        messageposition = 1;
      }
      else if (messageposition < sizeof(message))
      {
        // otherwise: store bytes until buffer is full
        message[messageposition] = data[0];
        messageposition++;
      }

      uint8_t channel = message[0] & 0x0F;
      uint8_t messagetype = message[0] & 0xF0;

      if (messagetype == NOTEON && messageposition == 3 && message[1] < NUMBER_OF_NOTES)
      {
        // Log::info("note on %d %d\r\n", message[1],message[2]);
        for (auto listener : instance->listeners)
          listener->onNoteOn(channel, message[1], message[2]);
      }

      if (messagetype == NOTEOFF && messageposition == 3 && message[1] < NUMBER_OF_NOTES)
      {
        for (auto listener : instance->listeners)
          listener->onNoteOff(channel, message[1], message[2]);
      }

      if (messagetype == CONTROLLERCHANGE && messageposition == 3 && message[1] < NUMBER_OF_CONTROLLERS)
      {
        // Log::info("controller %d %d\r\n", message[1],message[2]);
        for (auto listener : instance->listeners)
          listener->onControllerChange(channel, message[1], message[2]);
      }
    }
  }
};
