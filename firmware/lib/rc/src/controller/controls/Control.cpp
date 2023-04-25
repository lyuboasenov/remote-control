#include "controller\controls\Control.h"

#ifdef CONTROLLER

Control::Control(const char * name, ValueReader *value_reader, control_t * data, bool auto_centered) :
   _name(name), _value_reader(value_reader), _data(data), _auto_centered(auto_centered) { }

void Control::begin() {
   _value_reader->begin();

   if (_auto_centered) {
      set_median_value();
   }
}

const char *Control::get_name() {
   return _name;
}

uint8_t Control::value() {
   uint16_t value = _value_reader->read_value();

   _data->min = value < _data->min ? value : _data->min;
   _data->max = value > _data->max ? value : _data->max;
   if (_data->min == _data->max) {
      if (_data->max == 65535) {
         _data->min -= 1;
      } else {
         _data->max += 1;
      }
   }

   CONTROL_DEBUG(verbosity_t::debug, "%s[%hu:%hu:%hu:%hu]", _name, _data->min, value, _data->median, _data->max);

   if (_auto_centered) {
      if (value < _data->median) {
         return map(value, _data->min, _data->median, 0, 127);
      } else {
         return map(value, _data->median, _data->max, 128, 255);
      }
   } else {
      return map(value, _data->min, _data->max, 0, 255);
   }
}

void Control::set_median_value() {
   _data->median = _value_reader->read_value();
}

#endif