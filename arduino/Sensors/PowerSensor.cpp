#include "PowerSensor.h"

#include "Version.h"

#include <stdio.h>
#include <Arduino.h>

#if IS_MOUSSAILLON
const double kVoltCoef = 303.32;
const double kAmpereCoef = 89.2;
#elif IS_TELEMAQUE
const double kVoltCoef = 304.08;
const double kAmpereCoef = 89.2;
#else
#error *** No boat defined ***
#endif

PowerSensor::PowerSensor(uint8_t amperePin, uint8_t voltPin) {
  _amperePin = amperePin;
  _voltPin = voltPin;
  char *address = (char *)malloc(16);
  sprintf(address, "A:%ud,V:%ud", _amperePin, _voltPin);
  _address = address;
}

PowerSensor::~PowerSensor() {
  free((void *)_address);
}

void PowerSensor::begin() {
  pinMode(_amperePin, INPUT);
  pinMode(_voltPin, INPUT);
}

bool PowerSensor::readValues() {
  _rawAmpere = analogRead(_amperePin);
  _rawVolt = analogRead(_voltPin);
  return true;
}
