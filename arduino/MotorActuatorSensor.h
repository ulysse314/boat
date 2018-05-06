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
  const char *sensorType() const { return "Motor"; };
  const char *sensorClass() const override { return "Motor"; };
  bool begin() override;
  bool processValues(const char *values) override;
  bool printValues(Stream *serial) override;
  
private:
  Adafruit_PWMServoDriver *_pwmDriver;
  int _left = 0;
  int _right = 0;
  uint8_t _leftResult = 0;
  uint8_t _rightResult = 0;

  uint8_t setStringValueForMotor(const char *stringValue, int *value, int motorID);
  uint8_t setValueForMotor(int value, int motorID);
};

#endif // MotorActuatorSensor_h
