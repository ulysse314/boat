#ifndef Sensor_h
#define Sensor_h

#include <Arduino.h>

class Sensor {
public:
  virtual const char *sensorClass() const = 0;
  virtual const char *sensorType() const { return ""; };
  virtual const char *addressString() const { return ""; };
  virtual const char *copyAddressString() const { return NULL; };
  virtual boolean begin() { return true; };
  virtual boolean isReady() { return true; };
  virtual boolean loop() { return true; };
  virtual boolean readValues() { return true; };
  virtual boolean resetValues() { return true; };
  virtual boolean printInfo(Stream *serial, int nodeID = -1);
  virtual boolean printAddress(Stream *serial);
  virtual boolean printValues(Stream *serial) { return false; };
};

#endif // Sensor_h
