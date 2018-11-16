#include "MotorController.h"

#include "MotorError.h"
#include "PiLink.h"
#include "PWMDriver.h"
#include "SensorList.h"
#include "StringUtils.h"
#include "Version.h"

#if IS_MOUSSAILLON
uint8_t kLeftDallasAddress[8] = { 0x28, 0x0C, 0x17, 0x1F, 0x03, 0x00, 0x00, 0x08 };
uint8_t kRightDallasAddress[8] = { 0x28, 0x89, 0x1A, 0x1F, 0x03, 0x00, 0x00, 0xE5 };
#elif IS_TELEMAQUE
uint8_t kLeftDallasAddress[8] = { 0x28, 0xC3, 0x10, 0x1F, 0x03, 0x00, 0x00, 0xE2 };
uint8_t kRightDallasAddress[8] = { 0x28, 0xAB, 0xDD, 0x1E, 0x03, 0x00, 0x00, 0xC1 };
#else
#error *** No boat defined ***
#endif

#define LEFT_MOTOR_ID                   0
#define RIGHT_MOTOR_ID                  1

#define InfoTemperature                 50.0
#define WarningTemperature              60.0
#define CriticalTemperature             70.0

// static
MotorController *MotorController::LeftMotor(OneWire *oneWire, PWMDriver *pwmDriver) {
  static MotorController*motor = NULL;
  if (!motor) {
    motor = new MotorController("lm", oneWire, pwmDriver, LEFT_MOTOR_ID, kLeftDallasAddress);
  }
  return motor;
}

// static
MotorController *MotorController::RightMotor(OneWire *oneWire, PWMDriver *pwmDriver) {
  static MotorController*motor = NULL;
  if (!motor) {
    motor = new MotorController("rm", oneWire, pwmDriver, RIGHT_MOTOR_ID, kRightDallasAddress);
  }
  return motor;
}

MotorController::MotorController(const char *name, OneWire *oneWire, PWMDriver *pwmDriver, uint8_t motorID, const uint8_t dallasAddress[8]) :
    _name(createStringCopy(name)),
    _pwmDriver(pwmDriver),
    _temperatureSensor(new DallasSensor(dallasAddress, oneWire)),
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
  if (PiLink::getPiLink()->hasTimedOut()) {
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
