#include "AnalogSensor.h"

#include <Arduino.h>

AnalogSensor::AnalogSensor(uint32_t pin) {
  _pin = pin;
}

void AnalogSensor::begin() {
  pinMode(_pin, INPUT);
}

bool AnalogSensor::readValues() {
  _value = analogRead(_pin);
  return true;
}

bool AnalogSensor::printAddress(Stream *serial) {
  serial->print(_pin);
  return true;
}

bool AnalogSensor::printValues(Stream *serial) {
  serial->print(_value);
  return true;
}
