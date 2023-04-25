#pragma once

#ifdef CONTROLLER

#include "controller\controls\value_readers\ValueReader.h"
#include <Adafruit_MCP23X17.h>

class MCP23017ValueReader : public ValueReader {
   public:
      MCP23017ValueReader(Adafruit_MCP23X17 *mcp, uint8_t pin, bool pullup);
      virtual ~MCP23017ValueReader() {}
      void begin() override;
      uint16_t read_value() override;
   private:
      Adafruit_MCP23X17 *_mcp;
      uint8_t _pin;
      bool _pullup;
};

#endif