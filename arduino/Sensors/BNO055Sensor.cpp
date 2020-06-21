#include "BNO055Sensor.h"

#include <BNO055.h>

BNO055Sensor::BNO055Sensor(uint8_t address, TwoWire *i2cBus) :
    _bno055(NULL),
    _available(false) {
}

BNO055Sensor::~BNO055Sensor() {
}

void BNO055Sensor::begin() {
}

void BNO055Sensor::loop() {
}

bool BNO055Sensor::readValues() {
  return false;
}
