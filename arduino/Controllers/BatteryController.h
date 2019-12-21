#ifndef BatteryController_h
#define BatteryController_h

#include "Controller.h"
#include "BatteryError.h"
#include "DallasSensor.h"
#include "INA219Sensor.h"
#include "Value.h"

class ADS1115Sensor;
class OneWire;
class TwoWire;

class BatteryController : public Controller {
public:
  BatteryController(ADS1115Sensor *ads1115Sensor, TwoWire *i2c, OneWire *oneWire);
  ~BatteryController();

  // Controller.
  const char *getName() const override { return "bat"; };
  void begin() override;
  void addSensorsToList(SensorList *sensorList) override;
  void sensorsHasBeenUpdated() override;
  
protected:
  ADS1115Sensor *_ads1115Sensor;
  INA219Sensor _ina219Sensor;
  DallasSensor _batteryTemperatureSensor;
  DallasSensor _balancerTemperatureSensor;
  Value _voltage;
  Value _current;
  Value _batteryTemperature;
  Value _balancerTemperature;
  Value _balancer0;
  Value _balancer1;
  Value _balancer2;
};

#endif // BatteryController_h
