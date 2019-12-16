#ifndef ADS1115Driver_h
#define ADS1115Driver_h

#include "Driver.h"

#include <Adafruit_ADS1015.h>

class ADS1115Driver : public Driver {
public:
  ADS1115Driver();
  void begin() override;
  void loop() override;

  uint16_t getValue0() { return _value0; };
  uint16_t getValue1() { return _value1; };
  uint16_t getValue2() { return _value2; };
  uint16_t getValue3() { return _value3; };

protected:
  Adafruit_ADS1015 _ads1115;
  unsigned long long _lastMeasure = 0;
  uint16_t _value0 = 0;
  uint16_t _value1 = 0;
  uint16_t _value2 = 0;
  uint16_t _value3 = 0;
};

#endif // ADS1115Driver_h
