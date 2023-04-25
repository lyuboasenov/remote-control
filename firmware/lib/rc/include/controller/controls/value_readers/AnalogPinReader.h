#pragma once

#ifdef CONTROLLER

#include "ValueReader.h"

class AnalogPinReader : public ValueReader {
   public:
      AnalogPinReader(uint8_t pin);
      virtual ~AnalogPinReader() {}
      void begin() override;
      uint16_t read_value() override;
   private:
      uint8_t _pin;
};

#endif