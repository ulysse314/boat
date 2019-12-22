#ifndef ADS1115Sensor_h
#define ADS1115Sensor_h

#include "Sensor.h"

#include <Adafruit_ADS1015.h>

class ADS1115Sensor : public Sensor {
public:
  ADS1115Sensor();

  virtual const char *sensorClass() const override { return "ADS"; };
  virtual const char *sensorType() const override { return "ADS1115"; };
  virtual void begin() override;
  virtual void loop() override;
  virtual bool readValues() override;
  virtual bool printAddress(Stream *serial) override;
  virtual bool printValues(Stream *serial) override;

  bool getAvailable() const { return _available; };
  uint16_t getValue0() const { return _value0; };
  uint16_t getValue1() const { return _value1; };
  uint16_t getValue2() const { return _value2; };
  uint16_t getValue3() const { return _value3; };

protected:
  Adafruit_ADS1115 _ads1115;
  bool _available;
  unsigned long long _lastMeasure = 0;
  uint16_t _value0 = 0;
  uint16_t _value1 = 0;
  uint16_t _value2 = 0;
  uint16_t _value3 = 0;
};

#endif // ADS1115Sensor_h
