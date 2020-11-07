#ifndef HullController_h
#define HullController_h

#include "../Controllers/Controller.h"
#include "../Controllers/Value.h"

class BME680Sensor;
class BNO055Sensor;
class HardwareConfig;

class HullController : public Controller {
public:
  HullController(HardwareConfig *hardwareConfig);
  virtual ~HullController();

  const char *name() const override { return "hll"; };
  void begin() override;
  void addSensorsToList(SensorList *sensorList) override;
  void sensorsHasBeenUpdated() override;

protected:
  const uint8_t _leakPin;
  BME680Sensor *_bme680Sensor;
  BNO055Sensor *_bno055Sensor;
  Value _water;
  Value _temperature;
  Value _humidity;
  Value _pressure;
  Value _accX;
  Value _accY;
  Value _accZ;
  Value _magX;
  Value _magY;
  Value _magZ;
  Value _gyroX;
  Value _gyroY;
  Value _gyroZ;
  Value _eulerX;
  Value _eulerY;
  Value _eulerZ;
  Value _heading;
  Value _averageHeading;
};

#endif // HullController_h
