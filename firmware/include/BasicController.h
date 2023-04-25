#pragma once

#ifdef CONTROLLER

#include <RC.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>

#define CTRL_LEFT_JOY_TOP_X_PIN 13
#define CTRL_LEFT_JOY_TOP_Y_PIN 12

#define CTRL_LEFT_JOY_BOTTOM_X_PIN 14
#define CTRL_LEFT_JOY_BOTTOM_Y_PIN 27

#define CTRL_LEFT_ROTARY_POT_PIN 26
#define CTRL_LEFT_SLIDER_POT_PIN 25

#define CTRL_RIGHT_JOY_TOP_X_PIN 33
#define CTRL_RIGHT_JOY_TOP_Y_PIN 32

#define CTRL_RIGHT_JOY_BOTTOM_X_PIN 35
#define CTRL_RIGHT_JOY_BOTTOM_Y_PIN 34

#define CTRL_RIGHT_ROTARY_POT_PIN 39
#define CTRL_RIGHT_SLIDER_POT_PIN 36




class BasicController : public Controller {
   public:
      BasicController();
      void begin();
   protected:
      Radio * get_radio(uint8_t index = 0) override;
      Endpoint * get_endpoint(uint8_t index = 0) override;
   private:
      Radio **_radios;
      Endpoint **_endpoints;
      Adafruit_MCP23X17 _mcp;
};

#endif