#ifndef BatteryController_h
#define BatteryController_h

#include "Controller.h"
#include "BatteryError.h"
#include "DallasSensor.h"
#include "INA219Sensor.h"
#include "Value.h"

class ADS1115Driver;
class OneWire;
class TwoWire;

class BatteryController : public Controller {
public:
  BatteryController(ADS1115Driver *ads1115Driver, TwoWire *i2c, OneWire *oneWire);
  ~BatteryController();

  // Controller.
  const char *getName() const override { return "bat"; };
  void begin() override;
  void addSensorsToList(SensorList *sensorList) override;
  void sensorsHasBeenUpdated() override;
  
protected:
  ADS1115Driver *_ads1115Driver;
  INA219Sensor _ina219Sensor;
  DallasSensor _temperatureSensor;
  Value _voltage;
  Value _current;
  Value _temperature;
  Value _balancer0;
  Value _balancer1;
  Value _balancer2;
};

#endif // BatteryController_h
