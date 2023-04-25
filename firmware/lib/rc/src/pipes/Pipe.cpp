#include "pipes\Pipe.h"

Pipe::Pipe() { }

void Pipe::begin(Radio *radio) {
   _radio = radio;
}

void Pipe::update() {

}

void Pipe::set_type_filter(uint8_t filter) {
   _filter = filter;
}

bool Pipe::available() {

}