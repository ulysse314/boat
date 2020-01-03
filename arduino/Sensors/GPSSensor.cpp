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
