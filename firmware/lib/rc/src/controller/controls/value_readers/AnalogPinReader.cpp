#include "controller\controls\value_readers\AnalogPinReader.h"

#ifdef CONTROLLER

AnalogPinReader::AnalogPinReader(uint8_t pin) : ValueReader(), _pin(pin) {}

void AnalogPinReader::begin() {
   ValueReader::begin();
}

uint16_t AnalogPinReader::read_value() {
   uint16_t value = analogRead(_pin);
   VALUE_READER_DEBUG(verbosity_t::debug, "ANLG:VAL:%hhu:%hu", _pin, value);
   return value;
}

#endif