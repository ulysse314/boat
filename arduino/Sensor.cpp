#include "Sensor.h"

boolean Sensor::printInfo(Stream *serial, int nodeID) {
  const char *string;

  string = this->sensorClass();
  if (!string) {
    return false;
  } else {
    boolean result;
  
    if (nodeID != -1) {
      serial->print(nodeID);
      serial->print(" ");
    }
    serial->print(this->sensorType());
    serial->print(" ");
    this->printAddress(serial);
    serial->print(" ");
    result = this->printValues(serial);
    serial->println("");
    return result;
  }
}

boolean Sensor::printAddress(Stream *serial) {
  const char *string;
  boolean result = false;

  string = this->addressString();
  if (string && string[0] != 0) {
    serial->print(string);
    result = true;
  } else {
    string = this->copyAddressString();
    if (string && string[0] != 0) {
      serial->print(string);
      result = true;
      free((void *)string);
    }
  }
  return result;
}
