#include "GPSSensor.h"

#include <Adafruit_GPS.h>

#define GPSSerial Serial1

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

bool GPSSensor::loop() {
  if (GPSSerial.available()) {
    _gps->read();
    if (_gps->newNMEAreceived()) {
      const char *nmea = _gps->lastNMEA();
      _gps->parse(nmea);
    }
  }
  return true;
}

bool GPSSensor::printAddress(Stream *serial) {
  serial->print("serial1");
  return true;
}

bool GPSSensor::printValues(Stream *serial) {
  serial->print((int)_gps->antenna); // 3
  serial->print(" ");
  serial->print(_gps->satellites_in_views); // 4
  serial->print(" ");
  serial->print(_gps->satellites_used); // 5
  serial->print(" ");
  serial->print(_gps->mode); // 6
  if (_gps->fix) {
    serial->print(" ");
    serial->print(_gps->fix_quality); // 7
    serial->print(" ");
    serial->print(_gps->latitude_degree); // 8
    serial->print(" ");
    serial->print(_gps->longitude_degree); // 9
    serial->print(" ");
    serial->print(_gps->altitude); // 10
    serial->print(" ");
    serial->print(_gps->speed); // 11
    serial->print(" ");
    serial->print(_gps->angle); // 12
    serial->print(" ");
    serial->print(_gps->PDOP); // 13
    serial->print(" ");
    serial->print(_gps->VDOP); // 14
    serial->print(" ");
    serial->print(_gps->VDOP); // 15
  }
  return true;
}
