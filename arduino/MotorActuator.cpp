#include "MotorActuator.h"

#include "Adafruit_PWMServoDriver.h"

// micro sec
#define PWM_FORWARD_US 0.0019
// micro sec
#define PWM_STOPPED_US 0.0015
// micro sec
#define PWM_REVERSE_US 0.0011
//#define ADAFRUIT_FREQUENCE 250
#define REAL_FREQUENCE 297.7
#define LEFT_MOTOR_ID 0
#define RIGHT_MOTOR_ID 1

#define FORWARD() (PWM_FORWARD_US * 4096.0 * REAL_FREQUENCE)
#define STOPPED() (PWM_STOPPED_US * 4096.0 * REAL_FREQUENCE)
#define REVERSED() (PWM_REVERSE_US * 4096.0 * REAL_FREQUENCE)


MotorActuator::MotorActuator() {
  _pwmDriver = new Adafruit_PWMServoDriver();
}

MotorActuator::~MotorActuator() {
  delete _pwmDriver;
}

bool MotorActuator::begin() {
  _pwmDriver->begin();
  _pwmDriver->setPWMFreq(REAL_FREQUENCE);
  delay(10);
  return true;
}

bool MotorActuator::processValues(const char *values) {
  setStringValueForMotor(values, LEFT_MOTOR_ID);
  while (values[0] != ' ' && values[0] != 0) {
    values += 1;
  }
  if (values[0] != ' ') {
    return true;
  }
  values += 1;
  setStringValueForMotor(values, RIGHT_MOTOR_ID);
  return true;
}

void MotorActuator::setStringValueForMotor(const char *stringValue, int motorID) {
  if (stringValue[0] == 'N') {
    // None
    return;
  }
  int value = atoi(stringValue);
  setValueForMotor(value, motorID);
}

void MotorActuator::setValueForMotor(int value, int motorID) {
  int realValue = 0.;
  if (value < 0) {
    realValue = STOPPED() + (STOPPED() - REVERSED()) * value / 100.;
  } else if (value > 0) {
    realValue = STOPPED() + (FORWARD() - STOPPED()) * value / 100.;
  } else {
    realValue = STOPPED();
  }
  _pwmDriver->setPWM(motorID, 0, realValue);
}
