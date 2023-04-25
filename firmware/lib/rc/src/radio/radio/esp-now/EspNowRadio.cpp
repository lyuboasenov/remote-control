#include "radio\radio\esp-now\EspNowEndpoint.h"
#include "radio\radio\esp-now\EspNowRadio.h"

EspNowRadio *EspNowRadio::_this;

EspNowRadio::EspNowRadio() { }

bool EspNowRadio::begin(Endpoint *endpoint) {
   RADIO_DEBUG(verbosity_t::info, "ESP_N:BEG");

   EspNowEndpoint *enEndpoint = cast_endpoint(endpoint);
   if (enEndpoint == NULL ) {
      RADIO_DEBUG(verbosity_t::error, "ESP_N:END_P");
      return false;
   } else {
      _buf_handle = xRingbufferCreate(1024, RINGBUF_TYPE_NOSPLIT);
      if (_buf_handle == NULL) {
         RADIO_DEBUG(verbosity_t::error, "ESP_NOW:BUFF");
         return false;
      }

      WiFi.mode(WIFI_STA);

      set_initialized(esp_now_init() == ESP_OK);

      if (is_initialized()) {
         RADIO_DEBUG(verbosity_t::debug, "ESP_NOW:ESP_INIT");
         esp_now_register_send_cb(&EspNowRadio::on_data_sent);

         if (enEndpoint->get_has_receiver()) {
            this->_peer = new esp_now_peer_info_t();
            memcpy(this->_peer->peer_addr, enEndpoint->get_receiver_mac(), 6);
            this->_peer->channel = 0;
            this->_peer->encrypt = false;
            this->_endpoint = enEndpoint;

            RADIO_DEBUG(verbosity_t::debug, "ESP_NOW:RCV %02X:%02X:%02X:%02X:%02X:%02X",
               this->_peer->peer_addr[0],
               this->_peer->peer_addr[1],
               this->_peer->peer_addr[2],
               this->_peer->peer_addr[3],
               this->_peer->peer_addr[4],
               this->_peer->peer_addr[5],
               this->_peer->peer_addr[6]);

            set_initialized(esp_now_add_peer(this->_peer) == ESP_OK);

            if (is_initialized()) {
               RADIO_DEBUG(verbosity_t::debug, "ESP_NOW:RCV+");
               esp_now_register_recv_cb(&EspNowRadio::on_data_received);

               EspNowRadio::_this = this;
            }
         } else {
            RADIO_DEBUG(verbosity_t::error, "ESP_NOW:RCV");
         }
      } else {
         RADIO_DEBUG(verbosity_t::error, "ESP_NOW:ESP_INIT");
      }

      if (is_initialized()) {
         RADIO_DEBUG(verbosity_t::info, "ESP_N:BEG:OK");
      } else {
         RADIO_DEBUG(verbosity_t::error, "ESP_N:BEG:FAIL");
      }

      return is_initialized();
   }
}

bool EspNowRadio::available() {
   if (_packet_empty) {
      RADIO_DEBUG(verbosity_t::trace, "ESP_N:AV:R:BUF");
      size_t item_size;
      _packet = (packet_t *) xRingbufferReceive(_buf_handle, &item_size, pdMS_TO_TICKS(1));

      if (_packet == NULL) {
         RADIO_DEBUG(verbosity_t::debug, "ESP_N:AV:NA");
         return false;
      } else {
         _packet_empty = false;
         RADIO_DEBUG(verbosity_t::debug, "ESP_N:AV:TRUE[%hu]", item_size);
         return true;
      }
   } else {
      RADIO_DEBUG(verbosity_t::debug, "ESP_N:AV:TRUE");
      return true;
   }
}

bool EspNowRadio::read_header(header_t *header) {
   RADIO_DEBUG(verbosity_t::trace, "ESP_N:R_H");
   if (_packet_empty && !available()) {
      RADIO_DEBUG(verbosity_t::trace, "ESP_N:R_H:NA");
      return false;
   } else {
      RADIO_DEBUG(verbosity_t::trace, "ESP_N:R_H:AV");
      RADIO_DEBUG(
         verbosity_t::trace,
         "ESP_N:R_H[%hhu:%hhu:%hhu:%02X]",
         _packet->header.index,
         _packet->header.total,
         _packet->header.size,
         _packet->header.flags);

      memcpy(header, &_packet->header, sizeof(header_t));
      RADIO_DEBUG(verbosity_t::debug, "ESP_N:R_H [%hhu/%hhu:%hhu:%02X]", header->index, header->total, header->size, header->flags);

      set_header_read(true);

      return true;
   }
}

size_t EspNowRadio::read(void *buff) {
   RADIO_DEBUG(verbosity_t::trace, "ESP_N:R");
   if (_packet_empty && !available()) {
      RADIO_DEBUG(verbosity_t::trace, "ESP_N:R:NA");
      return 0;
   } else {
      read_header(&_header);
      RADIO_DEBUG(verbosity_t::trace, "ESP_N:R[%hhu]", _header.size);
      size_t offset = _header.index * (get_max_packet_size() - sizeof(header_t));
      memcpy(buff + offset, &_packet->payload, _header.size);

      set_header_read(false);

      return _header.size + offset;
   }
}

bool EspNowRadio::write_internal(header_t *header, const void *buff) {
   RADIO_DEBUG(verbosity_t::debug, "ESP_N:W_H [%hhu/%hhu:%hhu:%02X]", header->index, header->total, header->size, header->flags);

   memcpy(_send_buffer, header, sizeof(header_t));
   memcpy(_send_buffer + sizeof(header_t), buff, header->size);

   RADIO_DEBUG(verbosity_t::debug, "ESP_N:W_P");
   RADIO_DUMP_BUFFER(verbosity_t::trace, _send_buffer, sizeof(header_t) + header->size);

   esp_err_t result = esp_now_send(_endpoint->get_receiver_mac(), _send_buffer, sizeof(header_t) + header->size);

   RADIO_DEBUG(verbosity_t::debug, "ESP_N:W:%s", (result == ESP_OK ? "TRUE" : "FALSE"));
   return result == ESP_OK;
}

void EspNowRadio::flush() {

}

EspNowEndpoint *EspNowRadio::cast_endpoint(Endpoint *endpoint) {
   if (endpoint == NULL) {
      RADIO_DEBUG(verbosity_t::error, "ESP_N:END_P:NULL");
      return NULL;
   } else if (endpoint->type() == ENDPOINT_ESP_NOW) {
      RADIO_DEBUG(verbosity_t::info, "ESP_N:END_P:OK");
      return (EspNowEndpoint *) endpoint;
   } else {
      RADIO_DEBUG(verbosity_t::error, "ESP_N:END_P:FAIL");
      return NULL;
   }
}

void EspNowRadio::on_data_sent(const uint8_t *mac_addr, esp_now_send_status_t status) {
   if (status == 0) {
      // success = "Delivery Success :)";
      RADIO_DEBUG(verbosity_t::debug, "ESP_N:W:OK");
   } else{
      // success = "Delivery Fail :(";
      RADIO_DEBUG(verbosity_t::error, "ESP_N:W:FAIL");
   }
}

void EspNowRadio::on_data_received(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
   RADIO_DEBUG(verbosity_t::debug, "ESP_N:RCV [%02X:%02X:%02X:%02X:%02X:%02X] [%d]", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5], len);

   RADIO_DUMP_BUFFER(verbosity_t::trace, incomingData, len);

   EspNowRadio *radio = EspNowRadio::_this;

   if (memcmp(mac_addr, &radio->_peer->peer_addr, 6) == 0 &&
      len >= sizeof(header_t)) {
      packet_t p;
      memcpy(&p.header, incomingData, sizeof(header_t));
      memcpy(&p.payload, incomingData + sizeof(header_t), len - sizeof(header_t));

      RADIO_DEBUG(verbosity_t::debug, "ESP_N:RCV:[%hhu:%hhu:%hhu:%02X]", p.header.index, p.header.total, p.header.size, p.header.flags);

      UBaseType_t res = xRingbufferSend(EspNowRadio::_this->_buf_handle, &p, sizeof(packet_t), pdMS_TO_TICKS(100));
      if (res != pdTRUE) {
         RADIO_DEBUG(verbosity_t::error, "ESP_N:RCV:FAIL:MEM_ACQ");
      }
   } else if (!radio->_endpoint->get_has_receiver()) {
      // received data from another esp - discarding
      RADIO_DEBUG(verbosity_t::error, "ESP_N:RCV:UNKN");
   }
}

uint16_t EspNowRadio::get_max_packet_size() {
   return 250;
}