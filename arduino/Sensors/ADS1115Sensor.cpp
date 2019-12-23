#include "ADS1115Sensor.h"

#include <ADS1X15.h>

ADS1115Sensor::ADS1115Sensor() :
    _ads1115(new ADS1115()),
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

bool ADS1115Sensor::printAddress(Stream *serial) {
  serial->print((uint16_t)_ads1115->i2cAddress());
  return true;
}

bool ADS1115Sensor::printValues(Stream *serial) {
  serial->print(_value0);
  serial->print(",");
  serial->print(_value1);
  serial->print(",");
  serial->print(_value2);
  serial->print(",");
  serial->print(_value3);
  return true;
}

bool ADS1115Sensor::readValues() {
  if (_available) {
    _ads1115->readADC_SingleEnded(0, &_value0);
    _ads1115->readADC_SingleEnded(1, &_value1);
    _ads1115->readADC_SingleEnded(2, &_value2);
    _ads1115->readADC_SingleEnded(3, &_value3);
  }
  return _available;
}
