#include "Sensor.h"

#ifndef PowerSensor_h
#define PowerSensor_h

class PowerSensor : public Sensor {
public:
  PowerSensor(uint32_t amperePin, uint32_t voltPin);
  ~PowerSensor();
  
  const char *sensorClass() { return "PowerSensor"; };
  const char *addressString() { return _address; };
  boolean begin();
  boolean readValues();
  boolean printValues(Stream *serial);
  
protected:
  const char *_address = NULL;
  uint32_t _amperePin = 0;
  uint32_t _voltPin = 0;
  uint32_t _rawAmpere = 0;
  uint32_t _rawVolt = 0;
};

#endif // PowerSensor_h
