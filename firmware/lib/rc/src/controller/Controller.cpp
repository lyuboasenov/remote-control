#include "controller\Controller.h"

#ifdef CONTROLLER

#if DISPLAY == TFT || DISPLAY == OLED
display_t Controller::_display;
#endif

Controller::Controller()
#ifdef MENU
: _menu(this)
#endif
{ }
Controller::~Controller() { }

void Controller::begin() {
   _preferences.begin("CTRL", false);

   #if DISPLAY == OLED
   CTRL_MENU_DEBUG(verbosity_t::info, "BGN:DISP:OLED");
   Wire.begin();
   _display.begin(&Adafruit128x64, I2C_ADDRESS);
   _display.setFont(menuFont);

   _display.clear();
   _display.setCursor(0, 0);
   _display.print("Remote control");
   _display.setCursor(0, 2);
   _display.print("v");
   _display.print(TO_STRING(VERSION));
   delay(2000);
   _display.clear();
   #elif DISPLAY == TFT
   CTRL_MENU_DEBUG(verbosity_t::info, "BGN:DISP:TFT");
   SPI.begin();

   _display.init();
   // gfx.setFreeFont(&FreeSerif9pt7b);
   _display.setRotation(1);
   _display.setTextSize(textScale); //test scaling
   _display.setTextWrap(false);
   _display.fillScreen(Black);
   _display.setTextColor(Red, Black);
   _display.print("Remote control ");
   _display.print("v");
   _display.print(TO_STRING(VERSION));
   #endif

   read_memory_channels();
   read_memory_controls();

   #ifndef ESP32
   #ifdef MENU
   _menu.begin();
   #endif
   #endif

   #ifdef ESP32
   xTaskCreatePinnedToCore(
      Controller::update_async,        /* Function to implement the task */
      "update_async",                  /* Name of the task */
      10000,                           /* Stack size in words */
      this,                            /* Task input parameter */
      0,                               /* Priority of the task */
      &_update_task,                   /* Task handle. */
      1);                              /* Core where the task should run */
   #endif

   #ifdef __FPV__
   _fpv.begin(get_radio());
   TJpgDec.setJpgScale(1);
   // The decoder must be given the exact name of the rendering function above
   TJpgDec.setCallback(Controller::fpv_output);
   #endif
}

#ifdef __FPV__
bool Controller::fpv_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
   // Stop further decoding as image is running off bottom of screen
   if ( y >= _display.height() ) {
      FPV_DEBUG(verbosity_t::error, "DISP:W:%hu:%hu:%hu:%hu:%hu", x, y, w, h, _display.height());
      return 0;
   }
   FPV_DEBUG(verbosity_t::debug, "DISP:W:%hu:%hu:%hu:%hu:%hu", x, y, w, h, _display.height());
   _display.pushImage(x, y, w, h, bitmap);
   // Return 1 to decode next block
   return 1;
}
#endif

bool Controller::set_controls(Control **controls, uint8_t count) {

   for (uint8_t i = 0; i < min(count, (uint8_t) CHANNELS_COUNT); i++) {
      _controls[i] = controls[i];
   }

   _control_count = count;
   _channel_count = count;

   _header.index = 0;
   _header.total = 1;
   _header.size = count;

   CONTROLLER_DEBUG(verbosity_t::info, "SC:%hhu", count);
   bitClear(_header.flags, HEADER_FLAGS_BIT_TYPE_1);
   bitClear(_header.flags, HEADER_FLAGS_BIT_TYPE_2);
   bitClear(_header.flags, HEADER_FLAGS_BIT_TYPE_3);
   bitClear(_header.flags, HEADER_FLAGS_BIT_TYPE_4);

   if (count < CHANNELS_COUNT) {
      return true;
   } else {
      return false;
   }
}

void Controller::update() {
   read_radio();

   read_controls();

   #ifndef ESP32
   #ifdef MENU
   _menu.update();
   #endif
   #endif

   #ifndef ESP32
   #ifdef __FPV__
   _fpv.update();
   #endif
   #endif

   if (_state == controller_state_t::control) {
      for (uint8_t i = 0; i < _control_count; i++) {
         CONTROLLER_DEBUG(verbosity_t::debug, "CMD:W:%02X:%hu", i, _buffer[i]);
      }
      send_commands();

      #ifndef ESP32
      #ifdef DISPLAY
      update_display();
      #endif
      #endif
   }
}

void Controller::update_async(void * parameter) {
   Controller *_this = (Controller *) parameter;

   #ifdef MENU
   _this->_menu.begin();
   #endif

   for (;;) {
      #ifdef MENU
      _this->_menu.update();
      #endif

      #ifdef __FPV__
      _this->_fpv.update();
      #endif

      if (_this->_state == controller_state_t::control) {
         #ifdef DISPLAY
         _this->update_display();
         #endif
      }
      delay(10);
   }
}

void Controller::read_radio() {
   if (get_radio()->available()) {
      header_t header;
      get_radio()->read_header(&header);

      if (HEADER_FLAGS_TYPE_IS_CHANNELS(header.flags)) {
         _channel_count = min((uint8_t) (header.size / sizeof(channel_t)), (uint8_t) CHANNELS_COUNT);
         get_radio()->read(_channels);
         _channels_read = true;
         CONTROLLER_DEBUG(verbosity_t::info, "CH:R:%02X", _channel_count);

         dump_channels();
      }
   }
}

void Controller::dump_channels() {
   for(uint8_t i = 0; i < _channel_count; i++) {
      channel_t channel = _channels[i];

      CONTROLLER_DEBUG(verbosity_t::debug, "CH:ID:%hhu", channel.id);
      CONTROLLER_DEBUG(verbosity_t::debug, "CH:PIN:%hhu", channel.pin);
      CONTROLLER_DEBUG(verbosity_t::debug, "CH:EN:%s", channel.enabled ? "TRUE" : "FALSE");
      CONTROLLER_DEBUG(verbosity_t::debug, "CH:MIN:%hu", channel.min_us);
      CONTROLLER_DEBUG(verbosity_t::debug, "CH:MAX:%hu", channel.max_us);
      CONTROLLER_DEBUG(verbosity_t::debug, "CH:FHZ:%hu", channel.frequency_hz);
      CONTROLLER_DEBUG(verbosity_t::debug, "CH:REV:%s", channel.reversed ? "TRUE" : "FALSE");
      CONTROLLER_DEBUG(verbosity_t::debug, "CH:TYP:%s\n", channel.binary ? "BIN" : "SERV");
   }
}

void Controller::read_controls() {
   for (uint8_t i = 0; i < _control_count; i++) {
      _buffer[i] = _controls[i]->value();
   }
}

void Controller::send_commands() {
   get_radio()->write(&_header, &_buffer);
}

void Controller::request_channels() {
   CONTROLLER_DEBUG(verbosity_t::info, "CH:REQ");
   header_t header;

   header.index = 0;
   header.total = 1;
   header.size = 0;
   header.flags = 0;
   bitSet(header.flags, HEADER_FLAGS_BIT_TYPE_1);

   get_radio()->write(&header, &_buffer);
}

void Controller::load_default_channels() {
   CONTROLLER_DEBUG(verbosity_t::info, "CH:DEF");

   for (uint8_t i = 0; i < CHANNELS_COUNT; i++) {
      _channels[i] = channel_t();
   }

   dump_channels();
}

void Controller::read_memory_channels() {
   uint8_t channel_count = _preferences.isKey("CH") ? _preferences.getBytesLength("CH") / sizeof(channel_t) : 0;

   load_default_channels();

   if (channel_count > 0) {
      CONTROLLER_DEBUG(verbosity_t::info, "CH:MEM:%hhu", channel_count);

      _preferences.getBytes("CH", &_channels, sizeof(channel_t) * channel_count);

      dump_channels();
   } else {
      CONTROLLER_DEBUG(verbosity_t::error, "CH:MEM");
      load_default_channels();
   }
}

bool Controller::pair_with_receiver() {
   set_state(controller_state_t::pair);

   return true;
}

void Controller::set_state(controller_state_t state) {
   CONTROLLER_DEBUG(verbosity_t::info, "ST:%hhu", state);
   _state = state;
}

controller_state_t Controller::get_state() {
   return _state;
}

uint8_t Controller::get_channel_count() {
   return _channel_count;
}

channel_t *Controller::get_channels() {
   return _channels;
}

void Controller::save_channels() {
   write_memory_channels();

   send_channels();

   dump_channels();
}

void Controller::write_memory_channels() {
   CONTROLLER_DEBUG(verbosity_t::info, "CH:MEM:W");

   _preferences.remove("CH");
   _preferences.putBytes("CH", &_channels, sizeof(channel_t) * _channel_count);
}

void Controller::send_channels() {
   CONTROLLER_DEBUG(verbosity_t::info, "CH:SND");

   header_t header;

   header.index = 0;
   header.total = 1;
   header.size = _channel_count * sizeof(channel_t);
   header.flags = 0;
   bitSet(header.flags, HEADER_FLAGS_BIT_TYPE_1);

   get_radio()->write(&header, &_channels);
}

void Controller::send_channel(uint8_t index) {
   header_t header;

   header.index = 0;
   header.total = 1;
   header.size = sizeof(channel_t);
   header.flags = 0;
   bitSet(header.flags, HEADER_FLAGS_BIT_TYPE_1);

   get_radio()->write(&header, &get_channels()[index]);
}

uint8_t Controller::get_channel_value(const uint8_t channel) {
   return _buffer[channel];
}

uint16_t last_refreshed_millis = 0;
int16_t clean_height;
void Controller::update_display() {
#if DISPLAY == TFT

   #ifdef __FPV__
   if (_fpv.available()) {
      FPV_DEBUG(verbosity_t::debug, "DISP:W");
      TJpgDec.drawJpg(0, 0, _fpv.get_image(), _fpv.get_size());
   }
   #endif

   if (millis() - last_refreshed_millis > 40) {
      _display.setCursor(374, 0);
      _display.setTextColor(White);

      for (uint8_t i = 0; i < _control_count; i++) {
         _display.fillRect(420, i * 16, 80, 16, Black);
         _display.printf("%s % 3hhu\n", _controls[i]->get_name(), _buffer[i]);
         clean_height = _display.getCursorY();
         _display.setCursor(374, clean_height);
      }

      last_refreshed_millis = millis();
   }

#elif DISPLAY == OLED
#endif
}

void Controller::save_controls() {
   for (uint8_t i = 0; i < _control_count; i++) {
      _controls[i]->set_median_value();

      CONTROLLER_DEBUG(verbosity_t::info, "CTRLS:W:%hhu:[%hu:%hu:%hu]", i, _control_data[i].min, _control_data[i].median, _control_data[i].max);
   }
   write_memory_controls();
}

void Controller::load_defaults_controls() {
   CONTROLLER_DEBUG(verbosity_t::info, "CTRLS:R:D");

   for (uint8_t i = 0; i < _control_count; i++) {
      _control_data[i].min = 65535;
      _control_data[i].median = 32762;
      _control_data[i].max = 0;

      _controls[i]->set_median_value();
   }
}

void Controller::write_memory_controls() {
   CONTROLLER_DEBUG(verbosity_t::info, "CTRLS:MEM:W:%hu", sizeof(control_t) * _control_count);

   _preferences.remove("CTRL");
   _preferences.putBytes("CTRL", &_control_data, sizeof(control_t) * _control_count);
}

void Controller::read_memory_controls() {
   uint8_t control_count = _preferences.isKey("CTRL") ? _preferences.getBytesLength("CTRL") / sizeof(control_t) : 0;

   if (control_count == _control_count) {
      CONTROLLER_DEBUG(verbosity_t::info, "CTRLS:MEM:%hhu", control_count);

      _preferences.getBytes("CTRL", &_control_data, sizeof(channel_t) * control_count);

      for (uint8_t i = 0; i < _control_count; i++) {
         CONTROLLER_DEBUG(verbosity_t::debug, "CTRLS:MEM:%hhu:[%hu:%hu:%hu]", i, _control_data[i].min, _control_data[i].median, _control_data[i].max);
      }
   } else {
      CONTROLLER_DEBUG(verbosity_t::error, "CTRLS:MEM:%hhu", control_count);
   }
}

#endif