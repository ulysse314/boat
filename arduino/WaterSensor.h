#include "Sensor.h"

#ifndef WaterSensor_h
#define WaterSensor_h

class WaterSensor : public Sensor {
public:
  WaterSensor(uint32_t pin);
  
  const char *sensorClass() const override { return "WaterSensor"; };
  bool begin() override;
  bool readValues() override;
  bool printAddress(Stream *serial) override;
  bool printValues(Stream *serial) override;
  
protected:
  uint32_t _pin = 0;
  uint32_t _value = 0;
};

#endif // WaterSensor_h
