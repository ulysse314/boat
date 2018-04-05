#include "GPSSensor.h"

#include <Adafruit_GPS.h>

#define GPSSerial Serial1

const char *GPSSensor::sensorClass() {
  return "GPS";
}

const char *GPSSensor::sensorType() {
  return "GPS";
}

bool GPSSensor::begin() {
  _gps = new Adafruit_GPS(&GPSSerial);
  _gps->begin(9600);
  _gps->sendCommand(PMTK_SET_BAUD_115200);
  delay(1000);
  GPSSerial.end();
  delay(1000);
  _gps->begin(115200);
  _gps->sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  _gps->sendCommand(PGCMD_ANTENNA);
  return true;
}

boolean GPSSensor::loop() {
  if (GPSSerial.available()) {
    char c = _gps->read();
    if (_gps->newNMEAreceived()) {
      const char *nmea = _gps->lastNMEA();
      _gps->parse(nmea);
    }
  }
  return true;
}

boolean GPSSensor::printAddress(Stream *serial) {
  serial->print("serial1");
  return true;
}

boolean GPSSensor::printValues(Stream *serial) {
  serial->print(_gps->latitude_degree); // 3
  serial->print(" ");
  serial->print(_gps->longitude_degree); // 4
  serial->print(" ");
  serial->print(_gps->altitude); // 5
  serial->print(" ");
  serial->print(_gps->speed); // 6
  serial->print(" ");
  serial->print(_gps->angle); // 7
  serial->print(" ");
  serial->print(_gps->PDOP); // 8
  serial->print(" ");
  serial->print(_gps->VDOP); // 9
  serial->print(" ");
  serial->print(_gps->VDOP); // 10
  serial->print(" ");
  serial->print(_gps->satellites_used); // 11
  serial->print(" ");
  serial->print(_gps->satellites_in_views); // 12
  serial->print(" ");
  serial->print(_gps->mode); // 13
  serial->print(" ");
  serial->print(_gps->fix_quality); // 14
  return true;
}
