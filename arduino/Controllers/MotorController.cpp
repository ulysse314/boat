#include "MotorController.h"

#include "DallasSensor.h"
#include "HardwareConfig.h"
#include "MotorError.h"
#include "PiLink.h"
#include "PWMDriver.h"
#include "SensorList.h"
#include "StringUtils.h"

#define InfoTemperature                 55.0
#define WarningTemperature              65.0
#define CriticalTemperature             75.0

// static
MotorController *MotorController::LeftMotor(PWMDriver *pwmDriver, HardwareConfig *hardwareConfig) {
  static MotorController*motor = NULL;
  if (!motor) {
    motor = new MotorController("mtr-l", "left", pwmDriver, hardwareConfig->getLeftMotorPWMID(), hardwareConfig->getLeftMotorDallasAddress(), hardwareConfig);
  }
  return motor;
}

// static
MotorController *MotorController::RightMotor(PWMDriver *pwmDriver, HardwareConfig *hardwareConfig) {
  static MotorController*motor = NULL;
  if (!motor) {
    motor = new MotorController("mtr-r", "right", pwmDriver, hardwareConfig->getRightMotorPWMID(), hardwareConfig->getRightMotorDallasAddress(), hardwareConfig);
  }
  return motor;
}

MotorController::MotorController(const char *name, const char *humanName, PWMDriver *pwmDriver, uint8_t motorID, const OneWire::Address dallasAddress, HardwareConfig *hardwareConfig) :
    _name(createStringCopy(name)),
    _humanName(createStringCopy(humanName)),
    _pwmDriver(pwmDriver),
    _temperatureSensor(new DallasSensor(dallasAddress, hardwareConfig->getOneWire())),
    _motorID(motorID),
    _power(Value::Type::Integer, "pwr"),
    _temperature(Value::Type::Double, "t") {
}

MotorController::~MotorController() {
  free((void *)_name);
  free((void *)_humanName);
  delete _temperatureSensor;
}

void MotorController::addSensorsToList(SensorList *sensorList) {
  sensorList->addSensor(_temperatureSensor);
}

void MotorController::begin() {
  addValue(&_power);
  addValue(&_temperature);
}

void MotorController::sensorsHasBeenUpdated() {
  if (_temperatureSensor->hasValue()) {
    double temperature = _temperatureSensor->celsius();
    _temperature.setDouble(temperature);
    if (temperature < InfoTemperature) {
      // No error.
    } else if (temperature < WarningTemperature) {
      addError(new MotorError(MotorError::CodeTemperatureInfo, _humanName));
    } else if (temperature < CriticalTemperature) {
      addError(new MotorError(MotorError::CodeTemperatureWarning, _humanName));
    } else {
      addError(new MotorError(MotorError::CodeTemperatureCritical, _humanName));
    }
  } else {
    addError(new MotorError(MotorError::CodeTemperatureUnknown, _humanName));
    _temperature.setNull();
  }
  if (PiLink::getSharedPiLink()->hasTimedOut()) {
    setValue(0);
  }
}

void MotorController::setValue(int value) {
  if (value > 100) {
    value = 100;
  } else if (value < -100) {
    value = -100;
  }
  _power.setInteger(value);
  _pwmDriver->setValueForMotor(value, _motorID);
}
