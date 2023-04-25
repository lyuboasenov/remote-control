#pragma once

#include "radio\radio\Radio.h"
#include "radio\radio\Header.h"

class Pipe {
   public:
      Pipe();
      void begin(Radio *radio);
      void update();

      bool available();

      void set_type_filter(uint8_t filter);
   protected:
   private:
      Radio *_radio;
      uint8_t _filter;
};