#include "PowerSensor.h"

#include "Version.h"

#include <stdio.h>
#include <Arduino.h>

#if IS_MOUSSAILLON
const double kVoltCoef = 75.83;
const double kAmpereCoef = 22.3;
#elif IS_TELEMAQUE
const double kVoltCoef = 76.02;
const double kAmpereCoef = 22.3;
#else
#error *** No boat defined ***
#endif

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

bool PowerSensor::begin() {
  pinMode(_amperePin, INPUT);
  pinMode(_voltPin, INPUT);
  return true;
}

bool PowerSensor::readValues() {
  _rawAmpere = analogRead(_amperePin);
  _rawVolt = analogRead(_voltPin);
  return true;
}

bool PowerSensor::printValues(Stream *serial) {
  serial->print((double)_rawAmpere / kAmpereCoef);
  serial->print(" ");
  serial->print(_rawAmpere);
  serial->print(" ");
  serial->print((double)_rawVolt / kVoltCoef);
  serial->print(" ");
  serial->print(_rawVolt);
  return true;
}

