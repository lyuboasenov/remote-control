#pragma once

#include "radio\Config.h"

#include "radio\radio\Header.h"
#include "radio\radio\Endpoint.h"
#include "radio\radio\Radio.h"

#ifdef _INCLUDE_BT
#include "radio\radio\bt\BTEndpoint.h"
#include "radio\radio\bt\BTRadio.h"
#endif

#ifdef _INCLUDE_ESP_NOW
#include "radio\radio\esp-now\EspNowEndpoint.h"
#include "radio\radio\esp-now\EspNowRadio.h"
#endif

#ifdef _INCLUDE_RF24
#include "radio\radio\nrf24\RF24Endpoint.h"
#include "radio\radio\nrf24\RF24Radio.h"
#endif

#ifdef _INCLUDE_WI_FI
#include "radio\radio\wifi\WiFiEndpoint.h"
#include "radio\radio\wifi\WiFiRadio.h"
#endif
