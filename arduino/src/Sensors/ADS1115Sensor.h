#ifndef ADS1115Sensor_h
#define ADS1115Sensor_h

#include "../Sensors/Sensor.h"

#include <inttypes.h>

class ADS1115;
class TwoWire;

class ADS1115Sensor : public Sensor {
public:
  ADS1115Sensor(uint8_t address, TwoWire *i2cBus);
  ~ADS1115Sensor();

  virtual const char *sensorClass() const override { return "ADS"; };
  virtual const char *sensorType() const override { return "ADS1115"; };
  virtual void begin() override;
  virtual void loop() override;
  virtual bool readValues() override;

  bool getAvailable() const { return _available; };
  int16_t getValue0() const { return _value0; };
  int16_t getValue1() const { return _value1; };
  int16_t getValue2() const { return _value2; };
  int16_t getValue3() const { return _value3; };

protected:
  ADS1115 *_ads1115;
  bool _available;
  unsigned long long _lastMeasure = 0;
  int16_t _value0 = 0;
  int16_t _value1 = 0;
  int16_t _value2 = 0;
  int16_t _value3 = 0;
};

#endif // ADS1115Sensor_h
