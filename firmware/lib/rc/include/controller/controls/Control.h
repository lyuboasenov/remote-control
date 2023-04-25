#pragma once

#ifdef CONTROLLER

#include <Arduino.h>
#include "value_readers\ValueReader.h"
#include "control_t.h"

#ifdef CONTROL_DEBUG_ENABLED
#include <debug_print.h>
#define CONTROL_DEBUG(lvl, x,...)            DEBUG_OUTPUT(lvl, "CTRL", x, ##__VA_ARGS__) //!< DEBUG
#else
#define CONTROL_DEBUG(lvl, x,...)                                           //!< DEBUG null
#endif

class Control final {
   public:
      Control(const char * name, ValueReader * value_reader, control_t * data, bool auto_centered = false);

      void begin();
      uint8_t value();
      const char * get_name();
      void set_median_value();
   private:
      const char *_name;
      ValueReader *_value_reader;
      control_t *_data;
      bool _auto_centered;
};

#endif