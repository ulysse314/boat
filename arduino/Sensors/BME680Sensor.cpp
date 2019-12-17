#include "BME680Sensor.h"

#include "Arduino.h"

BME680Sensor::BME680Sensor(TwoWire *i2cBus) :
    _bme680(i2cBus) {
}

void BME680Sensor::begin() {
  _available = _bme680.begin(0x76);
}

void BME680Sensor::loop() {
  if (!_available) {
    _available = _bme680.begin(0x76);
  }
}

bool BME680Sensor::printAddress(Stream *serial) {
  serial->print(_bme680.i2cAddress());
  return true;
}

bool BME680Sensor::printValues(Stream *serial) {
  serial->print(_temperature);
  serial->print(",");
  serial->print(_pressure);
  serial->print(",");
  serial->print(_humidity);
  return true;
}

bool BME680Sensor::readValues() {
  if (_available) {
    _temperature = _bme680.readTemperature();
    _pressure = _bme680.readPressure();
    _humidity = _bme680.readHumidity();
  }
  return _available;
}
