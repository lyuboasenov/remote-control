#include "receiver\Receiver.h"

#ifdef RECEIVER

Receiver::Receiver() { }

void Receiver::begin() {
   _preferences.begin("RCV", false);

   ESP32PWM::allocateTimer(0);
   ESP32PWM::allocateTimer(1);
   ESP32PWM::allocateTimer(2);
   ESP32PWM::allocateTimer(3);

   read_memory_channels();
   for (uint8_t i = 0; i < CHANNELS_COUNT; i++) {
      _channels[i].set_channel_t(&_channels_data[i]);
      if (i < _channel_count) {
         _channels[i].begin();
      }
   }

   #ifdef __FPV__
   _fpv.begin(get_radio());
   #endif
}

void Receiver::update() {
   while(get_radio()->available()) {
      if (get_radio()->read_header(&_header)) {
         RECEIVER_DEBUG("RCV:R_H [%hhu/%hhu, %hhu, 0x%02X]", _header.index, _header.total, _header.size, _header.flags);
         size_t size = _header.size;

         if (HEADER_FLAGS_TYPE_IS_CHANNELS(_header.flags)) {
            // Channels
            RECEIVER_DEBUG("RCV:CH");
            if (size == 0) {
               RECEIVER_DEBUG("RCV:CH:REQ");
               get_radio()->read(&_buffer);                          // clear buffer
               send_channels();
            } else if (size > 0) {
               RECEIVER_DEBUG("RCV:CH:RCV");
               read_channels();
            }
         } else if (HEADER_FLAGS_TYPE_IS_UINT8(_header.flags)) {
            RECEIVER_DEBUG("RCV:UINT8");
            if (size > 0) {
               if (get_radio()->read(&_buffer) > 0) {
                  for (uint8_t i = 0; i < size && i < _channel_count; i++) {
                     _channels[i].set_value(_buffer[i]);
                     RECEIVER_DEBUG("RCV:CMD:%hhu: %hu", i, _buffer[i]);
                  }
               }
            }
         }
      }
   }

   #ifdef __FPV__
   _fpv.update();
   #endif
}

bool Receiver::set_channels(Channel *channels, uint8_t count) {
   RECEIVER_DEBUG("RCV:SC");

   memcpy(&_channels, channels, sizeof(Channel) * min(count, (uint8_t) CHANNELS_COUNT));
   _channel_count = count;

   if (count < CHANNELS_COUNT) {
      return true;
   } else {
      return false;
   }
}

void Receiver::read_channels() {
   header_t header;
   get_radio()->read_header(&header);
   uint8_t size = header.size;

   _channel_count = size / sizeof(channel_t);

   for (uint8_t i = 0; i < CHANNELS_COUNT; i++) {
      _channels[i].end();
   }

   get_radio()->read(&_channels_data);

   _preferences.remove("CH");
   _preferences.putBytes("CH", &_channels_data, sizeof(channel_t) * _channel_count);

   for (uint8_t i = 0; i < _channel_count; i++) {
      _channels[i].set_channel_t(&_channels_data[i]);
      _channels[i].begin();

      RECEIVER_DEBUG("RCV:CH:RCV:ID:%hhu", _channels_data[i].id);
      RECEIVER_DEBUG("RCV:CH:RCV:PIN:%hhu", _channels_data[i].pin);
      RECEIVER_DEBUG("RCV:CH:RCV:MIN:%hu", _channels_data[i].min_us);
      RECEIVER_DEBUG("RCV:CH:RCV:MAX:%hu", _channels_data[i].max_us);
      RECEIVER_DEBUG("RCV:CH:RCV:FHZ:%hu", _channels_data[i].frequency_hz);
      RECEIVER_DEBUG("");
   }
}

void Receiver::send_channels() {
   RECEIVER_DEBUG("RCV:CH:SND");
   uint8_t size = _channel_count * sizeof(channel_t);

   header_t header;
   header.index = 0;
   header.total = 1;
   header.size = size;

   bitSet(header.flags, HEADER_FLAGS_BIT_TYPE_1);

   for(int i = 0; i < _channel_count; i++) {
      RECEIVER_DEBUG("RCV:CH:SND:ID:%hhu", _channels_data[i].id);
      RECEIVER_DEBUG("RCV:CH:SND:PIN:%hhu", _channels_data[i].pin);
      RECEIVER_DEBUG("RCV:CH:SND:MIN:%hu", _channels_data[i].min_us);
      RECEIVER_DEBUG("RCV:CH:SND:MAX:%hu", _channels_data[i].max_us);
      RECEIVER_DEBUG("RCV:CH:SND:FHZ:%hu", _channels_data[i].frequency_hz);
      RECEIVER_DEBUG("");
   }

   get_radio()->write(&header, &_channels_data);

   RECEIVER_DEBUG("RCV:CH:SND:%hhu:%hhu", _channel_count, size);
}

void Receiver::read_memory_channels() {
   _channel_count = _preferences.isKey("CH") ? _preferences.getBytesLength("CH") / sizeof(channel_t) : 0;

   if (_channel_count > 0) {
      RECEIVER_DEBUG("RCV:CH:MEM:%hhu", _channel_count);

      _preferences.getBytes("CH", &_channels_data, sizeof(channel_t) * _channel_count);
   }
}

#endif