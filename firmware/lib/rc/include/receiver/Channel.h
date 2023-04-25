#pragma once

#include <Arduino.h>

typedef struct {
   uint8_t id = 0;
   uint8_t pin = 15;
   uint8_t value = 111;   // this seems to be the default value for the joystick

   bool enabled = false;
   bool reversed = false;
   bool binary = false;

   uint16_t min_us = 1000;
   uint16_t max_us = 1200;
   uint16_t frequency_hz = 50;
} channel_t;

#ifdef RECEIVER

#ifdef ESP32
#include <ESP32Servo.h>
#elif defined(AVR)
#include <Servo.h>
#endif

class Channel {
   public:
      Channel();
      void begin();
      void end();

      void set_value(uint8_t value);
      void set_min_us(uint16_t min);
      void set_max_us(uint16_t max);
      void set_frequency_hz(uint16_t frequency);

      uint8_t get_value();
      uint16_t get_min_us();
      uint16_t get_max_us();
      uint16_t get_frequency_hz();
      channel_t *get_channel_t();
      void set_channel_t(channel_t * data);
   private:
      Servo _servo;
      channel_t *_channel_data;
};

#endif