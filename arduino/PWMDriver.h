#ifndef PWMDriver_h
#define PWMDriver_h

#include <Adafruit_PWMServoDriver.h>

class PWMDriver {
public:
  PWMDriver(TwoWire *i2c, int address);

  void begin();
  void sensorsHasBeenUpdated();
  void loop();
  bool setValueForMotor(int8_t value, uint8_t motorID);

private:
  Adafruit_PWMServoDriver _adafruitDriver;
  bool _available;
};

#endif // PWMDriver_h
