#include "InfoActuatorSensor.h"
#include "MemoryFree.h"
#include "Version.h"

// Sensor

bool InfoActuatorSensor::readValues() {
  unsigned int freeRAM = freeMemory();
  _ramDifference = freeRAM - _freeRAM;
  _freeRAM = freeRAM;
  _needPrintInfo = _needPrintInfo || (_ramDifference != 0);
  return true;
}

void InfoActuatorSensor::loop() {
  unsigned long currentMillis = millis();
  unsigned long difference = currentMillis - _lastMillis;
  if (difference > 1000) {
    unsigned long addedSeconds = difference / 1000;
    _timestamp += addedSeconds;
    _lastMillis += addedSeconds * 1000;
  }
}

// Actuator

bool InfoActuatorSensor::processValues(const char *) {
  _needPrintInfo = true;
  return true;
}
