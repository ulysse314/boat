#include "GPSController.h"

#include <Adafruit_GPS.h>

#include "GPSError.h"
#include "GPSSensor.h"
#include "SensorList.h"

#define kUsedSatelliteTooLow 3
#define kUsedSatelliteLow 5

GPSController::GPSController() :
    _antenna(Value::Type::Integer, "ant"),
    _mode(Value::Type::Integer, "mod"),
    _viewedSatellites(Value::Type::Integer, "vst"),
    _usedSatellites(Value::Type::Integer, "ust"),
    _fixQuality(Value::Type::Integer, "fxq"),
    _latitudeDegree(Value::Type::Integer, "lat"),
    _longitudeDegree(Value::Type::Integer, "lon"),
    _altitude(Value::Type::Double, "alt"),
    _speed(Value::Type::Double, "spd"),
    _angle(Value::Type::Double, "ang"),
    _geoidHeight(Value::Type::Double, "gh"),
    _pdop(Value::Type::Double, "pdp"),
    _hdop(Value::Type::Double, "hdp"),
    _vdop(Value::Type::Double, "vdp") {
  _gpsSensor = new GPSSensor();
}

GPSController::~GPSController() {
}

void GPSController::begin() {
  addValue(&_antenna);
  addValue(&_mode);
  addValue(&_viewedSatellites);
  addValue(&_usedSatellites);
  addValue(&_fixQuality);
  addValue(&_latitudeDegree);
  addValue(&_longitudeDegree);
  addValue(&_altitude);
  addValue(&_speed);
  addValue(&_angle);
  addValue(&_geoidHeight);
  addValue(&_pdop);
  addValue(&_hdop);
  addValue(&_vdop);
}

void GPSController::addSensorsToList(SensorList *sensorList) {
  sensorList->addSensor(_gpsSensor);
}

void GPSController::sensorsHasBeenUpdated() {
  Adafruit_GPS *gps = _gpsSensor->getGPS();
  switch (gps->antenna) {
    case Adafruit_GPS::AntennaUnknown:
      addError(new GPSError(GPSError::CodeUnknownAntenna));
      break;
    case Adafruit_GPS::AntennaExternalProblem:
      addError(new GPSError(GPSError::CodeExternalAntennaProblem));
      break;
    case Adafruit_GPS::AntennaUsingInternal:
      addError(new GPSError(GPSError::CodeUsingInternalAntenna));
      break;
    case Adafruit_GPS::AntennaUsingExternal:
      break;
  }
  _antenna.setInteger(gps->antenna);
  switch (gps->mode) {
    case Adafruit_GPS::ModeNoFix:
      addError(new GPSError(GPSError::CodeNoFixMode));
      break;
    case Adafruit_GPS::Mode2D:
      addError(new GPSError(GPSError::Code2DMode));
      break;
    case Adafruit_GPS::Mode3D:
      break;
  }
  _mode.setInteger(gps->mode);
  _viewedSatellites.setInteger(gps->satellites_in_views);
  _usedSatellites.setInteger(gps->satellites_used);
  if (gps->satellites_used <= kUsedSatelliteTooLow) {
    addError(new GPSError(GPSError::CodeUsedSatellitesTooLow));
  } else if (gps->satellites_used <= kUsedSatelliteLow) {
    addError(new GPSError(GPSError::CodeUsedSatellitesLow));
  }
  if (gps->fix) {
    _fixQuality.setInteger(gps->fix_quality);
    _latitudeDegree.setInteger(gps->latitude_degree);
    _longitudeDegree.setInteger(gps->longitude_degree);
    _altitude.setDouble(gps->altitude);
    _speed.setDouble(gps->speed);
    _angle.setDouble(gps->angle);
    _geoidHeight.setDouble(gps->geoidheight);
    _pdop.setDouble(gps->PDOP);
    _hdop.setDouble(gps->HDOP);
    _vdop.setDouble(gps->VDOP);
  } else {
    _fixQuality.setNull();
    _latitudeDegree.setNull();
    _longitudeDegree.setNull();
    _altitude.setNull();
    _speed.setNull();
    _angle.setNull();
    _geoidHeight.setNull();
    _pdop.setNull();
    _hdop.setNull();
    _vdop.setNull();
  }
}
