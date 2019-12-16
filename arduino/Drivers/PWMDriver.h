#ifndef PWMDriver_h
#define PWMDriver_h

#include <Adafruit_PWMServoDriver.h>

#include "Driver.h"

class PWMDriver : public Driver {
public:
  PWMDriver(TwoWire *i2c, int address);

  void sensorsHasBeenUpdated();
  bool setValueForMotor(int8_t value, uint8_t motorID);
  void begin() override;
  void loop() override;

private:
  Adafruit_PWMServoDriver _adafruitDriver;
};

#endif // PWMDriver_h
