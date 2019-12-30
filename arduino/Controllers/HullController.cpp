#include "HullController.h"

#include "ADS1115Sensor.h"
#include "BME680Sensor.h"
#include "HardwareConfig.h"
#include "HullError.h"
#include "SensorList.h"

#define kLeakThresshold            10000
#define kTemperatureCritical       65
#define kTemperatureWarning        50
#define kTemperatureInfo           45
#define kTemperatureInvalid        2

HullController::HullController(ADS1115Sensor *ads1115Sensor, HardwareConfig *hardwareConfig) :
    _ads1115Sensor(ads1115Sensor),
    _bme680Sensor(new BME680Sensor(hardwareConfig->getBME680Address(), hardwareConfig->getI2C())),
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
    int16_t water = _ads1115Sensor->getValue0();
    _water.setInteger(water);
    if (water > kLeakThresshold) {
      addError(new HullError(HullError::CodeLeak));
    }
  } else {
    _water.setNull();
    addError(new HullError(HullError::CodeADS1115NotFound));
  }
  float temperature = _bme680Sensor->getTemperature();
  _temperature.setDouble(temperature);
  if (temperature >= kTemperatureCritical) {
    addError(new HullError(HullError::TemperatureCritical));
  } else if (temperature >= kTemperatureWarning) {
    addError(new HullError(HullError::TemperatureWarning));
  } else if (temperature >= kTemperatureInfo) {
    addError(new HullError(HullError::TemperatureInfo));
  } else if (temperature <= kTemperatureInvalid) {
    addError(new HullError(HullError::TemperatureInvalid));
  }
  _humidity.setDouble(_bme680Sensor->getHumidity());
  _pressure.setDouble(_bme680Sensor->getPressure());
}
