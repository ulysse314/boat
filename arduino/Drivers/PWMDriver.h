#ifndef PWMDriver_h
#define PWMDriver_h

#include <Adafruit_PWMServoDriver.h>

#include "Driver.h"

class PWMDriver : public Driver {
public:
  PWMDriver(TwoWire *i2c, int address);

  void begin() override;
  void sensorsHasBeenUpdated();
  void loop() override;
  bool setValueForMotor(int8_t value, uint8_t motorID);

private:
  Adafruit_PWMServoDriver _adafruitDriver;
  bool _available;
};

#endif // PWMDriver_h
