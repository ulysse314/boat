#include "MotorController.h"

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

// static
MotorController *MotorController::LeftMotor(OneWire *oneWire) {
  static MotorController*motor = NULL;
  if (!motor) {
    motor = new MotorController("lm", oneWire, kLeftDallasAddress);
  }
  return motor;
}

// static
MotorController *MotorController::RightMotor(OneWire *oneWire) {
  static MotorController*motor = NULL;
  if (!motor) {
    motor = new MotorController("rm", oneWire, kRightDallasAddress);
  }
  return motor;
}

MotorController::MotorController(const char *name, OneWire *oneWire, const uint8_t dallasAddress[8]) :
    _name(createStringCopy(name)),
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
    _temperature.setDouble(_temperatureSensor->celsius());
  } else {
    addError(new MotorError(MotorError::CodeTemperatureUnknown));
    _temperature.setNull();
  }
}
