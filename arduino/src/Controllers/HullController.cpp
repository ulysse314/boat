#include "HullController.h"

#include "../Errors/HullError.h"
#include "../Main/HardwareConfig.h"
#include "../Sensors/ADS1115Sensor.h"
#include "../Sensors/BME680Sensor.h"
#include "../Sensors/BNO055Sensor.h"
#include "../Sensors/SensorList.h"

#include <Arduino.h>

#define kLeakThresshold            10000
#define kTemperatureCritical       65
#define kTemperatureWarning        50
#define kTemperatureInfo           45
#define kTemperatureInvalid        2

namespace {

const char *BNO055SensorCalibrationToString(BNO055Sensor::Calibration calibration) {
  switch(calibration) {
  case BNO055Sensor::Calibration::None:
    return "None";
  case BNO055Sensor::Calibration::Low:
    return "Low";
  case BNO055Sensor::Calibration::Medium:
    return "Medium";
  case BNO055Sensor::Calibration::Fully:
    return "Fully";
  }
  return "-";
}

}  // namespace

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
    _eulerZ(Value::Type::Double, "eulz"),
    _heading(Value::Type::Double, "head"),
    _averageHeading(Value::Type::Double, "headav") {
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
  addValue(&_heading);
  addValue(&_averageHeading);
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
    _heading.setDouble(_bno055Sensor->getHeading());
    _averageHeading.setDouble(_bno055Sensor->getAverageHeading());
    BNO055Sensor::SystemError systemError = _bno055Sensor->getSystemError();
    if (systemError != BNO055Sensor::SystemError::NoError) {
      String message((uint8_t)systemError);
      addError(new HullError(HullError::BNO055Error, message.c_str()));
    }
    BNO055Sensor::SystemStatus systemStatus = _bno055Sensor->getSystemStatus();
    if (systemStatus != BNO055Sensor::SystemStatus::RunningWithFusionAlgorithm && systemStatus != BNO055Sensor::SystemStatus::RunningWihtoutFusionAlgorithm) {
      String message((uint8_t)systemStatus);
      addError(new HullError(HullError::BNO055SystemStatus, message.c_str()));
    }
    if (!_bno055Sensor->getAccelSelfTest() || !_bno055Sensor->getGyroSelfTest() || !_bno055Sensor->getMagSelfTest() || !_bno055Sensor->getSysSelfTest()) {
      String message;
      message += _bno055Sensor->getAccelSelfTest() ? "A" : "a";
      message += _bno055Sensor->getGyroSelfTest() ? "G" : "g";
      message += _bno055Sensor->getMagSelfTest() ? "M" : "m";
      message += _bno055Sensor->getSysSelfTest() ? "S" : "s";
      addError(new HullError(HullError::BNO055SelfTest, message.c_str()));
    }
    if (_bno055Sensor->getAccelCalibration() != BNO055Sensor::Calibration::Fully) {
      addError(new HullError(HullError::BNO055AccelCalibration, BNO055SensorCalibrationToString(_bno055Sensor->getAccelCalibration())));
    }
    if (_bno055Sensor->getGyroCalibration() != BNO055Sensor::Calibration::Fully) {
      addError(new HullError(HullError::BNO055GyroCalibration, BNO055SensorCalibrationToString(_bno055Sensor->getGyroCalibration())));
    }
    if (_bno055Sensor->getMagCalibration() != BNO055Sensor::Calibration::Fully) {
      addError(new HullError(HullError::BNO055MagCalibration, BNO055SensorCalibrationToString(_bno055Sensor->getMagCalibration())));
    }
    if (_bno055Sensor->getSysCalibration() != BNO055Sensor::Calibration::Fully) {
      addError(new HullError(HullError::BNO055SysCalibration, BNO055SensorCalibrationToString(_bno055Sensor->getSysCalibration())));
    }
  }
}
