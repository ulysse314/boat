#ifndef HullController_h
#define HullController_h

#include "Controller.h"
#include "Value.h"

class ADS1115Sensor;
class BME680Sensor;
class BNO055Sensor;
class HardwareConfig;

class HullController : public Controller {
public:
  HullController(ADS1115Sensor *ads1115Sensor, HardwareConfig *hardwareConfig);
  virtual ~HullController();

  const char *name() const override { return "hll"; };
  void begin() override;
  void addSensorsToList(SensorList *sensorList) override;
  void sensorsHasBeenUpdated() override;

protected:
  ADS1115Sensor *_ads1115Sensor;
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
};

#endif // HullController_h
