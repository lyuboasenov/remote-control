#pragma once

#include "Arduino.h"
#include "Endpoint.h"
#include "Header.h"
#include <debug_print.h>

#ifdef RADIO_DEBUG_ENABLED
#define RADIO_DEBUG(x, y,...)                DEBUG_OUTPUT(x, "RF", y, ##__VA_ARGS__) //!< DEBUG
#define RADIO_DUMP_BUFFER(l, b, s)           DUMP_BUFFER(l, "RF", b, s)              //!< DEBUG
#else
#define RADIO_DEBUG(x, y,...)                                              //!< DEBUG null
#define RADIO_DUMP_BUFFER(l, b, s)                                         //!< DEBUG null
#endif

#define RADIO_FLAGS_BIT_INIT 0
#define RADIO_FLAGS_BIT_HEADER_READ 1

#define RADIO_FLAGS_BIT_MASK_INIT         (bit(RADIO_FLAGS_BIT_INIT))
#define RADIO_FLAGS_BIT_MASK_HEADER_READ  (bit(RADIO_FLAGS_BIT_HEADER_READ))

class Radio {
   public:
      virtual bool begin(Endpoint *endpoint);

      virtual bool available();

      virtual bool read_header(header_t *header);
      virtual size_t read(void *buff);

      virtual size_t read(header_t *header, void *buff);

      bool write(header_t *header, const void *buff);
      virtual void flush();
   private:

   protected:
      virtual uint16_t get_max_packet_size();
      virtual bool write_internal(header_t *header, const void *buff);

      bool is_initialized();
      void set_initialized(const bool& value);

      bool is_header_read();
      void set_header_read(const bool& value);

      header_t _header;
      /// @brief
      /// [   |   |   |   |   |   | HEADER_READ | INIT ]
      uint8_t _flags;
};
