#include "controller\controls\value_readers\AvrExtensionAnalogReader.h"

#ifdef CONTROLLER

AvrExtensionAnalogReader::AvrExtensionAnalogReader(TwoWire *wire, uint8_t address, uint8_t channel) :
   ValueReader(), _wire(wire), _address(address), _channel(channel) {}

void AvrExtensionAnalogReader::begin() {
   ValueReader::begin();
}

uint8_t AvrExtensionAnalogReader::readByteFromWire(uint8_t address, uint8_t *receive_buffer, uint8_t size, bool sendStop) {
   if (_wire->requestFrom(address, (uint8_t) size, (uint8_t) sendStop)) {
      return _wire->readBytes(receive_buffer, size);
   }
}

bool AvrExtensionAnalogReader::read_adc_channel(uint8_t *buffer, uint8_t address, uint8_t channel) {
   // | RDY | O/C |  R  | CH4 | CH3 | CH2 | CH1 | CH0 |
   uint8_t config = (1 << 7) | (1 << 5) | channel;
   VALUE_READER_DEBUG(verbosity_t::debug, "AVR_ANLG:0x%02X:CFG:0x%02X\n", channel, config);

   _wire->beginTransmission(address);
   buffer[0] = 0x01;                            // ADC_CONFIG_REG
   buffer[1] = config;
   _wire->write(buffer, 2);
   _wire->endTransmission();

   buffer[0] = 0;
   buffer[1] = 0;

   config &= ~(1 << 7);
   uint8_t counter = 0;

   uint8_t result = 0;
   do {
      result = readByteFromWire(address, buffer, 3);
      VALUE_READER_DEBUG(verbosity_t::trace, "AVR_ANLG:VAL:0x%02X:0x%02X:0x%02X:0x%02X\n", result, buffer[0], buffer[1], buffer[2]);
      counter++;
   } while(buffer[0] != config && counter < RETRY);

   VALUE_READER_DEBUG(verbosity_t::trace, "AVR_ANLG:VAL:%hu:%hhu\n", (buffer[1] << 8 | buffer[2]), counter);

   return counter < RETRY;
}

uint16_t AvrExtensionAnalogReader::read_value() {
   uint8_t buffer[3];
   if (read_adc_channel(buffer, _address, _channel)) {
      _last_value = (buffer[1] << 8 | buffer[2]);
   }
   VALUE_READER_DEBUG(verbosity_t::debug, "AVR_ANLG:VAL:%hhu:%hu", _channel, _last_value);

   return _last_value;
}

#endif