#ifndef HullController_h
#define HullController_h

#include "Controller.h"

class AnalogSensor;

class HullController : public Controller {
public:
  HullController();
  virtual ~HullController();
  virtual void addSensorsToList(SensorList *sensorList) override;
  
protected:
  AnalogSensor *_waterSensor;
};

#endif // HullController_h
