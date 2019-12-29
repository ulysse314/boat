#include "MotorController.h"

#include "DallasSensor.h"
#include "HardwareConfig.h"
#include "MotorError.h"
#include "PiLink.h"
#include "PWMDriver.h"
#include "SensorList.h"
#include "StringUtils.h"

#define InfoTemperature                 50.0
#define WarningTemperature              60.0
#define CriticalTemperature             70.0

// static
MotorController *MotorController::LeftMotor(PWMDriver *pwmDriver, HardwareConfig *hardwareConfig) {
  static MotorController*motor = NULL;
  if (!motor) {
    motor = new MotorController("motor-l", pwmDriver, hardwareConfig->getLeftMotorPWMID(), hardwareConfig->getLeftMotorDallasAddress(), hardwareConfig);
  }
  return motor;
}

// static
MotorController *MotorController::RightMotor(PWMDriver *pwmDriver, HardwareConfig *hardwareConfig) {
  static MotorController*motor = NULL;
  if (!motor) {
    motor = new MotorController("motor-r", pwmDriver, hardwareConfig->getRightMotorPWMID(), hardwareConfig->getRightMotorDallasAddress(), hardwareConfig);
  }
  return motor;
}

MotorController::MotorController(const char *name, PWMDriver *pwmDriver, uint8_t motorID, const OneWire::Address dallasAddress, HardwareConfig *hardwareConfig) :
    _name(createStringCopy(name)),
    _pwmDriver(pwmDriver),
    _temperatureSensor(new DallasSensor(dallasAddress, hardwareConfig->getOneWire())),
    _motorID(motorID),
    _power(Value::Type::Integer, "pwr"),
    _temperature(Value::Type::Double, "t") {
}

MotorController::~MotorController() {
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
      addError(new MotorError(MotorError::CodeTemperatureInfo));
    } else if (temperature < CriticalTemperature) {
      addError(new MotorError(MotorError::CodeTemperatureWarning));
    } else {
      addError(new MotorError(MotorError::CodeTemperatureCritical));
    }
  } else {
    addError(new MotorError(MotorError::CodeTemperatureUnknown));
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
