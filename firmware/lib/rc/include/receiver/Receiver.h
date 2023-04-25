#pragma once

#ifdef RECEIVER

#ifndef RECEIVER_BUFFER_SIZE
#define RECEIVER_BUFFER_SIZE 249
#endif

#include "Radio.h"
#include "receiver\Channel.h"
#include <Preferences.h>
#include "debug_print.h"
#ifdef __FPV__
#include "fpv\FPV.h"
#endif

#ifdef RECEIVER_DEBUG_ENABLED
#define RECEIVER_DEBUG(lvl, x,...)            DEBUG_OUTPUT(lvl, "RCVR", x, ##__VA_ARGS__) //!< DEBUG
#else
#define RECEIVER_DEBUG(lvl, x,...)                                           //!< DEBUG null
#endif

class Receiver {
   public:
      Receiver();
      virtual ~Receiver() { }
      virtual void begin();
      void update();
   protected:
      bool set_channels(Channel *channel, uint8_t count);
      virtual Radio * get_radio() {}
      virtual Endpoint * get_endpoint() {}
   private:
      Preferences _preferences;
      Channel _channels[CHANNELS_COUNT];
      channel_t _channels_data[CHANNELS_COUNT];

      uint8_t _channel_count;
      header_t _header;
      uint8_t _buffer[RECEIVER_BUFFER_SIZE];

      #ifdef __FPV__
      FPV _fpv;
      #endif

      void send_channels();
      void read_channels();
      void read_memory_channels();
};

#endif