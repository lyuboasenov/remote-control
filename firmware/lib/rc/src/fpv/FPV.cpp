#include "fpv\FPV.h"
#include "esp32-hal.h"

FPV::FPV() { }

void FPV::begin(Radio *radio) {
   _radio = radio;

   #ifdef RECEIVER
   setup_camera();
   #endif
}

#ifdef RECEIVER

void FPV::setup_camera() {
   camera_config_t config;
   config.ledc_channel = LEDC_CHANNEL_0;
   config.ledc_timer = LEDC_TIMER_0;
   config.pin_d0 = Y2_GPIO_NUM;
   config.pin_d1 = Y3_GPIO_NUM;
   config.pin_d2 = Y4_GPIO_NUM;
   config.pin_d3 = Y5_GPIO_NUM;
   config.pin_d4 = Y6_GPIO_NUM;
   config.pin_d5 = Y7_GPIO_NUM;
   config.pin_d6 = Y8_GPIO_NUM;
   config.pin_d7 = Y9_GPIO_NUM;
   config.pin_xclk = XCLK_GPIO_NUM;
   config.pin_pclk = PCLK_GPIO_NUM;
   config.pin_vsync = VSYNC_GPIO_NUM;
   config.pin_href = HREF_GPIO_NUM;
   config.pin_sscb_sda = SIOD_GPIO_NUM;
   config.pin_sscb_scl = SIOC_GPIO_NUM;
   config.pin_pwdn = PWDN_GPIO_NUM;
   config.pin_reset = RESET_GPIO_NUM;
   config.xclk_freq_hz = 20000000;
   // config.frame_size = FRAMESIZE_UXGA;
   config.frame_size = FRAMESIZE_QVGA;
   config.pixel_format = PIXFORMAT_JPEG;           // for streaming
   // config.pixel_format = PIXFORMAT_RGB565;       // for face detection/recognition
   config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
   config.fb_location = CAMERA_FB_IN_PSRAM;
   // config.jpeg_quality = 12;
   config.jpeg_quality = 40;
   config.fb_count = 1;

  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(config.pixel_format == PIXFORMAT_JPEG){
      if(psramFound()){
         // config.jpeg_quality = 10;
         config.jpeg_quality = 40;
         config.fb_count = 2;
         config.grab_mode = CAMERA_GRAB_LATEST;
      } else {
         // Limit the frame size when PSRAM is not available
         config.frame_size = FRAMESIZE_SVGA;
         config.fb_location = CAMERA_FB_IN_DRAM;
      }
   } else {
      // Best option for face detection/recognition
      config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
      config.fb_count = 2;
#endif
   }

#if defined(CAMERA_MODEL_ESP_EYE)
   pinMode(13, INPUT_PULLUP);
   pinMode(14, INPUT_PULLUP);
#endif

   // camera init
   esp_err_t err = esp_camera_init(&config);
   if (err != ESP_OK) {
      FPV_DEBUG(verbosity_t::error, "INIT:0x%02X", err);
      return;
   }

   sensor_t * s = esp_camera_sensor_get();
   // initial sensors are flipped vertically and colors are a bit saturated
   if (s->id.PID == OV3660_PID) {
      s->set_vflip(s, 1); // flip it back
      s->set_brightness(s, 1); // up the brightness just a bit
      s->set_saturation(s, -2); // lower the saturation
   }
   // drop down frame size for higher initial frame rate
   if(config.pixel_format == PIXFORMAT_JPEG){
      s->set_framesize(s, FRAMESIZE_QVGA);
   }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
   s->set_vflip(s, 1);
   s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
   s->set_vflip(s, 1);
#endif

// Setup LED FLash if LED pin is defined in camera_pins.h
#if defined(LED_GPIO_NUM)
   setup_led_flash(LED_GPIO_NUM);
#endif
}

void FPV::setup_led_flash(uint8_t pin) {
   #if CONFIG_LED_ILLUMINATOR_ENABLED
   ledcSetup(LED_LEDC_CHANNEL, 5000, 8);
   ledcAttachPin(pin, LED_LEDC_CHANNEL);
   #else
   FPV_DEBUG(verbosity_t::error, "LED:DISB -> CONFIG_LED_ILLUMINATOR_ENABLED = 0");
   #endif
}

void FPV::take_and_push_frame() {
   camera_fb_t* fb = esp_camera_fb_get();
   if(!fb || fb->format != PIXFORMAT_JPEG){
      FPV_DEBUG(verbosity_t::error, "CAP:FAIL");
   }else{
      header_t header;
      header.index = 0;
      header.total = 1;
      header.flags = 0;
      bitSet(header.flags, HEADER_FLAGS_BIT_TYPE_2);
      header.size = fb->len;

      FPV_DEBUG(verbosity_t::trace, "CAP[%hu]", fb->len);

      _radio->write(&header, fb->buf);
   }
   esp_camera_fb_return(fb);
}

#endif

void FPV::update() {
   #ifdef RECEIVER
   take_and_push_frame();
   #endif
   #ifdef CONTROLLER
   header_t header;
   while(_radio->available()) {
      if (_radio->read_header(&header) &&
         HEADER_FLAGS_TYPE_IS_IMG(header.flags)) {
         size_t read = _radio->read(_image_buffer);
         if (_available != header.index + 1 == header.total) {
            _available = !_available;
            _size = read;
            FPV_DEBUG(verbosity_t::trace, "AV:%s:%hu", _available ? "TRUE" : "FALSE", _size);
         }
      }
   }
   #endif
}

#ifdef CONTROLLER
bool FPV::available() {
   return _available;
}

uint16_t FPV::get_size() {
   return _size;
}

uint8_t *FPV::get_image() {
   _available = false;
   FPV_DEBUG(verbosity_t::trace, "AV:FALSE");
   return _image_buffer;
}
#endif