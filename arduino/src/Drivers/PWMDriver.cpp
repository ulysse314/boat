#include "PWMDriver.h"

#include "../Controllers/ArduinoController.h"
#include "../Main/HardwareConfig.h"

// Frequency to set for the PWM chip
#define FREQUENCY_SET 250
// Frequency mesured
#define MEASURED_FREQUENCY 253.5
// Real expected value, sec: 0.0019
#define PWM_FORWARD_S 0.001905
// Real expected value, sec: 0.0015
#define PWM_STOPPED_S 0.0015
// Real expected value, sec: 0.0011
#define PWM_REVERSE_S 0.001095

#define FORWARD() (PWM_FORWARD_S * 4096.0 * MEASURED_FREQUENCY)
#define STOPPED() (PWM_STOPPED_S * 4096.0 * MEASURED_FREQUENCY)
#define REVERSED() (PWM_REVERSE_S * 4096.0 * MEASURED_FREQUENCY)

PWMDriver::PWMDriver(HardwareConfig *hardwareConfig) :
    _pca9685(hardwareConfig->pca9685Address(), hardwareConfig->i2c()) {
}

void PWMDriver::begin() {
  _available = _pca9685.begin();
  _pca9685.setOscillatorFrequency(24000000);
  _available = _available && _pca9685.setPWMFreq(FREQUENCY_SET);
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
