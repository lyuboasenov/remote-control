#pragma once

#include <esp_now.h>
#include "WiFi.h"

#include "..\Radio.h"
#include "EspNowEndpoint.h"
#include "freertos/ringbuf.h"

#ifndef RADIO_BUFFER_SIZE
#define RADIO_BUFFER_SIZE 250
#endif

typedef struct {
   header_t header;
   uint8_t payload[250];
} packet_t;

class EspNowRadio : public Radio {
   public:
      EspNowRadio();

      virtual bool begin(Endpoint *endpoint);

      bool available() override;

      bool read_header(header_t *header) override;
      size_t read(void *buff) override;

      void flush() override;
   protected:
      EspNowEndpoint *cast_endpoint(Endpoint *endpoint);
      uint16_t get_max_packet_size() override;
      bool write_internal(header_t *header, const void *buff) override;
   private:
      static EspNowRadio *_this;

      RingbufHandle_t _buf_handle;
      esp_now_peer_info_t *_peer;
      packet_t *_packet;
      bool _packet_empty;

      uint8_t _send_buffer[RADIO_BUFFER_SIZE];

      EspNowEndpoint *_endpoint;

      static void on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status);
      static void on_data_received(const uint8_t * mac, const uint8_t *incomingData, int len);
};