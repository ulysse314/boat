#ifndef GPSController_h
#define GPSController_h

#include "Controller.h"
#include "Value.h"

class GPSSensor;

class GPSController : public Controller {
public:
  enum ValueIndex {
    AntennaIndex = 0,
    ViewedSatellitesIndex,
    UsedSatellitesIndex,
    ModeIndex,
    FixQualityIndex,
    LatitudeDegreeIndex,
    LongitudeDegreeIndex,
    AltitudeIndex,
    SpeedIndex,
    AngleIndex,
    GeoidHeightIndex,
    PDOPIndex,
    HDOPIndex,
    VDOPIndex,
    MaxIndex,
  };

  GPSController();
  ~GPSController();
  
  // Controller
  const char *getName() const override { return "gps"; };
  void addSensorsToList(SensorList *sensorList) override;
  void sensorsHasBeenUpdated() override;
  int getValueCount() const override { return MaxIndex; };
  const Value *getValueAtIndex(int index) const override;

protected:
  GPSSensor *_gpsSensor;
  Value _antenna;
  Value _mode;
  Value _viewedSatellites;
  Value _usedSatellites;
  Value _fixQuality;
  Value _latitudeDegree;
  Value _longitudeDegree;
  Value _altitude;
  Value _speed;
  Value _angle;
  Value _geoidHeight;
  Value _pdop;
  Value _hdop;
  Value _vdop;
};

#endif // GPSController_h
