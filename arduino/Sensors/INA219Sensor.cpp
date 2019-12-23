#include "INA219Sensor.h"

#include <INA219.h>

INA219Sensor::INA219Sensor(float shuntValue, float maxCurrent, uint8_t address, TwoWire *i2c) :
    _ina219(new INA219(address, i2c)),
    _shuntValue(shuntValue),
    _maxCurrent(maxCurrent),
    _hasValue(false),
    _voltage(0),
    _current(0) {
}

INA219Sensor::~INA219Sensor() {
  delete _ina219;
}

void INA219Sensor::begin() {
  _hasValue = _ina219->begin();
  _hasValue = _hasValue && _ina219->configure(INA219::RANGE_16V, INA219::GAIN_2_80MV, INA219::ADC_64SAMP, INA219::ADC_64SAMP, INA219::CONT_SH_BUS);
  _hasValue = _hasValue && _ina219->calibrate(_shuntValue, _maxCurrent);
  if (!_hasValue) {
    _ina219->reset();
  }
}

void INA219Sensor::loop() {
  if (!_hasValue) {
    begin();
  }
}

bool INA219Sensor::printAddress(Stream *serial) {
  serial->print(_ina219->i2cAddress());
  return true;
}

bool INA219Sensor::printValues(Stream *serial) {
  serial->print(_voltage);
  serial->print(",");
  serial->print(_current);
  return true;
}

bool INA219Sensor::readValues() {
  if (_hasValue) {
    _hasValue = _ina219->busVoltage(&_voltage);
  }
  if (_hasValue) {
    _hasValue = _ina219->shuntCurrent(&_current);
  }
  return _hasValue;
}
