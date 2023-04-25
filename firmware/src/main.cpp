#include <Arduino.h>

#if defined(CONTROLLER) && defined(RECEIVER)
#error Both CONTROLLER and RECEIVER are defined
#endif

#ifdef CONTROLLER
#include "BasicController.h"

BasicController controller;
#elif defined(RECEIVER)
#include "BasicReceiver.h"

BasicReceiver receiver;
#endif

void setup() {
#ifdef SERIAL_ENABLED
   Serial.begin(115200);
#endif

#ifdef CONTROLLER
   controller.begin();
#elif defined(RECEIVER)
   receiver.begin();
#endif
}

void loop() {
#ifdef CONTROLLER
   controller.update();
#elif defined(RECEIVER)
   receiver.update();
#endif
}
