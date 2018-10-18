#include "MotorController.h"

#include "DallasSensor.h"
#include "SensorList.h"
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

MotorController::MotorController(OneWire *oneWire) {
  _leftTemperatureSensor = new DallasSensor(kLeftDallasAddress, oneWire);
  _rightTemperatureSensor = new DallasSensor(kRightDallasAddress, oneWire);
}

MotorController::~MotorController() {
  delete _leftTemperatureSensor;
  delete _rightTemperatureSensor;
}

void MotorController::addSensorsToList(SensorList *sensorList) {
  sensorList->addSensor(_leftTemperatureSensor);
  sensorList->addSensor(_rightTemperatureSensor);
}
