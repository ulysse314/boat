#ifndef MotorController_h
#define MotorController_h

#include "Controller.h"
#include "DallasSensor.h"
#include "Value.h"

class OneWire;
class PWMDriver;

class MotorController : public Controller {
public:
  static MotorController *LeftMotor(OneWire *oneWire, PWMDriver *pwmDriver);
  static MotorController *RightMotor(OneWire *oneWire, PWMDriver *pwmDriver);

  MotorController(const char *name, OneWire *oneWire, PWMDriver *pwmDriver, uint8_t motorID, const uint8_t dallasAddress[8]);
  ~MotorController();

  // Controller
  const char *getName() const override { return _name; };
  void addSensorsToList(SensorList *sensorList) override;
  void sensorsHasBeenUpdated() override;
  void begin() override;

  void setValue(int);
  
protected:
  const char *_name;
  PWMDriver *_pwmDriver;
  DallasSensor *_temperatureSensor;
  uint8_t _motorID;
  Value _power;
  Value _temperature;
};

#endif // MotorController_h
