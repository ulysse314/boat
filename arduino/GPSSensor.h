#ifndef GPSSensor_h
#define GPSSensor_h

#include "Sensor.h"

class Adafruit_GPS;

class GPSSensor : public Sensor {
public:
  virtual const char *sensorClass() override;
  virtual const char *sensorType() override;
  virtual boolean begin() override;
  virtual boolean loop() override;
  virtual boolean printAddress(Stream *serial) override;
  virtual boolean printValues(Stream *serial) override;

protected:
    Adafruit_GPS *_gps;
};

#endif // GPSSensor_h
