#ifndef AnalogSensor_h
#define AnalogSensor_h

#include <inttypes.h>

#include "../Sensors/Sensor.h"

class AnalogSensor : public Sensor {
public:
  AnalogSensor(uint32_t pin);

  const char *sensorClass() const override { return "AnalogSensor"; };
  void begin() override;
  bool readValues() override;

protected:
  uint32_t _pin = 0;
  uint32_t _value = 0;
};

#endif // AnalogSensor_h
