#pragma once

#ifdef CONTROLLER

#include <Wire.h>
#include "controller\controls\value_readers\ValueReader.h"

#define RETRY           0x10                    // 16

class AvrExtensionAnalogReader : public ValueReader {
   public:
      AvrExtensionAnalogReader(TwoWire *wire, uint8_t address, uint8_t channel);
      virtual ~AvrExtensionAnalogReader() {}
      void begin() override;
      uint16_t read_value() override;
   private:
      TwoWire *_wire;
      uint8_t _address;
      uint8_t _channel;
      uint16_t _last_value;

      uint8_t readByteFromWire(uint8_t address, uint8_t *receive_buffer, uint8_t size = 1, bool sendStop = true);
      bool read_adc_channel(uint8_t *buffer, uint8_t address, uint8_t channel);
};

#endif