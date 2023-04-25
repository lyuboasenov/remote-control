#include "controller\controls\value_readers\MCP23017ValueReader.h"

#ifdef CONTROLLER

MCP23017ValueReader::MCP23017ValueReader(Adafruit_MCP23X17 *mcp, uint8_t pin, bool pullup) :
   _mcp(mcp), _pin(pin), _pullup(pullup) {

}

void MCP23017ValueReader::begin() {
   if (_pullup) {
      VALUE_READER_DEBUG(verbosity_t::info, "MCP:BGN:%hhu:UP", _pin);
      _mcp->pinMode(_pin, INPUT_PULLUP);
   } else {
      VALUE_READER_DEBUG(verbosity_t::info, "MCP:BGN:%hhu:FL", _pin);
      _mcp->pinMode(_pin, INPUT);
   }
}

uint16_t MCP23017ValueReader::read_value() {
   uint8_t value = _mcp->digitalRead(_pin);
   VALUE_READER_DEBUG(verbosity_t::debug, "MCP:VAL:%hhu:%s", _pin, value == LOW ? "LOW" : "HIGH");

   return value == LOW ? 0 : 65535;
}

#endif