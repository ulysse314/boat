#ifndef MotorController_h
#define MotorController_h

#include "Controller.h"
#include "DallasSensor.h"
#include "Value.h"

class Adafruit_PWMServoDriver;
class OneWire;

class MotorController : public Controller {
public:
  static MotorController *LeftMotor(OneWire *oneWire, Adafruit_PWMServoDriver *pwmDriver);
  static MotorController *RightMotor(OneWire *oneWire, Adafruit_PWMServoDriver *pwmDriver);

  MotorController(const char *name, OneWire *oneWire, Adafruit_PWMServoDriver *pwmDriver, const uint8_t dallasAddress[8]);
  ~MotorController();

  // Controller
  const char *getName() const override { return _name; };
  void addSensorsToList(SensorList *sensorList) override;
  void sensorsHasBeenUpdated() override;
  void begin() override;
  void setValue(int value) override;
  
protected:
  const char *_name;
  Adafruit_PWMServoDriver *_pwmDriver;
  DallasSensor *_temperatureSensor;
  Value _temperature;
};

#endif // MotorController_h
