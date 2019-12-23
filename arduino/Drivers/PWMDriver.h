#ifndef PWMDriver_h
#define PWMDriver_h

#include "Driver.h"

#include <PCA9685.h>

class PWMDriver : public Driver {
public:
  PWMDriver(uint8_t address, TwoWire *i2cBus);

  void sensorsHasBeenUpdated();
  bool setValueForMotor(int8_t value, uint8_t motorID);
  void begin() override;
  void loop() override;

private:
  PCA9685 _pca9685;
};

#endif // PWMDriver_h
