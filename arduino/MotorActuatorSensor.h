#include "Actuator.h"
#include "Sensor.h"

#ifndef MotorActuatorSensor_h
#define MotorActuatorSensor_h

class Adafruit_PWMServoDriver;

class MotorActuatorSensor : public Actuator, public Sensor {
public:
  MotorActuatorSensor();
  ~MotorActuatorSensor();

  const char *addressString() const override { return "Motor"; };
  const char *sensorClass() const override { return "Motor"; };
  bool begin() override;
  bool processValues(const char *values) override;
  boolean printValues(Stream *serial) override;
  
private:
  Adafruit_PWMServoDriver *_pwmDriver;
  int _left;
  int _right;

  void setStringValueForMotor(const char *stringValue, int *value, int motorID);
  void setValueForMotor(int value, int motorID);
};

#endif // MotorActuatorSensor_h