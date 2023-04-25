#include "controller\menu\ControllerMenu.h"
#include "controller\Controller.h"

#if defined(CONTROLLER) && defined(MENU)

ControllerMenu *ControllerMenu::_this;

void IRAM_ATTR onTimer(); // Start the timer to read the clickEncoder every 1 ms

ClickEncoder clickEncoder = ClickEncoder(encA, encB, encBtn, encSteps);
ClickEncoderStream encStream(clickEncoder, btnSensitivity);

//a keyboard with only one key as the encoder button
keyMap encBtn_map[]={{ -encBtn, defaultNavCodes[enterCmd].ch }};        //negative pin numbers use internal pull-up, this is on when low
keyIn<1> encButton(encBtn_map);                                         //1 is the number of keys

//input from the encoder + encoder button + serial
menuIn* inputsList[]={ &encStream, &encButton };
chainStream<2> in(inputsList);                                          //3 is the number of inputs

result read_receiver_channels(eventMask e, navNode& _nav, prompt& item) {
   ControllerMenu::_this->_controller->request_channels();

   return proceed;
}

result read_memory_channels(eventMask e, navNode& _nav, prompt& item) {
   ControllerMenu::_this->_controller->read_memory_channels();

   return proceed;
}

result load_default_channels(eventMask e, navNode& _nav, prompt& item) {
   ControllerMenu::_this->_controller->load_default_channels();

   return proceed;
}

result pair_with_receiver(eventMask e, navNode& _nav, prompt& item) {
   if (ControllerMenu::_this->_controller->pair_with_receiver()) {

   } else {

   }
   return proceed;
}

result initialize_controls(eventMask e, navNode& _nav, prompt& item) {
   ControllerMenu::_this->_controller->set_state(controller_state_t::initialize);

   return proceed;
}

result initialize_controls_defaults(eventMask e, navNode& _nav, prompt& item) {
   ControllerMenu::_this->_controller->set_state(controller_state_t::menu);
   ControllerMenu::_this->_controller->load_defaults_controls();

   return quit;
}

result initialize_controls_save(eventMask e, navNode& _nav, prompt& item) {
   ControllerMenu::_this->_controller->set_state(controller_state_t::menu);
   ControllerMenu::_this->_controller->save_controls();

   return quit;
}

result initialize_controls_exit(eventMask e, navNode& _nav, prompt& item) {
   ControllerMenu::_this->_controller->set_state(controller_state_t::menu);

   return quit;
}

result controller_state_menu(eventMask e, navNode& _nav, prompt& item) {
   ControllerMenu::_this->_controller->set_state(controller_state_t::menu);

   return proceed;
}

uint8_t channel_count;
uint8_t selected_channel;
uint8_t selected_pin;
uint16_t selected_min_us;
uint16_t selected_max_us;
uint16_t selected_frequency_hz;
bool selected_reversed = false;
bool selected_enabled = false;
bool selected_binary = false;

result selected_channel_changed(eventMask e, navNode& _nav, prompt& item);

result save_channels(eventMask e, navNode& _nav, prompt& item) {
   ControllerMenu::_this->_controller->set_state(controller_state_t::menu);

   for(uint8_t i = 0; i < ControllerMenu::_this->_controller->get_channel_count(); i++) {
      channel_t channel = ControllerMenu::_this->_controller->get_channels()[i];

      CTRL_MENU_DEBUG(verbosity_t::debug, "SAVE_CH:ID:%hhu", channel.id);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SAVE_CH:EN:%s", channel.enabled ? "TRUE" : "FALSE");
      CTRL_MENU_DEBUG(verbosity_t::debug, "SAVE_CH:PIN:%hhu", channel.pin);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SAVE_CH:MIN:%hu", channel.min_us);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SAVE_CH:MAX:%hu", channel.max_us);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SAVE_CH:FHZ:%hu", channel.frequency_hz);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SAVE_CH:REV:%s", channel.reversed ? "TRUE" : "FALSE");
      CTRL_MENU_DEBUG(verbosity_t::debug, "SAVE_CH:BIN:%s\n", channel.binary ? "TRUE" : "FALSE");
   }

   ControllerMenu::_this->_controller->save_channels();

   return quit;
}

result update_selected_channel(eventMask e, navNode& _nav, prompt& item) {
   if (selected_channel < ControllerMenu::_this->_controller->get_channel_count()) {
      channel_t *channel = &ControllerMenu::_this->_controller->get_channels()[selected_channel];
      channel->id = selected_pin;
      channel->pin = selected_pin;
      channel->min_us = selected_min_us;
      channel->max_us = selected_max_us;
      channel->frequency_hz = selected_frequency_hz;
      channel->reversed = selected_reversed;
      channel->enabled = selected_enabled;
      channel->binary = selected_binary;

      CTRL_MENU_DEBUG(verbosity_t::debug, "UPD_CH:ID:%hhu", channel->id);
      CTRL_MENU_DEBUG(verbosity_t::debug, "UPD_CH:EN:%s", channel->enabled ? "TRUE" : "FALSE");
      CTRL_MENU_DEBUG(verbosity_t::debug, "UPD_CH:PIN:%hhu", channel->pin);
      CTRL_MENU_DEBUG(verbosity_t::debug, "UPD_CH:MIN:%hu", channel->min_us);
      CTRL_MENU_DEBUG(verbosity_t::debug, "UPD_CH:MAX:%hu", channel->max_us);
      CTRL_MENU_DEBUG(verbosity_t::debug, "UPD_CH:FHZ:%hu", channel->frequency_hz);
      CTRL_MENU_DEBUG(verbosity_t::debug, "UPD_CH:REV:%s", channel->reversed ? "TRUE" : "FALSE");
      CTRL_MENU_DEBUG(verbosity_t::debug, "UPD_CH:BIN:%s\n", channel->binary ? "TRUE" : "FALSE");
   }

   return proceed;
}

result update_and_send_selected_channel(eventMask e, navNode& _nav, prompt& item) {
   update_selected_channel(e, _nav, item);
   if (selected_channel < ControllerMenu::_this->_controller->get_channel_count()) {
      channel_t *channel = &ControllerMenu::_this->_controller->get_channels()[selected_channel];

      CTRL_MENU_DEBUG(verbosity_t::debug, "UPD_SEND_CH:ID:%hhu", channel->id);
      ControllerMenu::_this->_controller->send_channel(selected_channel);
   }

   return proceed;
}

result update_min_and_send_selected_channel(eventMask e, navNode& _nav, prompt& item) {
   if (selected_channel < ControllerMenu::_this->_controller->get_channel_count()) {
      channel_t *channel = &ControllerMenu::_this->_controller->get_channels()[selected_channel];
      channel->value = 0;
   }
   return update_and_send_selected_channel(e, _nav, item);
}

result update_max_and_send_selected_channel(eventMask e, navNode& _nav, prompt& item) {
   if (selected_channel < ControllerMenu::_this->_controller->get_channel_count()) {
      channel_t *channel = &ControllerMenu::_this->_controller->get_channels()[selected_channel];
      channel->value = 255;
   }
   return update_and_send_selected_channel(e, _nav, item);
}

result update_freq_and_send_selected_channel(eventMask e, navNode& _nav, prompt& item) {
   if (selected_channel < ControllerMenu::_this->_controller->get_channel_count()) {
      channel_t *channel = &ControllerMenu::_this->_controller->get_channels()[selected_channel];
      channel->value = 127;
   }
   return update_and_send_selected_channel(e, _nav, item);
}

prompt* typeData[]={
  new menuValue<bool>("Bin", true),
  new menuValue<bool>("Serv", false)
};
toggle<bool>& typeMenu
  = *new toggle<bool>("Type:", selected_binary, sizeof(typeData)/sizeof(prompt*), typeData, update_selected_channel, exitEvent);

prompt* enabledData[]={
  new menuValue<bool>("True", true),
  new menuValue<bool>("False", false)
};
toggle<bool>& enabledMenu
  = *new toggle<bool>("Enabled:", selected_enabled, sizeof(enabledData)/sizeof(prompt*), enabledData, update_selected_channel, exitEvent);

prompt* reversedData[]={
  new menuValue<bool>("On", true),
  new menuValue<bool>("Off", false)
};
toggle<bool>& reversedMenu
  = *new toggle<bool>("Rev:", selected_reversed, sizeof(reversedData)/sizeof(prompt*), reversedData, update_selected_channel, exitEvent);


//the submenu -------------------------------------
prompt* loadChannelsSubData[]={
   new Exit("<< Back"),

   new prompt("From memory", read_memory_channels, enterEvent),
   new prompt("From receiver", read_receiver_channels, enterEvent),
   new prompt("Defaults", load_default_channels, enterEvent)
};
menuNode& loadChannelsSubMenu = *new menuNode("Load channels", sizeof(loadChannelsSubData) / sizeof(prompt*), loadChannelsSubData);

//the submenu -------------------------------------
prompt* channelSetupSubData[]={
   new prompt("<< Back", initialize_controls_exit, enterEvent),
   new prompt(">> Save", save_channels, enterEvent),

   // new prompt("Read channels", read_receiver_channels, enterEvent),
   new menuField<typeof(channel_count)>(channel_count, "Total channels", "", 0, 255, 0, 0),
   &loadChannelsSubMenu,
   new menuField<typeof(selected_channel)>(selected_channel, "Channel", "", 0, 255, 1, 0, selected_channel_changed, exitEvent, wrapStyle),
   &enabledMenu,
   new menuField<typeof(selected_pin)>(selected_pin, "Pin", "", 0, 50, 1, 0, update_and_send_selected_channel, (eventMask)(enterEvent | updateEvent | exitEvent), wrapStyle),
   new menuField<typeof(selected_min_us)>(selected_min_us, "Min", "us", 0, 5000, 50, 1, update_min_and_send_selected_channel, (eventMask)(enterEvent | updateEvent | exitEvent), wrapStyle),
   new menuField<typeof(selected_max_us)>(selected_max_us, "Max", "us", 0, 5000, 50, 1, update_max_and_send_selected_channel, (eventMask)(enterEvent | updateEvent | exitEvent), wrapStyle),
   new menuField<typeof(selected_frequency_hz)>(selected_frequency_hz, "Freq", "Hz", 0, 1000, 10, 1, update_freq_and_send_selected_channel, (eventMask)(enterEvent | updateEvent | exitEvent), wrapStyle),
   &reversedMenu,
   &typeMenu
};

result enter_channel_setup(eventMask e, navNode& _nav, prompt& item) {
   channel_count = ControllerMenu::_this->_controller->get_channel_count();
   channelSetupSubData[0]->dirty = true;

   if (selected_channel < channel_count) {
      channel_t *channel = &ControllerMenu::_this->_controller->get_channels()[selected_channel];

      selected_pin = channel->pin;
      selected_min_us = channel->min_us;
      selected_max_us = channel->max_us;
      selected_frequency_hz = channel->frequency_hz;
      selected_reversed = channel->reversed;
      selected_enabled = channel->enabled;
      selected_binary = channel->binary;

      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:ID:%hhu", channel->id);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:EN:%s", channel->enabled ? "TRUE" : "FALSE");
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:PIN:%hhu", channel->pin);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:MIN:%hu", channel->min_us);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:MAX:%hu", channel->max_us);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:FHZ:%hu", channel->frequency_hz);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:REV:%s", channel->reversed ? "TRUE" : "FALSE");
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:BIN:%s", channel->binary ? "TRUE" : "FALSE");

      channelSetupSubData[3]->dirty = true;
      channelSetupSubData[4]->dirty = true;
      channelSetupSubData[5]->dirty = true;
      channelSetupSubData[6]->dirty = true;
      channelSetupSubData[7]->dirty = true;
      channelSetupSubData[8]->dirty = true;
      channelSetupSubData[9]->dirty = true;
   }

   return proceed;
}

menuNode& channelSetupSubMenu = *new menuNode("Channels setup", sizeof(channelSetupSubData) / sizeof(prompt*), channelSetupSubData, enter_channel_setup, enterEvent);

result selected_channel_changed(eventMask e, navNode& _nav, prompt& item) {

   CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:%hhu", selected_channel);
   if (selected_channel < ControllerMenu::_this->_controller->get_channel_count()) {
      channel_t channel = ControllerMenu::_this->_controller->get_channels()[selected_channel];

      selected_pin = channel.pin;
      selected_min_us = channel.min_us;
      selected_max_us = channel.max_us;
      selected_frequency_hz = channel.frequency_hz;
      selected_reversed = channel.reversed;
      selected_enabled = channel.enabled;
      selected_binary = channel.binary;

      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:ID:%hhu", channel.id);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:EN:%s", channel.enabled ? "TRUE" : "FALSE");
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:PIN:%hhu", channel.pin);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:MIN:%hu", channel.min_us);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:MAX:%hu", channel.max_us);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:FHZ:%hu", channel.frequency_hz);
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:REV:%s", channel.reversed ? "TRUE" : "FALSE");
      CTRL_MENU_DEBUG(verbosity_t::debug, "SEL_CH:BIN:%s", channel.binary ? "TRUE" : "FALSE");

      channelSetupSubData[3]->dirty = true;
      channelSetupSubData[4]->dirty = true;
      channelSetupSubData[5]->dirty = true;
      channelSetupSubData[6]->dirty = true;
      channelSetupSubData[7]->dirty = true;
      channelSetupSubData[8]->dirty = true;
      channelSetupSubData[9]->dirty = true;
   }

   return proceed;
}

prompt* initControlsSubData[]={
  new prompt("<< Back", initialize_controls_exit, enterEvent),
  new prompt(">> Save", initialize_controls_save, enterEvent),
  new prompt("Defaults", initialize_controls_defaults, enterEvent)
};
menuNode& initControlsSubMenu = *new menuNode("Init controls", sizeof(initControlsSubData) / sizeof(prompt*), initControlsSubData, initialize_controls, enterEvent);

menuNode& pairSubMenu = *new menuNode("Pair", sizeof(initControlsSubData) / sizeof(prompt*), initControlsSubData, pair_with_receiver, enterEvent);

prompt* mainData[]={
  new Exit("<< Exit"),
  &channelSetupSubMenu,
  &pairSubMenu,
  &initControlsSubMenu
};

menuNode& mainMenu=*new menuNode("Main menu", sizeof(mainData) / sizeof(prompt*), mainData, controller_state_menu, anyEvent);

#if DISPLAY == TFT

// define menu colors --------------------------------------------------------
//each color is in the format:
//  {{disabled normal,disabled selected},{enabled normal,enabled selected, enabled editing}}
const colorDef<uint16_t> colors[6] MEMMODE={
   {{(uint16_t)Black,(uint16_t)Black},     {(uint16_t)Black,      (uint16_t)Blue,        (uint16_t)Blue}},     //bgColor
   {{(uint16_t)Gray, (uint16_t)Gray},      {(uint16_t)White,      (uint16_t)White,       (uint16_t)White}},    //fgColor
   {{(uint16_t)White,(uint16_t)Black},     {(uint16_t)LighterRed, (uint16_t)LighterRed,  (uint16_t)Red}},      //valColor
   {{(uint16_t)White,(uint16_t)Black},     {(uint16_t)White,      (uint16_t)Yellow,      (uint16_t)Yellow}},   //unitColor
   {{(uint16_t)White,(uint16_t)Gray},      {(uint16_t)Black,      (uint16_t)LighterBlue, (uint16_t)White}},    //cursorColor
   {{(uint16_t)White,(uint16_t)Yellow},    {(uint16_t)Black,      (uint16_t)Yellow,      (uint16_t)Black}},    //titleColor
};

#endif // MENU_TFT
//describing a menu output device without macros
//define at least one panel for menu output

#if DISPLAY == OLED || DISPLAY == TFT
const panel panels[] MEMMODE = {{ PADDING_W, PADDING_H, DISPLAY_W / (fontW * textScale) - PADDING_W, DISPLAY_H / (fontH * textScale) - PADDING_H }};
navNode* nodes[sizeof(panels) / sizeof(panel)];                                     //navNodes to store navigation status
idx_t tops[MAX_DEPTH] = {0};                                                        //store cursor positions for each level
panelsList pList(panels, nodes, 1);                                                 //a list of panels and nodes
#endif

#if DISPLAY == OLED
SSD1306AsciiOut out_disp(Controller::_display, tops, pList, 8, 1 + ((fontH - 1)>>3) );      //_display output device menu driver
#elif DISPLAY == TFT
// eSPI
TFT_eSPIOutProxy out_disp(Controller::_display, colors, tops, pList, fontW * textScale, (fontH + 1) * textScale );
#endif

menuOut* constMEM outputs[] MEMMODE = {
   #if DISPLAY == OLED || DISPLAY == TFT
   &out_disp
   #endif
   };                  //list of output devices
outputsList out(outputs, sizeof(outputs) / sizeof(menuOut*));                       //outputs list

//navigation control -------------------------------------
navNode path[MAX_DEPTH];
navRoot _nav(mainMenu, path, MAX_DEPTH, in, out);

//when menu is suspended
result idle(menuOut &o, idleEvent e) {
   if (e == idling) {
      ControllerMenu::_this->_controller->set_state(controller_state_t::control);
   } else if (e == idleEnd) {
      ControllerMenu::_this->_controller->set_state(controller_state_t::menu);
   }

   o.clear();
   #if DISPLAY == OLED
   if (&o == &out_disp) {
   #elif DISPLAY == TFT
   if (&o == &out_disp) {
   #endif

   #if DISPLAY == OLED || DISPLAY == TFT
      if (e == idling) {
         o.println("Press menu button");
         o.println("  to enter the menu");
      }
   } else {
   #else
   if (true) {
   #endif
      switch (e) {
         case idleStart: o.println("Starting to control!"); break;
         case idling: o.println("controlling..."); break;
         case idleEnd: o.println("Entering menu."); break;
      }
   }

   return proceed;
}

ControllerMenu::ControllerMenu(Controller *controller) : _controller(controller) {
   _this = this;
}

hw_timer_t *timer = NULL;

void ControllerMenu::begin() {
   pinMode(encBtn,INPUT_PULLUP);

   clickEncoder.setAccelerationEnabled(true);
   clickEncoder.setDoubleClickEnabled(false); // Disable double clicks makes the response faster.  See: https://github.com/soligen2010/encoder/issues/6

   // // ESP32 timer
   timer = timerBegin(0, 240, true);
   timerAttachInterrupt(timer, &onTimer, true);
   timerAlarmWrite(timer, 1000, true);
   timerAlarmEnable(timer);

   _nav.idleTask = idle; //point a function to be used when menu is suspended
}

void ControllerMenu::update() {
   _nav.poll();
}

// ESP32 timer
void IRAM_ATTR onTimer() {
  clickEncoder.service();
}

#endif