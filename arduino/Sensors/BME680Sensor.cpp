#include "BME680Sensor.h"

#include "Arduino.h"

BME680Sensor::BME680Sensor(uint8_t address, TwoWire *i2cBus) :
    _bme680(address, i2cBus),
    _available(false),
    _waitingForData(false),
    _temperature(0),
    _humidity(0),
    _pressure(0) {
}

void BME680Sensor::begin() {
  _available = _bme680.begin();
}

void BME680Sensor::loop() {
  if (!_available) {
    _available = _bme680.begin();
    return;
  }
  if (_waitingForData && _bme680.remainingReadingMillis() == _bme680.reading_complete) {
    _waitingForData = false;
    _temperature = _bme680.temperature;
    _humidity = _bme680.humidity;
    _pressure = _bme680.pressure;
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
    _bme680.beginReading();
    _waitingForData = true;
  }
  return _available;
}
