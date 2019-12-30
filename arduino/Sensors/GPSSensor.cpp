#include "GPSSensor.h"

#include <MTK3339.h>

#define GPSSerial Serial1

void GPSSensor::begin() {
  _mtk3339 = new MTK3339(&GPSSerial);
  _mtk3339->begin(9600);
  _mtk3339->sendCommand(PMTK_SET_BAUD_115200);
  delay(1000);
  GPSSerial.end();
  delay(1000);
  _mtk3339->begin(115200);
  _mtk3339->sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  _mtk3339->sendCommand(PGCMD_ANTENNA);
}

void GPSSensor::loop() {
  if (GPSSerial.available()) {
    _mtk3339->read();
    if (_mtk3339->newNMEAreceived()) {
      const char *nmea = _mtk3339->lastNMEA();
      _mtk3339->parse(nmea);
    }
  }
}

bool GPSSensor::printAddress(Stream *serial) {
  serial->print("serial1");
  return true;
}

bool GPSSensor::printValues(Stream *serial) {
  serial->print((int)_mtk3339->antenna); // 3
  serial->print(" ");
  serial->print(_mtk3339->satellites_in_views); // 4
  serial->print(" ");
  serial->print(_mtk3339->satellites_used); // 5
  serial->print(" ");
  serial->print(_mtk3339->mode); // 6
  if (_mtk3339->fix) {
    serial->print(" ");
    serial->print(_mtk3339->fix_quality); // 7
    serial->print(" ");
    serial->print(_mtk3339->latitude_degree); // 8
    serial->print(" ");
    serial->print(_mtk3339->longitude_degree); // 9
    serial->print(" ");
    serial->print(_mtk3339->altitude); // 10
    serial->print(" ");
    serial->print(_mtk3339->speed); // 11
    serial->print(" ");
    serial->print(_mtk3339->angle); // 12
    serial->print(" ");
    serial->print(_mtk3339->PDOP); // 13
    serial->print(" ");
    serial->print(_mtk3339->VDOP); // 14
    serial->print(" ");
    serial->print(_mtk3339->VDOP); // 15
  }
  return true;
}
