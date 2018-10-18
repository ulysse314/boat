#include "BatteryController.h"

#include "AnalogSensor.h"
#include "DallasSensor.h"
#include "SensorList.h"
#include "Version.h"

#if IS_MOUSSAILLON
const double kVoltCoef = 303.32;
const double kAmpereCoef = 89.2;
uint8_t kDallasAddress[8] = { 0x28, 0x6F, 0x04, 0x1F, 0x03, 0x00, 0x00, 0xA0 };
#elif IS_TELEMAQUE
const double kVoltCoef = 304.08;
const double kAmpereCoef = 89.2;
uint8_t kDallasAddress[8] = { 0x28, 0x58, 0xDB, 0x1E, 0x03, 0x00, 0x00, 0x76 };
#else
#error *** No boat defined ***
#endif

#define CurrentSensorPin A0
#define TensionSensorPin A1

BatteryController::BatteryController(OneWire *oneWire) {
  _temperatureSensor = new DallasSensor(kDallasAddress, oneWire);
  _currentSensor = new AnalogSensor(CurrentSensorPin);
  _tensionSensor = new AnalogSensor(TensionSensorPin);
}

BatteryController::~BatteryController() {
  delete _temperatureSensor;
  delete _currentSensor;
  delete _tensionSensor;
}

void BatteryController::addSensorsToList(SensorList *sensorList) {
  sensorList->addSensor(_temperatureSensor);
  sensorList->addSensor(_currentSensor);
  sensorList->addSensor(_tensionSensor);
}
