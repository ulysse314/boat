#ifndef BatteryController_h
#define BatteryController_h

#include "Controller.h"

class AnalogSensor;
class DallasSensor;
class OneWire;

class BatteryController : public Controller {
public:
  BatteryController(OneWire *oneWire);
  virtual ~BatteryController();
  virtual void addSensorsToList(SensorList *sensorList) override;
  
protected:
  DallasSensor *_temperatureSensor;
  AnalogSensor *_currentSensor;
  AnalogSensor *_tensionSensor;
};

#endif // BatteryController_h
