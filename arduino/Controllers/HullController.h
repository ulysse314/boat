#ifndef HullController_h
#define HullController_h

#include "Controller.h"
#include "Value.h"

class ADS1115Sensor;
class HardwareConfig;
class BME680Sensor;

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
  Value _water;
  Value _temperature;
  Value _humidity;
  Value _pressure;
};

#endif // HullController_h
