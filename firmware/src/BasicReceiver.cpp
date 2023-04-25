#include "BasicReceiver.h"

#ifdef RECEIVER

const uint8_t receiver_mac[] = { 0x7C, 0x9E, 0xBD, 0xE2, 0xF8, 0x6C };

BasicReceiver::BasicReceiver() {
   _radio = new EspNowRadio();
   _endpoint = new EspNowEndpoint(receiver_mac);
}

void BasicReceiver::begin() {
   Receiver::begin();
   RECEIVER_DEBUG("RCV:BGN");
   _radio->begin(_endpoint);
}

Radio * BasicReceiver::get_radio() {
   return _radio;
}

Endpoint * BasicReceiver::get_endpoint() {
   return _endpoint;
}

#endif