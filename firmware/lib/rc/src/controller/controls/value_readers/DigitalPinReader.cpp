#include "controller\controls\value_readers\DigitalPinReader.h"

#ifdef CONTROLLER

DigitalPinReader::DigitalPinReader(uint8_t pin, bool pullup) : ValueReader(), _pin(pin), _pullup(pullup) {}

void DigitalPinReader::begin() {
   ValueReader::begin();
   if (_pullup) {
      VALUE_READER_DEBUG(verbosity_t::info, "DIG:BGN:%hhu:UP", _pin);
      _button.attach(_pin, INPUT_PULLUP);
   } else {
      VALUE_READER_DEBUG(verbosity_t::info, "DIG:BGN:%hhu:FL", _pin);
      _button.attach(_pin, INPUT);
   }
   _button.interval(BUTTON_RESTORE_DEFAULTS_PRESS_TIME_MS);
}

uint16_t DigitalPinReader::read_value() {
   _button.update();
   VALUE_READER_DEBUG(verbosity_t::debug, "DIG:VAL:%hhu:%s", _pin, _button.read() == LOW ? "LOW" : "HIGH");

   return _button.read() == LOW ? 0 : 65535;
}

#endif