#include "Arduino.h"
#include "DallasSensor.h"

typedef enum {
  DallasCommandConvertTemperature = 0x44,
  DallasCommandMatchRom = 0x55,
  DallasCommandReadScratchpad = 0xBE,
} DallasCommand;

static byte dallasType(const OneWire::Address address) {
  switch (address.data[0]) {
    case DallasSensorTypeDS18S20:
      return 1;
      break;
    case DallasSensorTypeDS18B20:
    case DallasSensorTypeDS1822:
      return 0;
      break;
    default:
      return -1;
  }
}

// static
const char *DallasSensor::sensorType(const OneWire::Address address) {
  const char *result = NULL;
  switch(address.data[0]) {
    case DallasSensorTypeDS18S20:
      result = "DS18S20";
      break;
    case DallasSensorTypeDS18B20:
      result = "DS18B20";
      break;
    case DallasSensorTypeDS1822:
      result = "DS1822";
      break;
  }
  return result;
}

DallasSensor::DallasSensor(const OneWire::Address address, OneWire *oneWire) :
    _oneWire(oneWire),
    _address(address),
    _hasValue(false),
    _celsius(0),
    _timer(0) {
}

const char *DallasSensor::sensorType() const {
  return DallasSensor::sensorType(_address);
}

const char *DallasSensor::copyAddressString() const {
  char *string;
  char *cursor;
  cursor = string = (char *)malloc(24);
  for(unsigned char ii = 0; ii < 8; ii++) {
    sprintf(cursor, "%02x", _address.data[ii]);
    cursor += 2;
    cursor[0] = ':';
    cursor++;
  }
  cursor--;
  cursor[0] = '\0';
  return string;
}

void DallasSensor::sendConvertCommand() {
  _oneWire->reset();
  _oneWire->select(_address);
  _oneWire->write(DallasCommandConvertTemperature, 1);         // Read Scratchpad
}

void DallasSensor::sendReadCommand() {
  byte ii;
  byte data[9];

  _oneWire->reset();
  _oneWire->select(_address);
  _oneWire->write(DallasCommandReadScratchpad);         // Read Scratchpad
  for (ii = 0; ii < 9; ii++) {           // we need 9 bytes
    data[ii] = _oneWire->read();
  }
  unsigned int raw = (data[1] << 8) | data[0];
  if (dallasType(_address)) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
    // default is 12 bit resolution, 750 ms conversion time
  }
  _celsius = (float)raw / 16.0;
  _hasValue = OneWire::crc8(data, 8) == data[8];
}

bool DallasSensor::readValues() {
  this->sendConvertCommand();
  _timer = millis();
  _hasValue = false;
  _tried = false;
  return true;
}

void DallasSensor::loop() {
  if (_tried) {
    return;
  }
  unsigned long currentTime = millis();
  if ((unsigned long)(currentTime - _timer) >= 800) {
    this->sendReadCommand();
    _tried = true;
  }
}
