#include "InfoActuatorSensor.h"
#include "MemoryFree.h"

// Sensor

bool InfoActuatorSensor::printInfo(Stream *serial, int nodeID) {
  if (!_needPrintInfo) {
    return true;
  }
  _needPrintInfo = false;
  return Sensor::printInfo(serial, nodeID);
}

bool InfoActuatorSensor::printValues(Stream *serial) {
  serial->print(_freeRAM);
  serial->print(" ");
  serial->print(_ramDifference);
  serial->print(" ");
  serial->print(millis());
  serial->print(" ");
  serial->print(_timestamp);
  serial->print(" ");
  serial->print(__DATE__);
  serial->print(" ");
  serial->print(__TIME__);
  return true;
}

bool InfoActuatorSensor::readValues() {
  unsigned int freeRAM = freeMemory();
  _ramDifference = freeRAM - _freeRAM;
  _needPrintInfo = _ramDifference != 0;
}

bool InfoActuatorSensor::loop() {
  unsigned long currentMillis = millis();
  unsigned long difference = currentMillis - _lastMillis;
  if (difference > 1000) {
    unsigned long addedSeconds = difference / 1000;
    _timestamp += addedSeconds;
    _lastMillis += addedSeconds * 1000;
  }
  return true;
}

// Actuator

bool InfoActuatorSensor::processValues(const char *values) {
  _needPrintInfo = true;
}
