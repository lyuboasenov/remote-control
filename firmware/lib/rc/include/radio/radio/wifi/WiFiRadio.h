#pragma once

#include "..\Radio.h"
#include "radio\radio\wifi\WiFiEndpoint.h"
#include <ArduinoWebsockets.h>
#include <WiFi.h>

#ifndef WIFI_SERVER_PORT
#define WIFI_SERVER_PORT        8888
#endif

#ifndef OUTPUT_BUFFER_SIZE
#define OUTPUT_BUFFER_SIZE        10000
#endif

#ifndef INPUT_BUFFER_SIZE
#define INPUT_BUFFER_SIZE        10000
#endif

using namespace websockets;

class WiFiRadio : public Radio {
   public:
      WiFiRadio();

      bool begin(Endpoint *endpoint) override;

      bool available() override;

      bool read_header(header_t *header) override;
      size_t read(void *buff) override;

      bool write(header_t *header, const void *buff);
      void flush() override;

   protected:
      WiFiEndpoint *cast_endpoint(Endpoint *endpoint);

      uint16_t get_max_packet_size() override;
      bool write_internal(header_t *header, const void *buff) override;

   private:
      WebsocketsServer _server;
      WebsocketsClient _client;
      WiFiEndpoint *_wifi_endpoint;

      const uint8_t *_input_buffer;
      uint32_t _input_buffer_size;

      uint8_t _output_buffer[OUTPUT_BUFFER_SIZE];
      static constexpr char* _server_host = "192.168.4.1";
};
