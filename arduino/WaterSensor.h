#include "Sensor.h"

#ifndef WaterSensor_h
#define WaterSensor_h

class WaterSensor : public Sensor {
public:
  WaterSensor(uint32_t pin);
  
  const char *sensorClass() { return "WaterSensor"; };
  boolean printAddress(Stream *serial);
  boolean begin();
  boolean readValues();
  boolean printValues(Stream *serial);
  
protected:
  uint32_t _pin = 0;
  uint32_t _value = 0;
};

#endif // WaterSensor_h
