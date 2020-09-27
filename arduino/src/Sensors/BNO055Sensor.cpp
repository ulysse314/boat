#include "BNO055Sensor.h"

#include <BNO055.h>
#include <math.h>

#define DATA_FETCH_PERIOD_MS       100

namespace {

bool readVector(BNO055::Vector vectorName, BNO055Sensor::Vector &vector, BNO055 *bno055) {
  imu::Vector<3> xyz;
  if (!bno055->getVector(vectorName, &xyz)) {
    return false;
  }
  vector.x = xyz[0];
  vector.y = xyz[1];
  vector.z = xyz[2];
  return true;
}

BNO055Sensor::Calibration ConvertBNO055CalibrationToBNO055SensorCalibration(BNO055::Calibration calibration) {
  switch (calibration) {
  case BNO055::Calibration::None:
    return BNO055Sensor::Calibration::None;
  case BNO055::Calibration::Low:
    return BNO055Sensor::Calibration::Low;
  case BNO055::Calibration::Medium:
    return BNO055Sensor::Calibration::Medium;
  case BNO055::Calibration::Fully:
    return BNO055Sensor::Calibration::Fully;
  }
  return BNO055Sensor::Calibration::None;
}

}  // namespace

BNO055Sensor::BNO055Sensor(uint8_t address, TwoWire *i2cBus) :
    _bno055(new BNO055((BNO055::Address)address, i2cBus)),
    _available(false),
    _lastDataFetch(0) {
}

BNO055Sensor::~BNO055Sensor() {
}

void BNO055Sensor::begin() {
  _available = _bno055->begin();
  if (!_available) {
    return;
  }
  BNO055::AxesConfiguration axesConfiguration;
  axesConfiguration = BNO055::getAxesConfigurationForDevicePlacement(BNO055::DevicePlacement::P5);
  _available = _bno055->setAxesRemap(axesConfiguration);
}

void BNO055Sensor::loop() {
  unsigned long long currentMillis = millis();
  if (_available && (unsigned long long)(currentMillis - _lastDataFetch) > DATA_FETCH_PERIOD_MS) {
    _lastDataFetch = currentMillis;
    if (!readVector(BNO055::Vector::Magnetometer, _mag, _bno055)) {
      _available = false;
      return;
    }
    _magValues[_magValueIndex] = _mag;
    ++_magValueIndex;
    if (_magValueIndex >= MAG_MAX_COUNT) {
      _magValueIndex = 0;
    }
  }
}

bool BNO055Sensor::readValues() {
  if (!_available) {
    begin();
    return _available;
  }
  BNO055::SystemStatus bno055SystemStatus;
  if (!_bno055->getSystemStatus(&bno055SystemStatus)) {
    _available = false;
    return _available;
  }
  switch (bno055SystemStatus) {
  case BNO055::SystemStatus::Idle:
    _systemStatus = SystemStatus::Idle;
    break;
  case BNO055::SystemStatus::Error:
    _systemStatus = SystemStatus::Error;
    break;
  case BNO055::SystemStatus::InitializingPeripherals:
    _systemStatus = SystemStatus::InitializingPeripherals;
    break;
  case BNO055::SystemStatus::SystemInitialization:
    _systemStatus = SystemStatus::SystemInitialization;
    break;
  case BNO055::SystemStatus::ExecutingSelfTest:
    _systemStatus = SystemStatus::ExecutingSelfTest;
    break;
  case BNO055::SystemStatus::RunningWithFusionAlgorithm:
    _systemStatus = SystemStatus::RunningWithFusionAlgorithm;
    break;
  case BNO055::SystemStatus::RunningWihtoutFusionAlgorithm:
    _systemStatus = SystemStatus::RunningWihtoutFusionAlgorithm;
    break;
  }
  BNO055::SystemError bno055SystemError;
  if (!_bno055->getSystemError(&bno055SystemError)) {
    _available = false;
    return _available;
  }
  switch (bno055SystemError) {
  case BNO055::SystemError::NoError:
    _systemError = SystemError::NoError;
    break;
  case BNO055::SystemError::PeripheralInitializationError:
    _systemError = SystemError::PeripheralInitializationError;
    break;
  case BNO055::SystemError::SystemInitializationError:
    _systemError = SystemError::SystemInitializationError;
    break;
  case BNO055::SystemError::SelfTestResultFailed:
    _systemError = SystemError::SelfTestResultFailed;
    break;
  case BNO055::SystemError::RegisterMapValueOutOfRange:
    _systemError = SystemError::RegisterMapValueOutOfRange;
    break;
  case BNO055::SystemError::RegisterMapAddressOutOfRange:
    _systemError = SystemError::RegisterMapAddressOutOfRange;
    break;
  case BNO055::SystemError::RegisterMapWriteError:
    _systemError = SystemError::RegisterMapWriteError;
    break;
  case BNO055::SystemError::LowPowerNotAvailableForSelectedOperationMode:
    _systemError = SystemError::LowPowerNotAvailableForSelectedOperationMode;
    break;
  case BNO055::SystemError::AccelerometerPowerModeNotAvailable:
    _systemError = SystemError::AccelerometerPowerModeNotAvailable;
    break;
  case BNO055::SystemError::FusionAlgorithmConfigurationError:
    _systemError = SystemError::FusionAlgorithmConfigurationError;
    break;
  case BNO055::SystemError::SensorConfigurationError:
    _systemError = SystemError::SensorConfigurationError;
    break;
  }
  if (!readVector(BNO055::Vector::Accelerometer, _acc, _bno055)) {
    _available = false;
    return _available;
  }
  if (!readVector(BNO055::Vector::Gyroscope, _gyro, _bno055)) {
    _available = false;
    return _available;
  }
  if (!readVector(BNO055::Vector::Euler, _euler, _bno055)) {
    _available = false;
    return _available;
  }
  BNO055::Calibration sysCalibration;
  BNO055::Calibration gyroCalibration;
  BNO055::Calibration accelCalibration;
  BNO055::Calibration magCalibration;
  if (!_bno055->getCalibration(&sysCalibration, &gyroCalibration, &accelCalibration, &magCalibration)) {
    _available = false;
    return _available;
  }
  _sysCalibration = ConvertBNO055CalibrationToBNO055SensorCalibration(sysCalibration);
  _gyroCalibration = ConvertBNO055CalibrationToBNO055SensorCalibration(gyroCalibration);
  _accelCalibration = ConvertBNO055CalibrationToBNO055SensorCalibration(accelCalibration);
  _magCalibration = ConvertBNO055CalibrationToBNO055SensorCalibration(magCalibration);
  if (!_bno055->getSelfTestResult(&_accelSelfTest, &_magSelfTest, &_gyroSelfTest, &_sysSelfTest)) {
    _available = false;
    return _available;
  }
  return _available;
}

double BNO055Sensor::headingForVector(Vector vector) const {
  double heading = atan2(vector.y, vector.x) / PI * 180. - 90.;
  if (heading < -180.) {
    heading += 360.;
  }
  return heading;
}

double BNO055Sensor::getAverageHeading() const {
  Vector average = { 0, 0, 0};
  for (size_t i = 0; i < MAG_MAX_COUNT ; ++i) {
    average.x += _magValues[i].x;
    average.y += _magValues[i].y;
    average.z += _magValues[i].z;
  }
  average.x = average.x / MAG_MAX_COUNT;
  average.y = average.y / MAG_MAX_COUNT;
  average.z = average.z / MAG_MAX_COUNT;
  return headingForVector(average);
}
