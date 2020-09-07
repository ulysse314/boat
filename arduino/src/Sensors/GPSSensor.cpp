#include "GPSSensor.h"

#include <Arduino.h>
#include <MTK3339.h>

GPSSensor::GPSSensor(Stream *serial) :
    _serial(serial),
    _mtk3339(new MTK3339(serial)) {
}

GPSSensor::~GPSSensor() {
  delete(_mtk3339);
}

void GPSSensor::switchToHighSpeed() {
  _mtk3339->sendCommand(PMTK_SET_BAUD_115200);
}

void GPSSensor::highSpeedSwitchDone() {
  _mtk3339->sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  _mtk3339->sendCommand(PGCMD_ANTENNA);
}

void GPSSensor::begin() {
  _mtk3339->begin();
}

void GPSSensor::loop() {
  if (_serial->available()) {
    _mtk3339->read();
    if (_mtk3339->newNMEAreceived()) {
      const char *nmea = _mtk3339->lastNMEA();
      _mtk3339->parse(nmea);
    }
  }
}
