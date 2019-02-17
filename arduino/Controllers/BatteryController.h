#ifndef BatteryController_h
#define BatteryController_h

#include "Controller.h"
#include "BatteryError.h"
#include "DallasSensor.h"
#include "INA219Sensor.h"
#include "Value.h"

class OneWire;
class TwoWire;

class BatteryController : public Controller {
public:
  static BatteryController *getBatteryController();
  static bool addBatteryError(BatteryError::Code code);
  static bool removeBatteryError(BatteryError::Code code);

  BatteryController(TwoWire *i2c, OneWire *oneWire);
  virtual ~BatteryController();

  // Controller.
  const char *getName() const override { return "bat"; };
  void begin() override;
  void addSensorsToList(SensorList *sensorList) override;
  void sensorsHasBeenUpdated() override;
  
protected:
  INA219Sensor _ina219Sensor;
  DallasSensor _temperatureSensor;
  Value _voltage;
  Value _current;
  Value _temperature;
};

#endif // BatteryController_h
