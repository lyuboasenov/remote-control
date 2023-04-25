#pragma once

#if defined(CONTROLLER) && defined(MENU)

#include <CommonMacros.h>

#ifdef CONTROLLER_MENU_DEBUG_ENABLED
#include <debug_print.h>
#define CTRL_MENU_DEBUG(lvl, x,...)            DEBUG_OUTPUT(lvl, "MENU", x, ##__VA_ARGS__) //!< DEBUG
#else
#define CTRL_MENU_DEBUG(lvl, x,...)                                           //!< DEBUG null
#endif

#define ENC_BUTTONINTERVAL 1000

#include <menu.h>                            //menu macros and objects
#include <ClickEncoder.h>                    // Using this library: https://github.com/soligen2010/encoder.git
#include <menuIO/clickEncoderIn.h>
#include <menuIO/keyIn.h>                    //keyboard driver and fake stream (for the encoder button)
#include <menuIO/chainStream.h>              // concatenate multiple input streams (this allows adding a button to the encoder)

#if DISPLAY == OLED
#include <menuIO/SSD1306AsciiOut.h>
#elif DISPLAY == TFT
#include <menuIO/TFT_eSPIOut.h>
#include "TFT_eSPIOutProxy.h"
#include <menuIO/esp8266Out.h>               //must include this even if not doing web output...
#include "controller\display\Colors.h"
#endif

// Encoder /////////////////////////////////////
#define encA 5
#define encB 16
//this encoder has a button here
#define encBtn 17
#define encSteps 2
#define btnSensitivity 1

#define ENC_SENSITIVITY 8
#define MAX_DEPTH 4

using namespace Menu;

#if DISPLAY == OLED

#define menuFont X11fixed7x14
#define fontW 7
#define fontH 14

#define DISPLAY_W 128
#define DISPLAY_H 64

#define textScale 1

#define PADDING_W 0
#define PADDING_H 0

#elif DISPLAY == TFT

#define DISPLAY_W 480
#define DISPLAY_H 320
#define fontW 6
#define fontH 9

#define PADDING_W 1
#define PADDING_H 1

#endif

class Controller;

class ControllerMenu {
   public:
      static ControllerMenu *_this;
      Controller *_controller;

      ControllerMenu(Controller *controller);
      void begin();
      void update();
   protected:
   private:
};

#endif