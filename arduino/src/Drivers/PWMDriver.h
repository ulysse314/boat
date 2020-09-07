#ifndef PWMDriver_h
#define PWMDriver_h

#include "../Drivers/Driver.h"

#include <PCA9685.h>

class HardwareConfig;

class PWMDriver : public Driver {
public:
  PWMDriver(HardwareConfig *hardwareConfig);

  void sensorsHasBeenUpdated();
  bool setValueForMotor(int8_t value, uint8_t motorID);
  void begin() override;
  void loop() override;

private:
  PCA9685 _pca9685;
};

#endif // PWMDriver_h
