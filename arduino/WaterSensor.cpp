#include "WaterSensor.h"

#include <stdio.h>
#include <Arduino.h>

WaterSensor::WaterSensor(uint32_t pin) {
  _pin = pin;
}

boolean WaterSensor::begin() {
  pinMode(_pin, INPUT);
  return true;
}

boolean WaterSensor::readValues() {
  _value = analogRead(_pin);
  return true;
}

boolean WaterSensor::printAddress(Stream *serial) {
  serial->print(_pin);
  return true;
}

boolean WaterSensor::printValues(Stream *serial) {
  serial->print(_value);
  return true;
}

