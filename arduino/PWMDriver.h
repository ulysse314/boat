#ifndef PWMDriver_h
#define PWMDriver_h

#include <Adafruit_PWMServoDriver.h>

class PWMDriver {
public:
  PWMDriver(TwoWire *i2c, int address);

  void begin();
  void sensorsHasBeenUpdated();
  void loop();
  void setLeftMotorValue(int8_t value);
  void setRightMotorValue(int8_t value);

private:
  bool setValueForMotor(int8_t value, uint8_t motorID);

  Adafruit_PWMServoDriver _adafruitDriver;
  bool _available;
  int8_t _leftMotorValue;
  int8_t _rightMotorValue;
};

#endif // PWMDriver_h
