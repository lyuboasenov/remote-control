#include "radio\radio\Radio.h"


size_t Radio::read(header_t *header, void *buff) {
   if (read_header(header)) {
      return read(buff);
   } else {
      return 0;
   }
}

bool Radio::is_initialized() {
   return _flags & RADIO_FLAGS_BIT_MASK_INIT;
}

void Radio::set_initialized(const bool& value) {
   value ? bitSet(_flags, RADIO_FLAGS_BIT_INIT) : bitClear(_flags, RADIO_FLAGS_BIT_INIT);
}

bool Radio::is_header_read() {
   return _flags & RADIO_FLAGS_BIT_MASK_HEADER_READ;
}

void Radio::set_header_read(const bool& value) {
   value ? bitSet(_flags, RADIO_FLAGS_BIT_HEADER_READ) : bitClear(_flags, RADIO_FLAGS_BIT_HEADER_READ);
}

bool Radio::write(header_t *header, const void *buff) {
   uint16_t packet_size = get_max_packet_size();
   uint16_t effective_packet_size = packet_size - sizeof(header_t);

   if (header->size < effective_packet_size) {
      return write_internal(header, buff);
   } else {
      uint8_t total = header->size / effective_packet_size;
      if (header->size % effective_packet_size) {
         total++;
      }

      header_t part_header;
      part_header.flags = header->flags;
      part_header.size = effective_packet_size;
      part_header.total = total;

      for(uint8_t i = 0; i < total; i++) {
         part_header.index = i;

         if (i == total - 1) {
            part_header.size = header->size % effective_packet_size;
         }

         if (!write_internal(&part_header, buff + i)) {
            return false;
         }
      }

      return true;
   }
}