#ifndef PowerSensor_h
#define PowerSensor_h

#include "Sensor.h"

#include <inttypes.h>

class PowerSensor : public Sensor {
public:
  PowerSensor(uint8_t amperePin, uint8_t voltPin);
  ~PowerSensor();
  
  const char *sensorClass() const override { return "PowerSensor"; };
  const char *addressString() const override { return _address; };
  void begin() override;
  bool readValues() override;
  
protected:
  const char *_address = NULL;
  uint8_t _amperePin = 0;
  uint8_t _voltPin = 0;
  uint32_t _rawAmpere = 0;
  uint32_t _rawVolt = 0;
};

#endif // PowerSensor_h
