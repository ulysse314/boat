#ifndef MotorController_h
#define MotorController_h

#include "Controller.h"
#include "DallasSensor.h"
#include "Value.h"

class OneWire;

class MotorController : public Controller {
public:
  static MotorController *LeftMotor(OneWire *oneWire);
  static MotorController *RightMotor(OneWire *oneWire);

  MotorController(const char *name, OneWire *oneWire, const uint8_t dallasAddress[8]);
  ~MotorController();

  // Controller
  const char *getName() const override { return _name; };
  void addSensorsToList(SensorList *sensorList) override;
  void sensorsHasBeenUpdated() override;
  void begin() override;
  
protected:
  const char *_name;
  DallasSensor *_temperatureSensor;
  Value _temperature;
};

#endif // MotorController_h
