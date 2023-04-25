#pragma once

#include <Arduino.h>

typedef struct {
   char name[8];

   uint16_t min = 65535;
   uint16_t median = 32762;
   uint16_t max = 0;
} control_t;