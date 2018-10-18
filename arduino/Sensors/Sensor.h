#ifndef Sensor_h
#define Sensor_h

#include <Arduino.h>

class Sensor {
public:
  virtual ~Sensor() {};
  virtual const char *sensorClass() const = 0;
  virtual const char *sensorType() const { return ""; };
  virtual const char *addressString() const { return ""; };
  virtual const char *copyAddressString() const { return NULL; };
  virtual void begin() {};
  virtual bool isReady() { return true; };
  virtual void loop() {};
  virtual bool readValues() { return true; };
  virtual bool resetValues() { return true; };
  virtual bool printInfo(Stream *serial, int nodeID = -1);
  virtual bool printAddress(Stream *serial);
  virtual bool printValues(Stream *) { return false; };
};

#endif // Sensor_h
