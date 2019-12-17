#include "ADS1115Sensor.h"

ADS1115Sensor::ADS1115Sensor() :
    _ads1115(),
    _available(false) {
}

void ADS1115Sensor::begin() {
  _available = _ads1115.begin();
}

void ADS1115Sensor::loop() {
  if (!_available) {
    _available = _ads1115.begin();
  }
}

bool ADS1115Sensor::printAddress(Stream *serial) {
  serial->print(_ads1115.i2cAddress());
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
    _value0 = _ads1115.readADC_SingleEnded(0);
    _value1 = _ads1115.readADC_SingleEnded(1);
    _value2 = _ads1115.readADC_SingleEnded(2);
    _value3 = _ads1115.readADC_SingleEnded(3);
  }
  return _available;
}
