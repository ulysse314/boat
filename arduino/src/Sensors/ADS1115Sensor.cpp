#include "ADS1115Sensor.h"

#include <ADS1X15.h>

ADS1115Sensor::ADS1115Sensor(uint8_t address, TwoWire *i2cBus) :
    _ads1115(new ADS1115(i2cBus, (ADS1X15::I2CAddress)address)),
    _available(false) {
  _ads1115->setDataRate(ADS1115::DataRate::SPS860);
}

ADS1115Sensor::~ADS1115Sensor() {
  delete _ads1115;
}

void ADS1115Sensor::begin() {
  _available = _ads1115->begin();
}

void ADS1115Sensor::loop() {
  if (!_available) {
    _available = _ads1115->begin();
  }
}

bool ADS1115Sensor::readValues() {
  if (_available) {
    _ads1115->readADCSingleEnded(ADS1X15::InputChannel::A0ToGND, &_value0);
    _ads1115->readADCSingleEnded(ADS1X15::InputChannel::A1ToGND, &_value1);
    _ads1115->readADCSingleEnded(ADS1X15::InputChannel::A2ToGND, &_value2);
    _ads1115->readADCSingleEnded(ADS1X15::InputChannel::A3ToGND, &_value3);
  }
  return _available;
}
