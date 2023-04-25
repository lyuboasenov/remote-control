#pragma once

#ifdef CONTROLLER

#include <Arduino.h>

#ifdef VALUE_READER_DEBUG_ENABLED
#include <debug_print.h>
#define VALUE_READER_DEBUG(lvl, x,...)            DEBUG_OUTPUT(lvl, "VAL", x, ##__VA_ARGS__) //!< DEBUG
#else
#define VALUE_READER_DEBUG(lvl, x,...)                                           //!< DEBUG null
#endif

class ValueReader {
   public:
      ValueReader() {}
      virtual ~ValueReader() {}
      virtual void begin() {}
      virtual uint16_t read_value() { return 0; }
};

#endif