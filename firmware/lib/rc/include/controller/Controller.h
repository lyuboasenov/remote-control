#pragma once

#ifdef CONTROLLER

#include "controls\Control.h"
#include "receiver\Channel.h"
#include "Radio.h"
#include "ControllerState.h"
#include <debug_print.h>
#include <Preferences.h>

#ifdef MENU
#include "controller\menu\ControllerMenu.h"
#endif

#ifdef __FPV__
#include "fpv\FPV.h"
#include <TJpg_Decoder.h>
#endif

#if DISPLAY == OLED
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#define display_t SSD1306AsciiWire

constexpr int OLED_SDA=21;
constexpr int OLED_SDC=22;

#define I2C_ADDRESS 0x3C
#elif DISPLAY == TFT
#include <TFT_eSPI.h>
#include "display\Colors.h"
#define display_t TFT_eSPI

#define textScale 2
#endif

#ifdef CONTROLLER_DEBUG_ENABLED
#define CONTROLLER_DEBUG(lvl, x,...)            DEBUG_OUTPUT(lvl, "CTRLR", x, ##__VA_ARGS__) //!< DEBUG
#else
#define CONTROLLER_DEBUG(lvl, x,...)                                           //!< DEBUG null
#endif

class Controller {
   public:
      Controller();
      virtual ~Controller();
      virtual void begin();
      void update();

      bool pair_with_receiver();

      void set_state(controller_state_t state);
      controller_state_t get_state();

      void request_channels();
      void read_memory_channels();
      void load_default_channels();
      uint8_t get_channel_count();
      channel_t *get_channels();
      void save_channels();
      void send_channel(uint8_t index);
      uint8_t get_channel_value(const uint8_t channel);

      void load_defaults_controls();
      void save_controls();

      #if DISPLAY == TFT
      static display_t _display;
      #elif DISPLAY == OLED
      static display_t _display;
      #endif

      #ifdef ESP32
      static void update_async(void * parameter);
      #endif
   protected:
      control_t _control_data[CHANNELS_COUNT];

      void send_channels();
      void write_memory_channels();

      bool set_controls(Control **controls, uint8_t count);
      void write_memory_controls();
      void read_memory_controls();

      virtual Radio * get_radio(uint8_t index = 0) { }
      virtual Endpoint * get_endpoint(uint8_t index = 0) { }
      void update_display();
   private:
      Preferences _preferences;

      Control *_controls[CHANNELS_COUNT];
      uint8_t _control_count;

      channel_t _channels[CHANNELS_COUNT];
      uint8_t _channel_count;
      bool _channels_read = false;
      uint8_t _buffer[CHANNELS_COUNT];

      controller_state_t _state;

      header_t _header;

      void read_radio();
      void read_controls();
      void send_commands();
      void dump_channels();

      #ifdef MENU
      ControllerMenu _menu;
      #endif

      #ifdef ESP32
      std::mutex _mutex;
      TaskHandle_t _update_task;
      #endif

      #ifdef __FPV__
      FPV _fpv;
      static bool fpv_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap);
      #endif
};

#endif