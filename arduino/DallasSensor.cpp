#include "Arduino.h"
#include "DallasSensor.h"

DallasSensor::DallasSensor(uint8_t address[8], OneWire *oneWire) {
  _oneWire = oneWire;
  memcpy(_address, address, sizeof(_address));
  _state = DallasSensorStateReset;
  _celsius = 20.0;
  _gotFirstValue = false;
}

static byte dallasType(uint8_t address[8]) {
  switch (address[0]) {
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

const char *DallasSensor::sensorType(const uint8_t address[8]) {
  const char *result = NULL;
  
  switch(address[0]) {
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

const char *DallasSensor::sensorType() const {
  return DallasSensor::sensorType(_address);
}

const char *DallasSensor::copyAddressString() const {
  char *string;
  char *cursor;
  char ii;
  
  cursor = string = (char *)malloc(24);
  for(ii = 0; ii < 8; ii++) {
    sprintf(cursor, "%02x", _address[ii]);
    cursor += 2;
    cursor[0] = ':';
    cursor++;
  }
  cursor--;
  cursor[0] = '\0';
  return string;
}

bool DallasSensor::printAddress(Stream *serial) {
  byte ii;
  for(ii = 0; ii < 8; ii++) {
    if (_address[ii] < 16) {
      serial->print("0");
    }
    serial->print(_address[ii], HEX);
    if (ii < 7) {
      serial->print(':');
    }
  }
  return true;
}

bool DallasSensor::printValues(Stream *serial) {
  char buffer[32];
  sprintf(buffer, "%.2f", this->celsius());
  serial->print(buffer);
  return true;
}

void DallasSensor::sendConvertCommand() {
  _oneWire->reset();
  _oneWire->select(_address);    
  _oneWire->write(0x44,1);         // Read Scratchpad
}

void DallasSensor::sendReadCommand() {
  byte ii;
  byte data[12];

  _oneWire->reset();
  _oneWire->select(_address);    
  _oneWire->write(0xBE);         // Read Scratchpad
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
}

bool DallasSensor::loop() {
  switch(_state) {
      case DallasSensorStateReset:
          this->sendConvertCommand();
          _state = DallasSensorStateResetWait;
          _timer = millis();
          break;
      case DallasSensorStateResetWait:
          unsigned long currentTime = millis();
          
          if ((unsigned long)(currentTime - _timer) >= 1000) {
              this->sendReadCommand();
              _state = DallasSensorStateReset;
              _gotFirstValue = true;
          }
          break;
  }
  return true;
}
