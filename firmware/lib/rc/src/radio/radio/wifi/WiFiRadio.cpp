#include "radio\radio\wifi\WiFiEndpoint.h"
#include "radio\radio\wifi\WiFiRadio.h"

WiFiRadio::WiFiRadio() {

}

bool WiFiRadio::begin(Endpoint *endpoint) {
   _wifi_endpoint = cast_endpoint(endpoint);

   if (_wifi_endpoint == NULL) {
      return false;
   } else {
      if (_wifi_endpoint->is_master()) {
         WiFi.softAP(_wifi_endpoint->get_ssid(), _wifi_endpoint->get_password(), 1, 0, 1);
         _server.listen(WIFI_SERVER_PORT);
      } else {
         WiFi.begin(_wifi_endpoint->get_ssid(), _wifi_endpoint->get_password(), 1);

         while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            RADIO_DEBUG(verbosity_t::trace, "WIFI:BEG:WAIT[%s:%s]", _wifi_endpoint->get_ssid(), _wifi_endpoint->get_password());
         }

         while(!_client.connect(_server_host, WIFI_SERVER_PORT, "/")){
            delay(500);
            RADIO_DEBUG(verbosity_t::trace, "WIFI:BEG:WAIT:SOC[%s:%s]", _wifi_endpoint->get_ssid(), _wifi_endpoint->get_password());
         }
      }

      RADIO_DEBUG(verbosity_t::info, "WIFI:BEG:[%s:%s:%hu]", _wifi_endpoint->get_ssid(), _wifi_endpoint->get_password(), WIFI_SERVER_PORT);
      set_initialized(true);

      return true;
   }
}

bool WiFiRadio::available() {
   if (_wifi_endpoint == NULL) {
      return false;
   } else {
      return _client.available();
   }
}

bool WiFiRadio::read_header(header_t *header) {
   RADIO_DEBUG(verbosity_t::trace, "WIFI:R_H");
   if (is_header_read()) {
      RADIO_DEBUG(verbosity_t::trace, "WIFI:R_H:AR");

      if (header != &_header) {
         memcpy(header, &_header, sizeof(header_t));
      }

      return true;
   } else if (available()) {
      RADIO_DEBUG(verbosity_t::trace, "WIFI:R_H:AV");

      WebsocketsMessage msg = _client.readBlocking();
      _input_buffer = (const uint8_t *)msg.c_str();
      _input_buffer_size = msg.length();

      memcpy(header, _input_buffer, sizeof(header_t));

      RADIO_DEBUG(
         verbosity_t::trace,
         "WIFI:R_H[%hhu:%hhu:%hhu:%02X]",
         header->index,
         header->total,
         header->size,
         header->flags);

      set_header_read(true);

      return true;
   } else {
      RADIO_DEBUG(verbosity_t::trace, "WIFI:R_H:NA");
      header = NULL;
      return false;
   }
}

size_t WiFiRadio::read(void *buff) {
   RADIO_DEBUG(verbosity_t::trace, "WIFI:R");
   if (!is_header_read() && !read_header(&_header)) {
      RADIO_DEBUG(verbosity_t::trace, "WIFI:R:NA");
      return 0;
   } else {
      read_header(&_header);
      RADIO_DEBUG(verbosity_t::trace, "WIFI:R[%hhu]", _header.size);
      memcpy(buff, _input_buffer + sizeof(header_t), _header.size);

      set_header_read(false);

      return _header.size;
   }
}

void WiFiRadio::flush() {

}

WiFiEndpoint *WiFiRadio::cast_endpoint(Endpoint *endpoint) {
   if (endpoint == NULL) {
      RADIO_DEBUG(verbosity_t::error, "WIFI:END_P:NULL");
      return NULL;
   } else if (endpoint->type() == ENDPOINT_WI_FI) {
      RADIO_DEBUG(verbosity_t::info, "WIFI:END_P:OK");
      return (WiFiEndpoint *) endpoint;
   } else {
      RADIO_DEBUG(verbosity_t::error, "WIFI:END_P:FAIL");
      return NULL;
   }
}

uint16_t WiFiRadio::get_max_packet_size() {
   return OUTPUT_BUFFER_SIZE;
}

bool WiFiRadio::write_internal(header_t *header, const void *buff) {
   memcpy(_output_buffer, header, sizeof(header_t));
   memcpy(_output_buffer + sizeof(header_t), buff, header->size);

   _client.sendBinary((const char*) &_output_buffer, header->size + sizeof(header_t));
}
