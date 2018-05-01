#include "Sensor.h"

#ifndef PowerSensor_h
#define PowerSensor_h

class PowerSensor : public Sensor {
public:
  PowerSensor(uint32_t amperePin, uint32_t voltPin);
  ~PowerSensor();
  
  const char *sensorClass() const override { return "PowerSensor"; };
  const char *addressString() const override { return _address; };
  boolean begin() override;
  boolean readValues() override;
  boolean printValues(Stream *serial) override;
  
protected:
  const char *_address = NULL;
  uint32_t _amperePin = 0;
  uint32_t _voltPin = 0;
  uint32_t _rawAmpere = 0;
  uint32_t _rawVolt = 0;
};

#endif // PowerSensor_h
