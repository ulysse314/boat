#include "BME680Sensor.h"

#include <Arduino.h>

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
  if (_available) {
    _bme680.setTemperatureOversampling(BME680_OS_8X);
    _bme680.setHumidityOversampling(BME680_OS_2X);
    _bme680.setPressureOversampling(BME680_OS_4X);
    _bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);
    _bme680.setGasHeater(320, 150); // 320*C for 150 ms
  }
}

void BME680Sensor::loop() {
  if (!_available) {
    _available = _bme680.begin();
    return;
  }
  if (_waitingForData && _bme680.remainingReadingMillis() == 0) {
    _bme680.endReading();
    _waitingForData = false;
    _temperature = _bme680.temperature;
    _humidity = _bme680.humidity;
    _pressure = _bme680.pressure / 100.;
  }
}

bool BME680Sensor::readValues() {
  if (_available) {
    _bme680.beginReading();
    _waitingForData = true;
  }
  return _available;
}
