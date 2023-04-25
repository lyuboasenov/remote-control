#ifdef _INCLUDE_RF24

#include "radio\radio\nrf24\RF24Radio.h"
#include "radio\radio\nrf24\RF24Endpoint.h"

uint64_t pipe_addresses[6] = {   0x7878787878LL,
                                 0xB3B4B5B6F1LL,
                                 0xB3B4B5B6CDLL,
                                 0xB3B4B5B6A3LL,
                                 0xB3B4B5B60FLL,   // free
                                 0xB3B4B5B605LL }; // free

RF24Radio::RF24Radio(RF24 *radio) : _radio(radio) {

}

bool RF24Radio::begin(Endpoint *endpoint) {
   RF24Endpoint *rf24Endpoint = cast_endpoint(endpoint);
   if (NULL == rf24Endpoint) {
      return false;
   } else {
      _radio->setChannel(rf24Endpoint->get_channel());
      _radio->setDataRate((rf24_datarate_e)(rf24Endpoint->get_data_rate()));
      _radio->setAutoAck(true);

      // Set the PA Level low to try preventing power supply related problems
      // because these examples are likely run with nodes in close proximity to
      // each other.
      _radio->setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.

      // save on transmission time by setting the radio to only transmit the
      // number of bytes we need to transmit
      _radio->setPayloadSize(32);  // default value is the maximum 32 bytes

      // set the RX address of the TX node into a RX pipe
      for (uint8_t i = 0; i < 6; i++) {
         _radio->openReadingPipe(i, pipe_addresses[i]);
      }

      _radio->startListening();  // put radio in RX mode

      // For debugging info
      // printf_begin();             // needed only once for printing details
      _radio->printDetails();        // (smaller) function that prints raw register values
      _radio->printPrettyDetails();  // (larger) function that prints human readable data

      this->_initialized = true;

      return true;
   }
}

bool RF24Radio::available() {
   return _radio->available();
}

bool read_header(header_t *header) {
   return _serial->readBytes((uint8_t *)&_header, sizeof(header_t)) == sizeof(header_t);
}

size_t read(void *buff) {
   return _serial->readBytes((uint8_t *)buff, _header.size);
}

// TODO: Accept payloads larger than 32 bytes
// TODO: this does not work at the moment
bool RF24Radio::write(header_t *header, void *buff) {
   RF24Endpoint *rf24Endpoint = cast_endpoint(endpoint);
   if (NULL == rf24Endpoint) {
      return false;
   } else {
      _radio->stopListening();  // put radio in TX mode
      _radio->openWritingPipe(pipe_addresses[rf24Endpoint->get_pipe()]);

      return _radio->write(buff, size);
   }
}

void RF24Radio::flush() {

}

RF24Endpoint *RF24Radio::cast_endpoint(Endpoint *endpoint) {
   if (endpoint->type() == ENDPOINT_RF24) {
      return (RF24Endpoint *) endpoint;
   } else {
      return NULL;
   }
}

#endif
