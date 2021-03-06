#ifndef GPSController_h
#define GPSController_h

#include "../Controllers/Controller.h"
#include "../Controllers/Value.h"

class GPSSensor;
class HardwareConfig;

class GPSController : public Controller {
public:
  GPSController(HardwareConfig *hardwareConfig);
  ~GPSController();

  // Controller
  const char *name() const override { return "gps"; };
  void begin() override;
  void addSensorsToList(SensorList *sensorList) override;
  void sensorsHasBeenUpdated() override;

  double getLatitudeDegree() const { return _latitudeDegree.valueAsDouble(); };
  double getLongitudeDegree() const { return _longitudeDegree.valueAsDouble(); };

protected:
  HardwareConfig *_hardwareConfig;
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
  Value _heading;
  Value _geoidHeight;
  Value _pdop;
  Value _hdop;
  Value _vdop;
};

#endif // GPSController_h
