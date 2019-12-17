#ifndef HullController_h
#define HullController_h

#include "Controller.h"
#include "Value.h"

class ADS1115Sensor;
class BME680Sensor;
class TwoWire;

class HullController : public Controller {
public:
  HullController(ADS1115Sensor *ads1115Sensor, TwoWire *i2cBus);
  virtual ~HullController();

  const char *getName() const override { return "hull"; };
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
