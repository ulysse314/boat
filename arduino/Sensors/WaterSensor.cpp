#include "WaterSensor.h"

#include <stdio.h>
#include <Arduino.h>

WaterSensor::WaterSensor(uint32_t pin) {
  _pin = pin;
}

bool WaterSensor::begin() {
  pinMode(_pin, INPUT);
  return true;
}

bool WaterSensor::readValues() {
  _value = analogRead(_pin);
  return true;
}

bool WaterSensor::printAddress(Stream *serial) {
  serial->print(_pin);
  return true;
}

bool WaterSensor::printValues(Stream *serial) {
  serial->print(_value);
  return true;
}

