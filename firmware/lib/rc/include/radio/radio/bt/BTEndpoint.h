#pragma once

#ifdef _INCLUDE_BT

#include <Arduino.h>
#include "..\Endpoint.h"

#define ENDPOINT_BT 1

class BTEndpoint : public Endpoint {
   public:
      BTEndpoint(String local_name) : _is_host(false), _local_name(local_name), _remote_name("") { }
      BTEndpoint(String local_name, String remote_name) : _is_host(true), _local_name(local_name), _remote_name(remote_name) { }
      bool get_is_host() { return _is_host; }
      String get_local_name() { return _local_name; }
      String get_remote_name() { return _remote_name; }
      virtual uint8_t type() { return ENDPOINT_BT; }
   private:
      bool _is_host;
      String _local_name;
      String _remote_name;
};

#endif