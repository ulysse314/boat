#ifndef MotorController_h
#define MotorController_h

#include "Controller.h"

class DallasSensor;
class OneWire;

class MotorController : public Controller {
public:
  MotorController(OneWire *oneWire);
  virtual ~MotorController();
  virtual void addSensorsToList(SensorList *sensorList) override;
  
protected:
  DallasSensor *_leftTemperatureSensor;
  DallasSensor *_rightTemperatureSensor;
};

#endif // MotorController_h
