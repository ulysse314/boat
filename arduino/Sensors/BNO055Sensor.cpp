#include "BNO055Sensor.h"

#include <BNO055.h>

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

}  // namespace

BNO055Sensor::BNO055Sensor(uint8_t address, TwoWire *i2cBus) :
    _bno055(new BNO055((BNO055::Address)address, i2cBus)),
    _available(false) {
}

BNO055Sensor::~BNO055Sensor() {
}

void BNO055Sensor::begin() {
  _available = _bno055->begin();
  if (_available) {
    _available = _bno055->setPlacementConfig(BNO055::PlacementConfig::P5);
  }
}

void BNO055Sensor::loop() {
}

bool BNO055Sensor::readValues() {
  if (!_available) {
    begin();
    return _available;
  }
  if (!readVector(BNO055::Vector::Accelerometer, _acc, _bno055)) {
    _available = false;
    return _available;
  }
  if (!readVector(BNO055::Vector::Magnetometer, _mag, _bno055)) {
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
  return _available;
}
