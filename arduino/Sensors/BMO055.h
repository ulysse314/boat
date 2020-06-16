#ifndef BNO055Sensor_h
#define BNO055Sensor_h

#include "Sensor.h"

#include <inttypes.h>

class BNO055;
class TwoWire;

class BNO055Sensor : public Sensor {
public:
  BNO055Sensor(uint8_t address, TwoWire *i2cBus);
  ~BNO055Sensor();

  // Sensor
  virtual const char *sensorClass() const override { return "ADS"; };
  virtual const char *sensorType() const override { return "BNO055"; };
  virtual void begin() override;
  virtual void loop() override;
  virtual bool readValues() override;

  bool getAvailable() const { return _available; };
  int16_t getValue0() const { return _value0; };
  int16_t getValue1() const { return _value1; };
  int16_t getValue2() const { return _value2; };
  int16_t getValue3() const { return _value3; };

protected:
  BNO055 *_bno055;
  bool _available;
  unsigned long long _lastMeasure = 0;
  int16_t _value0 = 0;
  int16_t _value1 = 0;
  int16_t _value2 = 0;
  int16_t _value3 = 0;
};

#endif // BNO055Sensor_h
