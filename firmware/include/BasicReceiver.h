#pragma once

#ifdef RECEIVER

#define CH_0 0
#define CH_1 1

#define CH_0_PIN 14
#define CH_1_PIN 15

#include <Arduino.h>

#include <RC.h>

class BasicReceiver : public Receiver {
   public:
      BasicReceiver();
      void begin();
   protected:
      Radio * get_radio();
      Endpoint * get_endpoint();
   private:
      Radio *_radio;
      Endpoint *_endpoint;
};

#endif