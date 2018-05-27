#include "InfoActuatorSensor.h"
#include "MemoryFree.h"
#include "Version.h"

// Sensor

bool InfoActuatorSensor::printInfo(Stream *serial, int nodeID) {
  if (!_needPrintInfo || _timestamp < 5) {
    // Don't send info before 5s after boot. Wait until everything
    // is settle down.
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
  serial->print(_cycleCount);
  serial->print(" ");
  serial->print(_loopDuration);
  serial->print(" ");
  serial->print(millis());
  serial->print(" ");
  serial->print(_timestamp);
  serial->print(" ");
  serial->print(__DATE__);
  serial->print(" ");
  serial->print(__TIME__);
  serial->print(" ");
  serial->print(ArduinoVersion);
  return true;
}

bool InfoActuatorSensor::readValues() {
  unsigned int freeRAM = freeMemory();
  _ramDifference = freeRAM - _freeRAM;
  _freeRAM = freeRAM;
  _needPrintInfo = _needPrintInfo || (_ramDifference != 0);
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
