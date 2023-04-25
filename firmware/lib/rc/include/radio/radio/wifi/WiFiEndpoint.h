#pragma once

#include <Arduino.h>
#include "..\Endpoint.h"

#define ENDPOINT_WI_FI 3

class WiFiEndpoint : public Endpoint {
   public:
      WiFiEndpoint(const char * ssid, const char * password, bool master) :
      _ssid(ssid), _password(password), _master(master) {}
      const char * get_ssid() { return _ssid; }
      const char * get_password() { return _password; }
      bool is_master() { return _master; }
      virtual uint8_t type() { return ENDPOINT_WI_FI; }
   private:
      const char * _ssid;
      const char * _password;
      bool _master;
};
