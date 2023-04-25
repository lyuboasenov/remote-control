#pragma once

#ifdef _INCLUDE_BT

#include "BluetoothSerial.h"

#include "..\Radio.h"
#include "BTEndpoint.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

class BTRadio : public Radio {
   public:
      BTRadio();

      virtual bool begin(Endpoint *endpoint);

      virtual bool available();

      virtual bool read_header(header_t *header);
      virtual size_t read(void *buff);
      virtual size_t read(header_t *header, void *buff);

      virtual bool write(header_t *header, void *buff);
      virtual void flush();
   protected:
      BTEndpoint *cast_endpoint(Endpoint *endpoint);
   private:
      Endpoint *_endpoint;
      BluetoothSerial *_serial;
      char *_name;
};

#endif