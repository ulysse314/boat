#ifndef INA219Sensor_h
#define INA219Sensor_h

#include "../Sensors/Sensor.h"

#include <inttypes.h>

class INA219;
class TwoWire;

class INA219Sensor : public Sensor {
public:
  INA219Sensor(float shuntValue, float maxCurrent, uint8_t address, TwoWire *i2cBus);
  virtual ~INA219Sensor();

  virtual const char *sensorClass() const override { return "INA"; };
  virtual const char *sensorType() const override { return "INA219"; };
  virtual void begin() override;
  virtual void loop() override;
  virtual bool readValues() override;
  bool hasValue() const { return _hasValue; };
  float getVoltage() { return _voltage; };
  float getCurrent() { return _current; };

protected:
  INA219 *_ina219;
  float _shuntValue;
  float _maxCurrent;
  bool _hasValue;
  float _voltage;
  float _current;
};

#endif // INA219Sensor_h
