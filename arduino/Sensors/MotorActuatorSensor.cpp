#include "MotorActuatorSensor.h"

#include "Adafruit_PWMServoDriver.h"

// Frequency to set for the PWM chip
#define ADAFRUIT_FREQUENCE 250
// Real expected value, sec: 0.0019
#define PWM_FORWARD_S 0.001905
// Real expected value, sec: 0.0015
#define PWM_STOPPED_S 0.0015
// Real expected value, sec: 0.0011
#define PWM_REVERSE_S 0.0011
// Frequency mesured
#define REAL_FREQUENCE 238
#define LEFT_MOTOR_ID 0
#define RIGHT_MOTOR_ID 1
// Time out in msec
#define TIME_OUT 4000

#define FORWARD() (PWM_FORWARD_S * 4096.0 * REAL_FREQUENCE)
#define STOPPED() (PWM_STOPPED_S * 4096.0 * REAL_FREQUENCE)
#define REVERSED() (PWM_REVERSE_S * 4096.0 * REAL_FREQUENCE)


MotorActuatorSensor::MotorActuatorSensor() {
  Wire.begin();
  Wire.setClock(3400000L);
  _pwmDriver = new Adafruit_PWMServoDriver(&Wire);
}

MotorActuatorSensor::~MotorActuatorSensor() {
  delete _pwmDriver;
}

void MotorActuatorSensor::begin() {
  _pwmDriver->begin();
  _pwmDriver->setPWMFreq(ADAFRUIT_FREQUENCE);
  delay(10);
}

bool MotorActuatorSensor::processValues(const char *values) {
  _lastUpdate = millis();
  if (strcmp(values, "ping") == 0) {
    return true;
  }
  _leftResult = setStringValueForMotor(values, &_left, LEFT_MOTOR_ID);
  while (values[0] != ' ' && values[0] != 0) {
    values += 1;
  }
  if (values[0] != ' ') {
    return true;
  }
  values += 1;
  _rightResult = setStringValueForMotor(values, &_right, RIGHT_MOTOR_ID);
  return true;
}

void MotorActuatorSensor::loop() {
  if ((unsigned long)(millis() - _lastUpdate) > TIME_OUT) {
    _left = 0;
    _leftResult = setValueForMotor(_left, LEFT_MOTOR_ID);
    _right = 0;
    _rightResult = setValueForMotor(_right, RIGHT_MOTOR_ID);
  }
}

uint8_t MotorActuatorSensor::setStringValueForMotor(const char *stringValue, int *value, int motorID) {
  if (stringValue[0] == 'N') {
    // None
    return -1;
  }
  *value = atoi(stringValue);
  return setValueForMotor(*value, motorID);
}

uint8_t MotorActuatorSensor::setValueForMotor(int value, int motorID) {
  int realValue = 0.;
  if (value < 0) {
    realValue = STOPPED() + (STOPPED() - REVERSED()) * value / 100.;
  } else if (value > 0) {
    realValue = STOPPED() + (FORWARD() - STOPPED()) * value / 100.;
  } else {
    realValue = STOPPED();
  }
  if (_left == 0 && _right == 0) {
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  return _pwmDriver->setPWM(motorID, 0, realValue);
}

bool MotorActuatorSensor::printValues(Stream *serial) {
  serial->print(_left);
  serial->print(" ");
  serial->print(_right);
  serial->print(" ");
  serial->print(_leftResult);
  serial->print(" ");
  serial->print(_rightResult);
  return true;
}
