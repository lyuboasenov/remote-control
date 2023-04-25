#pragma once

#ifdef _INCLUDE_RF24

#include <RF24.h>
#include "..\Radio.h"
#include "RF24Endpoint.h"

class RF24Radio : public Radio {
   public:
      RF24Radio(RF24 *radio);

      virtual bool begin(Endpoint *endpoint);

      virtual bool available();

      virtual bool read_header(header_t *header);
      virtual size_t read(void *buff);
      virtual size_t read(header_t *header, void *buff);

      virtual bool write(header_t *header, void *buff);
      virtual void flush();
   protected:
      RF24Endpoint *cast_endpoint(Endpoint *endpoint);
   private:
      RF24 *_radio;
};

#endif
