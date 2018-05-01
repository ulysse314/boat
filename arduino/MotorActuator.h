#include "Actuator.h"

#ifndef MotorActuator_h
#define MotorActuator_h

class Adafruit_PWMServoDriver;

class MotorActuator : public Actuator {
public:
  MotorActuator();
  ~MotorActuator();

  const char *addressString() const override { return "Motor"; };
  bool begin() override;
  bool processValues(const char *values) override;
  
private:
  Adafruit_PWMServoDriver *_pwmDriver;

  void setStringValueForMotor(const char *stringValue, int motorID);
  void setValueForMotor(int value, int motorID);
};

#endif // MotorActuator_h