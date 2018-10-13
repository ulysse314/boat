#ifndef GPSSensor_h
#define GPSSensor_h

#include "Sensor.h"

class Adafruit_GPS;

class GPSSensor : public Sensor {
public:
  virtual const char *sensorClass() const override { return "GPS"; };
  virtual const char *sensorType() const override { return "GPS"; };
  virtual void begin() override;
  virtual void loop() override;
  virtual bool printAddress(Stream *serial) override;
  virtual bool printValues(Stream *serial) override;
  Adafruit_GPS *getGPS() { return _gps; };
  
protected:
    Adafruit_GPS *_gps;
};

#endif // GPSSensor_h
