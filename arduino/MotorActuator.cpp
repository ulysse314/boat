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
  int left = atoi(values);
  int right = 0;
  while (values[0] != ' ' && values[0] != 0) {
    values++;
  }
  if (values[0] == ' ') {
    values++;
    right = atoi(values);
  }
  return true;
}
