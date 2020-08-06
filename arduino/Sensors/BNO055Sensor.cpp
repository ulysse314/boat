#include "BNO055Sensor.h"

#include <BNO055.h>

namespace {

bool readVector(BNO055::adafruit_vector_type_t vectorType, BNO055Sensor::Vector &vector, BNO055 *bno055) {
  imu::Vector<3> xyz;
  if (!bno055->getVector(vectorType, &xyz)) {
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
  }
}

void BNO055Sensor::loop() {
  if (!_available) {
    _available = _bno055->begin();
    return;
  }
  if (!readVector(BNO055::VECTOR_ACCELEROMETER, _acc, _bno055)) {
    _available = false;
    return;
  }
  if (!readVector(BNO055::VECTOR_MAGNETOMETER, _mag, _bno055)) {
    _available = false;
    return;
  }
  if (!readVector(BNO055::VECTOR_GYROSCOPE, _gyro, _bno055)) {
    _available = false;
    return;
  }
  if (!readVector(BNO055::VECTOR_EULER, _euler, _bno055)) {
    _available = false;
    return;
  }
}

bool BNO055Sensor::readValues() {
  return _available;
}
