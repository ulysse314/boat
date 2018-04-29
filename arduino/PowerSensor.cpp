#include "PowerSensor.h"

#include <stdio.h>
#include <Arduino.h>

PowerSensor::PowerSensor(uint32_t amperePin, uint32_t voltPin) {
  _amperePin = amperePin;
  _voltPin = voltPin;
  char *address = (char *)malloc(16);
  sprintf(address, "A:%d,V:%d", _amperePin, _voltPin);
  _address = address;
}

PowerSensor::~PowerSensor() {
  free((void *)_address);
}

boolean PowerSensor::begin() {
  pinMode(_amperePin, INPUT);
  pinMode(_voltPin, INPUT);
  return true;
}

boolean PowerSensor::readValues() {
  _rawAmpere = analogRead(_amperePin);
  _rawVolt = analogRead(_voltPin);
  return true;
}

boolean PowerSensor::printValues(Stream *serial) {
  serial->print(_rawAmpere);
  serial->print(" ");
  serial->print(_rawAmpere);
  serial->print(" ");
  serial->print(_rawVolt);
  serial->print(" ");
  serial->print(_rawVolt);
  return true;
}

