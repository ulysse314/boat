#include "HullController.h"

#include "ADS1115Sensor.h"
#include "BME680Sensor.h"
#include "BNO055Sensor.h"
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
    _bme680Sensor(new BME680Sensor(hardwareConfig->bme680Address(), hardwareConfig->i2c())),
    _bno055Sensor(new BNO055Sensor(hardwareConfig->bno055Address(), hardwareConfig->i2c())),
    _water(Value::Type::Integer, "wtr"),
    _temperature(Value::Type::Double, "T"),
    _humidity(Value::Type::Double, "hm"),
    _pressure(Value::Type::Double, "prss"),
    _accX(Value::Type::Double, "accx"),
    _accY(Value::Type::Double, "accy"),
    _accZ(Value::Type::Double, "accz"),
    _magX(Value::Type::Double, "magx"),
    _magY(Value::Type::Double, "magy"),
    _magZ(Value::Type::Double, "magz"),
    _gyroX(Value::Type::Double, "gyrx"),
    _gyroY(Value::Type::Double, "gyry"),
    _gyroZ(Value::Type::Double, "gyrz"),
    _eulerX(Value::Type::Double, "eulx"),
    _eulerY(Value::Type::Double, "euly"),
    _eulerZ(Value::Type::Double, "eulz") {
}

HullController::~HullController() {
}

void HullController::begin() {
  addValue(&_water);
  addValue(&_temperature);
  addValue(&_humidity);
  addValue(&_pressure);
  addValue(&_accX);
  addValue(&_accY);
  addValue(&_accZ);
  addValue(&_magX);
  addValue(&_magY);
  addValue(&_magZ);
  addValue(&_gyroX);
  addValue(&_gyroY);
  addValue(&_gyroZ);
  addValue(&_eulerX);
  addValue(&_eulerY);
  addValue(&_eulerZ);
  _bme680Sensor->begin();
  _bno055Sensor->begin();
}

void HullController::addSensorsToList(SensorList *sensorList) {
  sensorList->addSensor(_bme680Sensor);
  sensorList->addSensor(_bno055Sensor);
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
  if (_bno055Sensor->getAvailable()) {
    _accX.setDouble(_bno055Sensor->getAcc().x);
    _accY.setDouble(_bno055Sensor->getAcc().y);
    _accZ.setDouble(_bno055Sensor->getAcc().z);
    _magX.setDouble(_bno055Sensor->getMag().x);
    _magY.setDouble(_bno055Sensor->getMag().y);
    _magZ.setDouble(_bno055Sensor->getMag().z);
    _gyroX.setDouble(_bno055Sensor->getGyro().x);
    _gyroY.setDouble(_bno055Sensor->getGyro().y);
    _gyroZ.setDouble(_bno055Sensor->getGyro().z);
    _eulerX.setDouble(_bno055Sensor->getEuler().x);
    _eulerY.setDouble(_bno055Sensor->getEuler().y);
    _eulerZ.setDouble(_bno055Sensor->getEuler().z);
  }
}
