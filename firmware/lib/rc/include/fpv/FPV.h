#pragma once

#ifdef FPV_DEBUG_ENABLED
#define FPV_DEBUG(lvl, x,...)            DEBUG_OUTPUT(lvl, "FPV", x, ##__VA_ARGS__) //!< DEBUG
#else
#define FPV_DEBUG(lvl, x,...)                                           //!< DEBUG null
#endif

#include "radio\radio\Radio.h"

#ifdef RECEIVER

#include "esp_camera.h"

#define CAMERA_MODEL_AI_THINKER // Has PSRAM

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define LED_GPIO_NUM      33

#endif

#ifdef CONTROLLER
#ifndef MAX_IMAGE_SIZE
#define MAX_IMAGE_SIZE 12000
#endif
#endif

class FPV {
   public:
      FPV();
      void begin(Radio *radio);
      void update();

      #ifdef CONTROLLER
      bool available();
      uint16_t get_size();
      uint8_t *get_image();
      #endif
   protected:
   private:
      Radio *_radio;

      #ifdef RECEIVER
      void take_and_push_frame();
      void setup_camera();
      void setup_led_flash(uint8_t pin);
      #endif

      #ifdef CONTROLLER
      uint8_t _image_buffer[MAX_IMAGE_SIZE];
      bool _available;
      uint16_t _size;
      #endif
};