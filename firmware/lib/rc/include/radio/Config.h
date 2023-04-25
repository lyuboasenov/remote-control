#pragma once

#ifdef RF24_RADIO
#define _INCLUDE_RF24
#endif

#ifdef BT_RADIO
#define _INCLUDE_BT
#endif

#ifdef ESP_NOW_RADIO
#define _INCLUDE_ESP_NOW
#endif

#ifdef WI_FI_RADIO
#define _INCLUDE_WI_FI
#endif