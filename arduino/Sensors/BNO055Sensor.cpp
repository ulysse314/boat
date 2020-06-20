#include "BNO055Sensor.h"

#include <ADS1X15.h>

BNO055Sensor::BNO055Sensor(uint8_t address, TwoWire *i2cBus) :
    _ads1115(new BNO055((ADS1X15::I2CAddress)address, i2cBus)),
    _available(false) {
  _ads1115->setDataRate(BNO055::DataRate::SPS860);
}

BNO055Sensor::~BNO055Sensor() {
  delete _bno055;
}

void BNO055Sensor::begin() {
  _available = _ads1115->begin();
}

void BNO055Sensor::loop() {
  if (!_available) {
    _available = _ads1115->begin();
  }
}

bool BNO055Sensor::readValues() {
  if (_available) {
    _ads1115->readADC_SingleEnded(0, &_value0);
    _ads1115->readADC_SingleEnded(1, &_value1);
    _ads1115->readADC_SingleEnded(2, &_value2);
    _ads1115->readADC_SingleEnded(3, &_value3);
  }
  return _available;
}
