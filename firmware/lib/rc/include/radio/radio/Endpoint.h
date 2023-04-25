#pragma once

class Endpoint {
   public:
      virtual ~Endpoint() { }
      // HACK: for dynamic_cast to work
      virtual uint8_t type();
   protected:
   private:
};
