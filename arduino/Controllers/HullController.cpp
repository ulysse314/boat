#include "HullController.h"

#include "ADS1115Sensor.h"
#include "BME680Sensor.h"
#include "HullError.h"
#include "SensorList.h"

#define kLeakThresshold      3000

HullController::HullController(ADS1115Sensor *ads1115Sensor, TwoWire *i2cBus) :
    _ads1115Sensor(ads1115Sensor),
    _bme680Sensor(new BME680Sensor(i2cBus)),
    _water(Value::Type::Integer, "water"),
    _temperature(Value::Type::Double, "temp"),
    _humidity(Value::Type::Double, "humi"),
    _pressure(Value::Type::Double, "press") {
}

HullController::~HullController() {
}

void HullController::begin() {
  addValue(&_water);
  addValue(&_temperature);
  addValue(&_humidity);
  addValue(&_pressure);
  _bme680Sensor->begin();
}

void HullController::addSensorsToList(SensorList *sensorList) {
  sensorList->addSensor(_bme680Sensor);
}

void HullController::sensorsHasBeenUpdated() {
  if (_ads1115Sensor->getAvailable()) {
    uint16_t water = _ads1115Sensor->getValue0();
    _water.setInteger(water);
    if (water < kLeakThresshold) {
      addError(new HullError(HullError::CodeLeak));
    }
  } else {
    _water.setNull();
    addError(new HullError(HullError::CodeADS1115NotFound));
  }
  //_temperature.setDouble(_bme680Sensor->getTemperature());
  //_humidity.setDouble(_bme680Sensor->getHumidity());
  //_pressure.setDouble(_bme680Sensor->getPressure());
}
