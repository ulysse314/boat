#ifndef PWMDriver_h
#define PWMDriver_h

#include "Driver.h"

#include <PCA9685.h>

class PWMDriver : public Driver {
public:
  PWMDriver(TwoWire *i2c, int address);

  void sensorsHasBeenUpdated();
  bool setValueForMotor(int8_t value, uint8_t motorID);
  void begin() override;
  void loop() override;

private:
  PCA9685 _adafruitDriver;
};

#endif // PWMDriver_h
