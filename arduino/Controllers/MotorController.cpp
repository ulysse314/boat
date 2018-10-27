#include "MotorController.h"

#include "Adafruit_PWMServoDriver.h"
#include "MotorError.h"
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

#define InfoTemperature 50.0
#define WarningTemperature 60.0
#define CriticalTemperature 70.0

// static
MotorController *MotorController::LeftMotor(OneWire *oneWire, Adafruit_PWMServoDriver *pwmDriver) {
  static MotorController*motor = NULL;
  if (!motor) {
    motor = new MotorController("lm", oneWire, pwmDriver, kLeftDallasAddress);
  }
  return motor;
}

// static
MotorController *MotorController::RightMotor(OneWire *oneWire, Adafruit_PWMServoDriver *pwmDriver) {
  static MotorController*motor = NULL;
  if (!motor) {
    motor = new MotorController("rm", oneWire, pwmDriver, kRightDallasAddress);
  }
  return motor;
}

MotorController::MotorController(const char *name, OneWire *oneWire, Adafruit_PWMServoDriver *pwmDriver, const uint8_t dallasAddress[8]) :
    _name(createStringCopy(name)),
    _pwmDriver(pwmDriver),
    _temperatureSensor(new DallasSensor(dallasAddress, oneWire)),
    _temperature(Value::Type::Double, "t") {
}

MotorController::~MotorController() {
}

void MotorController::addSensorsToList(SensorList *sensorList) {
  sensorList->addSensor(_temperatureSensor);
}

void MotorController::begin() {
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
}
