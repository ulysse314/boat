#ifndef BatteryController_h
#define BatteryController_h

#include "../Controllers/Controller.h"
#include "../Controllers/Value.h"
#include "../Errors/BatteryError.h"
#include "../Sensors/DallasSensor.h"
#include "../Sensors/INA219Sensor.h"

class ADS1115Sensor;
class HardwareConfig;

class BatteryController : public Controller {
public:
  BatteryController(ADS1115Sensor *ads1115Sensor, HardwareConfig *hardwareConfig);
  ~BatteryController();

  // Controller.
  const char *name() const override { return "batt"; };
  void begin() override;
  void addSensorsToList(SensorList *sensorList) override;
  void sensorsHasBeenUpdated() override;

  void addCurrentConsumption(double initialConsumption);
  void resetCurrentConsumption();

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
  Value _currentConsumption;
  unsigned long long _lastCurrentValueTimestamp;
};

#endif // BatteryController_h
