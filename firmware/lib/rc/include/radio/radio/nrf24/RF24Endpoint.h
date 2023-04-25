#pragma once

#ifdef _INCLUDE_RF24

#include <Arduino.h>
#include "..\Endpoint.h"

#define ENDPOINT_RF24 0

class RF24Endpoint : public Endpoint {
   public:
      RF24Endpoint(u_int8_t channel, u_int8_t data_rate, u_int8_t pipe) : _channel(channel), _data_rate(data_rate), _pipe(pipe) {}
      uint8_t get_channel() { return _channel; }
      uint64_t get_pipe() { return _pipe; }
      uint8_t get_data_rate() { return _data_rate; }
      virtual uint8_t type() { return ENDPOINT_RF24; }
   private:
      uint8_t _channel;
      uint8_t _pipe;
      uint8_t _data_rate;
};

#endif