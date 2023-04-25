#include "receiver\Channel.h"
#include "receiver\Receiver.h"

#ifdef RECEIVER

Channel::Channel() {
}

void Channel::begin() {
   if (_channel_data->enabled) {
      RECEIVER_DEBUG("RCV:CH:BGN:ID:%hhu", _channel_data->id);
      RECEIVER_DEBUG("RCV:CH:BGN:PIN:%hhu", _channel_data->pin);
      RECEIVER_DEBUG("RCV:CH:BGN:MIN:%hu", _channel_data->min_us);
      RECEIVER_DEBUG("RCV:CH:BGN:MAX:%hu", _channel_data->max_us);
      RECEIVER_DEBUG("RCV:CH:BGN:FRQ:%hu", _channel_data->frequency_hz);
      RECEIVER_DEBUG("RCV:CH:BGN:VAL:%hhu", _channel_data->value);

      _servo.attach(_channel_data->pin, _channel_data->min_us, _channel_data->max_us);
      _servo.setPeriodHertz(_channel_data->frequency_hz);
      _servo.write(map(127, 0, 255, 0, 180));

      RECEIVER_DEBUG("RCV:CH:BGN:SRV:ATT:%s", _servo.attached() ? "TRUE" : "FALSE");
      RECEIVER_DEBUG("RCV:CH:BGN:SRV:VA::%d", _servo.read());
   } else {
      _servo.detach();
   }
}

void Channel::end() {
   RECEIVER_DEBUG("RCV:CH:END:ID:%hhu", _channel_data->id);
   RECEIVER_DEBUG("RCV:CH:END:PIN:%hhu", _channel_data->pin);
   _servo.detach();
}

void Channel::set_value(uint8_t value) {
   if (_channel_data->enabled) {
      _channel_data->value = value;
      _servo.write(map(_channel_data->value, 0, 255, 0, 180));
   }
}

void Channel::set_min_us(uint16_t min) {
   _channel_data->min_us = min;
   _servo.detach();
   _servo.attach(_channel_data->pin, _channel_data->min_us, _channel_data->max_us);
}

void Channel::set_max_us(uint16_t max) {
   _channel_data->max_us = max;
   _servo.detach();
   _servo.attach(_channel_data->pin, _channel_data->min_us, _channel_data->max_us);
}

void Channel::set_frequency_hz(uint16_t frequency) {
   _channel_data->frequency_hz = frequency;
   _servo.setPeriodHertz(_channel_data->frequency_hz);
}

uint8_t Channel::get_value() {
   return _channel_data->value;
}

uint16_t Channel::get_min_us() {
   return _channel_data->min_us;
}

uint16_t Channel::get_max_us() {
   return _channel_data->max_us;
}

uint16_t Channel::get_frequency_hz() {
   return _channel_data->frequency_hz;
}

channel_t *Channel::get_channel_t() {
   return _channel_data;
}

void Channel::set_channel_t(channel_t * data) {
   _channel_data = data;
}

#endif