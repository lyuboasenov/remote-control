#ifdef _INCLUDE_BT

#include "radio\radio\bt\BTEndpoint.h"
#include "radio\radio\bt\BTRadio.h"

BTRadio::BTRadio() : _serial(new BluetoothSerial()) { }

bool BTRadio::begin(Endpoint *endpoint) {
   BTEndpoint *btEndpoint = cast_endpoint(endpoint);
   if (btEndpoint == NULL ) {
      return false;
   } else {
      this->_serial->begin(btEndpoint->get_local_name(), btEndpoint->get_is_host());

      if (btEndpoint->get_is_host()) {
         this->_serial->connect(btEndpoint->get_remote_name());
      }

      this->_endpoint = endpoint;
      this->_initialized = true;

      return true;
   }
}

bool BTRadio::available() {
   return this->_serial->available();
}

bool read_header(header_t *header) {
   return _serial->readBytes((uint8_t *)&_header, sizeof(header_t)) == sizeof(header_t);
}

size_t read(void *buff) {
   return this->_serial->readBytes((uint8_t *)buff, Radio::get_size_in_bytes(&_header));
}

bool BTRadio::write(header_t *header, void *buff) {
   if (_serial->write((uint8_t *) header, sizeof(header_t))) {
      return _serial->write((uint8_t *) buff, Radio::get_size_in_bytes(&_header));
   } else {
      return false;
   }
}

void BTRadio::flush() {
   this->_serial->flush();
}

BTEndpoint *BTRadio::cast_endpoint(Endpoint *endpoint) {
   if (endpoint->type() == ENDPOINT_BT) {
      return (BTEndpoint *) endpoint;
   } else {
      return NULL;
   }
}

#endif
