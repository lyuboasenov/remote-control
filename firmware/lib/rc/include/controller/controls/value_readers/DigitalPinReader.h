#pragma once

#ifdef CONTROLLER

#include "ValueReader.h"
#include <Bounce2.h>

#define BUTTON_RESTORE_DEFAULTS_PRESS_TIME_MS (100)  // time the button to restore defaults has to be pressed to become active

class DigitalPinReader : public ValueReader {
   public:
      DigitalPinReader(uint8_t pin, bool pullup);
      virtual ~DigitalPinReader() {}
      void begin() override;
      uint16_t read_value() override;
   private:
      Bounce _button;
      uint8_t _pin;
      bool _pullup;
};

#endif