#include "PWMDriver.h"

#include "ArduinoController.h"
#include "HardwareConfig.h"

// Frequency to set for the PWM chip
#define ADAFRUIT_FREQUENCE 250
// Real expected value, sec: 0.0019
#define PWM_FORWARD_S 0.001905
// Real expected value, sec: 0.0015
#define PWM_STOPPED_S 0.0015
// Real expected value, sec: 0.0011
#define PWM_REVERSE_S 0.0011
// Frequency mesured
#define REAL_FREQUENCE 247.2

#define FORWARD() (PWM_FORWARD_S * 4096.0 * REAL_FREQUENCE)
#define STOPPED() (PWM_STOPPED_S * 4096.0 * REAL_FREQUENCE)
#define REVERSED() (PWM_REVERSE_S * 4096.0 * REAL_FREQUENCE)

PWMDriver::PWMDriver(HardwareConfig *hardwareConfig) :
    _pca9685(hardwareConfig->getPCA9685Address(), hardwareConfig->getI2C()) {
}

void PWMDriver::begin() {
  _available = _pca9685.begin();
  _pca9685.setOscillatorFrequency(27000000);
  _available = _available && _pca9685.setPWMFreq(ADAFRUIT_FREQUENCE);
  Error *error = new ArduinoError(ArduinoError::CodePWMDriverNotAvailable, NULL);
  if (_available) {
    ArduinoController::sharedController()->removeError(error);
  } else {
    ArduinoController::sharedController()->addError(error);
  }
}

void PWMDriver::loop() {
  if (!_available) {
    begin();
  }
}

bool PWMDriver::setValueForMotor(int8_t value, uint8_t motorID) {
  if (!_available) {
    return false;
  }
  uint16_t realValue = 0.;
  if (value < 0) {
    realValue = STOPPED() + (STOPPED() - REVERSED()) * value / 100.;
  } else if (value > 0) {
    realValue = STOPPED() + (FORWARD() - STOPPED()) * value / 100.;
  } else {
    realValue = STOPPED();
  }
  _available = _pca9685.setPWM(motorID, 0, realValue);
  if (!_available) {
    Error *error = new ArduinoError(ArduinoError::CodePWMDriverNotAvailable, NULL);
    ArduinoController::sharedController()->addError(error);
  }
  return _available;
}
