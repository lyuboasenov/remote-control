#pragma once

#include <esp_now.h>

#include <Arduino.h>
#include "..\Endpoint.h"

#define ENDPOINT_ESP_NOW 2

class EspNowEndpoint : public Endpoint {
   public:
      EspNowEndpoint() : _has_receiver(false) { }
      EspNowEndpoint(const uint8_t *receiver_mac) : _has_receiver(true) {
         memcpy(this->_receiver_mac, receiver_mac, 6);
      }
      uint8_t *get_receiver_mac() { return _receiver_mac; }
      void set_receiver_mac(const uint8_t *receiver_mac) {
         memcpy(this->_receiver_mac, receiver_mac, 6);
         this->_has_receiver = true;
      }
      bool get_has_receiver() { return _has_receiver; }
      virtual uint8_t type() { return ENDPOINT_ESP_NOW; }
   private:
      bool _has_receiver;
      uint8_t _receiver_mac[6];
};
